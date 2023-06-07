// [CFXS] //
#include <cmath>
#include <CFXS/Base/Debug.hpp>
#include <CFXS/Base/Time.hpp>
#include <CFXS/Base/Memory.hpp>
#include <CFXS/Platform/App.hpp>
#include <CFXS/Platform/CPU.hpp>
#include <CFXS/Platform/Task.hpp>
#include <CFXS/Platform/STM32/CoreInit.hpp>
#include <CFXS/Base/ByteOrder.hpp>
#include <CFXS/Base/Math.hpp>
#include "stm32g4xx_hal.h"

#include <arm_math.h>
#include <Hardware/Buttons.hpp>
#include <Hardware/Display.hpp>
#include <UI/TextObject.hpp>

#define ADC_MAX 0x1FFF
#define R_SHUNT 1.5

using CFXS::Task;
using namespace CFXS;

extern "C" void SystemInit(); // HAL

extern void SystemClock_Config();
extern void MX_DMA_Init();
extern void MX_GPIO_Init(void);
extern void MX_CORDIC_Init(void);
extern void MX_CRC_Init(void);
extern void MX_FMAC_Init(void);
extern void MX_RNG_Init(void);
extern void MX_ADC1_Init(void);
extern void MX_SPI3_Init(void);
extern void MX_DAC1_Init(void);
extern void MX_ADC5_Init(void);
extern void MX_TIM6_Init(void);
extern void MX_TIM1_Init(void);

constexpr uint16_t RGB888_To_RGB565(uint8_t r, uint8_t g, uint8_t b) {
    return CFXS::SwapByteOrder16(((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3));
}

void __cfxs_startup() {
    SystemInit();
}

extern DMA_HandleTypeDef hdma_dac1_ch2;
extern DMA_HandleTypeDef hdma_adc5;

extern DAC_HandleTypeDef hdac1;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim1;

// DAC DMA
void __cfxs_interrupt_27() {
    HAL_DMA_IRQHandler(&hdma_dac1_ch2);
}

// ADC DMA
void __cfxs_interrupt_28() {
    HAL_DMA_IRQHandler(&hdma_adc5);
}

void InitializeModules() {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_CORDIC_Init();
    MX_CRC_Init();
    MX_FMAC_Init();
    MX_RNG_Init();
    MX_ADC1_Init();
    MX_SPI3_Init();
    MX_ADC5_Init();
    MX_DAC1_Init();
    MX_TIM6_Init();
    MX_TIM1_Init();

    Task::AddGroups({
        {LOW_PRIORITY, 32},
        {HIGH_PRIORITY, 4},
        {SYSTEM_PRIORITY, 2},
    });
}

int wave_lut_size = 0;

static uint16_t Wave_LUT[1024] = {};

int last_div                   = 0;
static auto hz_to_timer_period = [](float hz) {
    return (int)(CFXS::CPU::CLOCK_FREQUENCY / ((float)wave_lut_size * hz));
};

// ADC sampling time
// 170000000 / 4 (ADC clock div) / 16 (oversampling) / (24*2) (sampling time/ch)
// ~55ksps max

// ADC sampling clock
// 40kHz events
// 170000000 / 40000 = 4250
// Timer period: 4250-1 = 4249

bool adc_trig = false;

extern ADC_HandleTypeDef hadc5;
#define NUM_SAMPLES (256 * 2)
uint16_t adc_buf[NUM_SAMPLES * 2];
float reading_voltage[NUM_SAMPLES];
float reading_current[NUM_SAMPLES];

int s_ConvDelay = 0;

int s_HZ_Index;
int s_Stop;
#define HZ_INDEX_COUNT (400)

static float s_HZ_Table[HZ_INDEX_COUNT] = {};

float s_Impedance[HZ_INDEX_COUNT];
float s_Phase[HZ_INDEX_COUNT];

#include <complex>

extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    HAL_ADC_Stop_DMA(&hadc5);
    adc_trig = false;

    // remove DC offset
    int avg_u = 0;
    int avg_i = 0;
    for (int i = 0; i < NUM_SAMPLES * 2; i += 2) {
        avg_u += adc_buf[i];
        avg_i += adc_buf[i + 1];
    }
    avg_u /= NUM_SAMPLES;
    avg_i /= NUM_SAMPLES;
    int nidx              = 0;
    float amplitude_u_rms = 0;
    float amplitude_i_rms = 0;
    for (int i = 0; i < NUM_SAMPLES * 2; i += 2) {
        reading_voltage[nidx] = (adc_buf[i] - avg_u) * (3.3f / ADC_MAX);
        reading_current[nidx] = ((adc_buf[i + 1] - avg_i) * (3.3f / ADC_MAX)) / 10; // gain = 10
        amplitude_u_rms += reading_voltage[nidx] * reading_voltage[nidx];
        amplitude_i_rms += reading_current[nidx] * reading_current[nidx];
        nidx++;
    }

    amplitude_u_rms = sqrtf(amplitude_u_rms / NUM_SAMPLES);
    amplitude_i_rms = sqrtf(amplitude_i_rms / NUM_SAMPLES);

    bool start_zero_cross_find_u = false;
    bool start_zero_cross_find_i = false;
    int idx_zero_cross_u         = 0; // voltage zero cross
    int idx_zero_cross_i         = 0; // current zero cross
    int idx_zero_cross_back_u    = 0; // voltage half period cross back
    for (int i = 0; i < NUM_SAMPLES; i++) {
        if (start_zero_cross_find_u == false) {
            if (reading_voltage[i] >= amplitude_u_rms) {
                start_zero_cross_find_u = true;
            }
        } else if (!idx_zero_cross_u) {
            if (reading_voltage[i] <= 0) {
                idx_zero_cross_u = i;
            }
        } else if (!idx_zero_cross_back_u) {
            if (reading_voltage[i] >= 0) {
                idx_zero_cross_back_u = i;
            }
        }

        if (start_zero_cross_find_u) {
            if (start_zero_cross_find_i == false) {
                if (reading_current[i] >= amplitude_i_rms) {
                    start_zero_cross_find_i = true;
                }
            } else if (!idx_zero_cross_i) {
                if (reading_current[i] <= 0) {
                    idx_zero_cross_i = i;
                }
            }
        }
    }

#undef PI
    amplitude_u_rms *= 1.3295; // 1.3295 fast calibration for 10k load
    static float last_phase = 0;
    if (s_HZ_Index == 0) {
        last_phase = 0;
    }
    float phase = std::lerp(
        last_phase, (((CFXS::Math::PI / (idx_zero_cross_back_u - idx_zero_cross_u)) * (idx_zero_cross_i - idx_zero_cross_u))), 0.9);
    last_phase = phase;
    float r    = (amplitude_u_rms / amplitude_i_rms);

    // s_Impedance[s_HZ_Index]  = (r - 1.5); // current measurement resistor 1.5R
    s_Phase[s_HZ_Index] = phase;

    auto impedance          = (amplitude_u_rms / (amplitude_i_rms * std::polar(1.0f, phase)));
    s_Impedance[s_HZ_Index] = (impedance.real() - R_SHUNT);

    s_HZ_Index++;
    s_ConvDelay = 0;
}

#undef PI
#define WAVE_MAG 550 / 2
void CalculateWaveLUT(int freq_low) {
    // if (freq_low >= 1000) {
    //     if (wave_lut_size != 512) {
    //         wave_lut_size = 512;
    //         for (int i = 0; i < wave_lut_size; i++) {
    //             Wave_LUT[i] = cosf((CFXS::Math::PI * 2) / wave_lut_size * i + CFXS::Math::PI) * WAVE_MAG + 0x7FF;
    //         }
    //     }
    // } else {
    if (wave_lut_size != 256) {
        wave_lut_size = 256;
        for (int i = 0; i < wave_lut_size; i++) {
            Wave_LUT[i] = sinf((CFXS::Math::PI * 2) / wave_lut_size * i) * WAVE_MAG + WAVE_MAG + 200;
        }
    }
    // }
}

extern "C" void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef* hdac) {
    if (adc_trig)
        return;
    if (s_HZ_Index >= s_Stop) {
        HAL_TIM_Base_Stop(&htim6);
        return;
    }

    htim6.Init.Period = hz_to_timer_period(s_HZ_Table[s_HZ_Index]);
    HAL_TIM_Base_Init(&htim6);
    // HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_2);
    HAL_TIM_Base_Start(&htim6);
    HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_2, (uint32_t*)Wave_LUT, wave_lut_size, DAC_ALIGN_12B_R);

    if (!adc_trig && s_ConvDelay == 0) {
        htim1.Init.Period = htim6.Init.Period;
        HAL_TIM_Base_Init(&htim1);
        HAL_TIM_Base_Start(&htim1);

        adc_trig = true;
        HAL_ADC_Start_DMA(&hadc5, (uint32_t*)adc_buf, NUM_SAMPLES * 2);
        s_ConvDelay = 0;
    }

    if (s_ConvDelay)
        s_ConvDelay--;
}

void RestartMeasurement() {
    memset(s_Impedance, 0, sizeof(s_Impedance));
    memset(s_Phase, 0, sizeof(s_Phase));
    Display::Clear();
    adc_trig          = false;
    s_HZ_Index        = 0;
    s_Stop            = HZ_INDEX_COUNT;
    htim6.Init.Period = hz_to_timer_period(10);
    s_ConvDelay       = 10;
    HAL_TIM_Base_Init(&htim6);
    HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_2, (uint32_t*)Wave_LUT, wave_lut_size, DAC_ALIGN_12B_R);
    HAL_TIM_Base_Start(&htim6);
}

static TextObject s_TextR1(Vector2<int16_t>{0, 0});
static TextObject s_TextR2(Vector2<int16_t>{0, 0 + 20});
static TextObject s_TextR3(Vector2<int16_t>{320 * 0.75, 0});

ADC_ChannelConfTypeDef s_ADC_Config_U = {};
ADC_ChannelConfTypeDef s_ADC_Config_I = {};

float I_SCALE = 1;

void InitializeApp() {
    HAL_ADCEx_Calibration_Start(&hadc5, ADC_SINGLE_ENDED);

    static const float log_low  = CFXS::Math::Approx_Log10(20);
    static const float log_high = CFXS::Math::Approx_Log10(13000);
    // Generate evenly distributed frequency array from a logarithmic scale
    for (int i = 0; i < HZ_INDEX_COUNT; i++) {
        float f       = CFXS::Math::Approx_Pow10(CFXS::Math::Map(i, 0, HZ_INDEX_COUNT, log_low, log_high));
        s_HZ_Table[i] = f;
    }

#define Log10 CFXS::Math::Approx_Log10
#define Pow10 CFXS::Math::Approx_Pow10
    using CFXS::Math::Map;
    // Generated at compile time - generate list of X coordinates matching frequencies on a logarithmic scale
    __used static const float segments[] = {
        Map(Log10(Pow10(Log10(20))), log_low, log_high, 0, 320),    //
        Map(Log10(Pow10(Log10(30))), log_low, log_high, 0, 320),    //
        Map(Log10(Pow10(Log10(40))), log_low, log_high, 0, 320),    //
        Map(Log10(Pow10(Log10(50))), log_low, log_high, 0, 320),    //
        Map(Log10(Pow10(Log10(60))), log_low, log_high, 0, 320),    //
        Map(Log10(Pow10(Log10(70))), log_low, log_high, 0, 320),    //
        Map(Log10(Pow10(Log10(80))), log_low, log_high, 0, 320),    //
        Map(Log10(Pow10(Log10(90))), log_low, log_high, 0, 320),    //
        Map(Log10(Pow10(Log10(100))), log_low, log_high, 0, 320),   //
        Map(Log10(Pow10(Log10(200))), log_low, log_high, 0, 320),   //
        Map(Log10(Pow10(Log10(300))), log_low, log_high, 0, 320),   //
        Map(Log10(Pow10(Log10(400))), log_low, log_high, 0, 320),   //
        Map(Log10(Pow10(Log10(500))), log_low, log_high, 0, 320),   //
        Map(Log10(Pow10(Log10(600))), log_low, log_high, 0, 320),   //
        Map(Log10(Pow10(Log10(700))), log_low, log_high, 0, 320),   //
        Map(Log10(Pow10(Log10(800))), log_low, log_high, 0, 320),   //
        Map(Log10(Pow10(Log10(900))), log_low, log_high, 0, 320),   //
        Map(Log10(Pow10(Log10(1000))), log_low, log_high, 0, 320),  //
        Map(Log10(Pow10(Log10(2000))), log_low, log_high, 0, 320),  //
        Map(Log10(Pow10(Log10(3000))), log_low, log_high, 0, 320),  //
        Map(Log10(Pow10(Log10(4000))), log_low, log_high, 0, 320),  //
        Map(Log10(Pow10(Log10(5000))), log_low, log_high, 0, 320),  //
        Map(Log10(Pow10(Log10(6000))), log_low, log_high, 0, 320),  //
        Map(Log10(Pow10(Log10(7000))), log_low, log_high, 0, 320),  //
        Map(Log10(Pow10(Log10(8000))), log_low, log_high, 0, 320),  //
        Map(Log10(Pow10(Log10(9000))), log_low, log_high, 0, 320),  //
        Map(Log10(Pow10(Log10(10000))), log_low, log_high, 0, 320), //
    };

    auto drawTask = CFXS::Task::Create(
        LOW_PRIORITY,
        "Draw",
        [](auto...) {
            bool start_zero_cross_search_u = false;
            bool start_zero_cross_search_i = false;
            int zero_cross_u               = 0;
            int zero_cross_i               = 0;

            float amplitude_u = 0;
            float amplitude_i = 0;
            for (int i = 0; i < NUM_SAMPLES; i++) {
                amplitude_u = std::max<float>(std::abs(reading_voltage[i]), amplitude_u);
                amplitude_i = std::max<float>(std::abs(reading_current[i]), amplitude_i);
            }

            static bool first = true;
            if (first) {
                first = false;
                Display::Clear();
            }

            static uint8_t last_h_u[HZ_INDEX_COUNT];
            static uint8_t last_h_i[HZ_INDEX_COUNT];

            float max_scale = 0;
            float min_scale = 1000000000000;
            for (int i = 0; i < HZ_INDEX_COUNT; i++) {
                if (s_Impedance[i] > max_scale)
                    max_scale = s_Impedance[i];
                if (s_Impedance[i] <= min_scale && s_Impedance[i] > 0.1)
                    min_scale = s_Impedance[i];
            }

            float minr      = 1000;
            float maxr      = -1000;
            float max_point = 0;

            for (int i = 0; i < 160; i++) {
                if (s_Impedance[i]) {
                    if (s_Impedance[i] > 0) {
                        minr = std::min<float>(minr, s_Impedance[i]);
                    }

                    if (s_Impedance[i] > maxr) {
                        maxr      = s_Impedance[i];
                        max_point = s_HZ_Table[i];
                    }
                }
            }

            if (s_HZ_Index <= s_Stop) {
                for (int y = 0; y < 170; y++) {
                    for (auto s : segments) {
                        Display::SetPixel((int)s, y, Display::ToPixelColor(16, 16, 16));
                    }
                }
            }

            if (s_HZ_Index == s_Stop) {
                s_HZ_Index++;

                char str[32];
                snprintf(str, 32, "Min R: %.1f", minr);
                s_TextR1.DrawText(str);
                snprintf(str, 32, "Max R: %.1f @ %.1fHz", maxr, 170000000.0f / ((hz_to_timer_period(max_point) + 1) * wave_lut_size));
                s_TextR2.DrawText(str);
            }

            float current_hz = 170000000.0f / ((hz_to_timer_period(s_HZ_Table[s_HZ_Index]) + 1) * wave_lut_size);

            if (s_HZ_Index >= s_Stop) {
                char str[32];
                str[0] = 0;
                s_TextR3.DrawText(str);
            } else {
                char str[32];
                snprintf(str, 32, "F: %.1f", current_hz);
                s_TextR3.DrawText(str);
            }

            for (int i = 0; i < HZ_INDEX_COUNT; i++) {
                last_h_u[i] = (uint8_t)(Display::GetHeight() - CFXS::Math::ClampMap(s_Impedance[i], 0, 24, 0, Display::GetHeight()));
                Display::SetPixel(CFXS::Math::Map(i, 0, HZ_INDEX_COUNT, 0, 320), last_h_u[i], Display::ToPixelColor(0, 255, 0));

                if (s_Phase[i]) {
                    auto phase_loc = (uint8_t)(Display::GetHeight() -
                                               CFXS::Math::ClampMap(s_Phase[i], -CFXS::Math::PI, CFXS::Math::PI, 0, Display::GetHeight()));
                    Display::SetPixel(CFXS::Math::Map(i, 0, HZ_INDEX_COUNT, 0, 320), phase_loc, Display::ToPixelColor(255, 0, 0));
                }
            }
        },
        0);
    drawTask->Start();

    Buttons::Initialize();

    Display::Initialize();

    CalculateWaveLUT(0);

    Buttons::RegisterStateCallback([](void* data, Buttons::Key key, bool state) {
        // Ignore release events
        if (!state)
            return;

        // If OK is pressed
        if (key == Buttons::Key::OK) {
            // If LEFT and RIGHT is pressed at the same time
            if (Buttons::GetState(Buttons::Key::LEFT) && Buttons::GetState(Buttons::Key::RIGHT)) {
                // Release POWER_HOLD
                GPIOA->ODR &= ~GPIO_PIN_2;
            }

            // Start new measurement sweep
            RestartMeasurement();
        } else if (key == Buttons::Key::DOWN) {
            // Cancel measurement
            s_Stop = 0;
        } else if (key == Buttons::Key::UP) {
            // Testing
            adc_trig = false;
        }
    });

    Task::EnableProcessing();
}

///////////////////////////////////////////////////////////////////////////////////

void CFXS_SystemPriorityLoop() {
    CFXS::Time::ms++;
#if defined(CFXS_PLATFORM_STM32)
    HAL_IncTick();
#endif
    Task::ProcessGroup(SYSTEM_PRIORITY);

    if (CFXS::Time::ms == 250 && !Buttons::GetState(Buttons::Key::LEFT) && !Buttons::GetState(Buttons::Key::RIGHT)) {
        // Enable POWER_HOLD
        GPIOA->ODR |= GPIO_PIN_2;
    }
}

void CFXS_HighPriorityLoop() {
    Task::ProcessGroup(HIGH_PRIORITY);
}

void CFXS_LowPriorityLoop() {
    Task::ProcessGroup(LOW_PRIORITY);
}

///////////////////////////////////////////////////////////////////////////////////

const CFXS::STM32::CoreInitDescriptor s_PlatformInitDescriptor = {};

const CFXS::AppDescriptor e_AppDescriptor{
    .platformInitDescriptor     = &s_PlatformInitDescriptor,
    .moduleInit                 = InitializeModules,
    .postModuleInit             = InitializeApp,
    .highPriorityLoopPeriod     = CFXS::CPU::GetCyclesPerMillisecond(),
    .highPriorityLoopPriority   = 4,
    .systemPriorityLoopPeriod   = CFXS::CPU::GetCyclesPerMillisecond(),
    .systemPriorityLoopPriority = 1, // ignored
};
