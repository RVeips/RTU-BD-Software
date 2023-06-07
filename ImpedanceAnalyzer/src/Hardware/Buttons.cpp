#include "Buttons.hpp"
#include <CFXS/Platform/Task.hpp>
#include <CFXS/Base/Debug.hpp>
#include "stm32g4xx_hal.h"
#include <EASTL/vector.h>

extern ADC_HandleTypeDef hadc1;

static bool s_ButtonState[Buttons::Key::_COUNT]             = {};
static bool s_LastButtonState[Buttons::Key::_COUNT]         = {};
static CFXS::Time_t s_LastStateChange[Buttons::Key::_COUNT] = {};

ADC_ChannelConfTypeDef s_OK_ADC_Config = {};

static eastl::vector<Buttons::StateCallbackFunction> s_StateCallbacks;
void Buttons::Initialize() {
    s_OK_ADC_Config.Channel      = ADC_CHANNEL_2;
    s_OK_ADC_Config.Rank         = ADC_REGULAR_RANK_1;
    s_OK_ADC_Config.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
    s_OK_ADC_Config.SingleDiff   = ADC_SINGLE_ENDED;
    s_OK_ADC_Config.OffsetNumber = ADC_OFFSET_NONE;
    s_OK_ADC_Config.Offset       = 0;

    s_LastButtonState[Key::OK] = true; // Initial state while powering up

    CFXS::Task::Create(LOW_PRIORITY, "Button Process", Process, 0)->Start();
}

bool Buttons::GetState(Key key) {
    return s_ButtonState[key];
}

void Buttons::RegisterStateCallback(const StateCallbackFunction& cb) {
    s_StateCallbacks.push_back(cb);
}

void Buttons::Process(void* task_data) {
    static bool temp_state[Key::_COUNT];
    temp_state[Key::UP]    = !(GPIOA->IDR & GPIO_PIN_6); // PA6
    temp_state[Key::DOWN]  = !(GPIOA->IDR & GPIO_PIN_4); // PA4
    temp_state[Key::LEFT]  = !(GPIOA->IDR & GPIO_PIN_3); // PA3
    temp_state[Key::RIGHT] = !(GPIOB->IDR & GPIO_PIN_1); // PB1

    HAL_ADC_ConfigChannel(&hadc1, &s_OK_ADC_Config);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1);
    auto adc_ok_reading = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    temp_state[Key::OK] = adc_ok_reading > 100;

    for (int i = 0; i < Key::_COUNT; i++) {
        if (s_LastButtonState[i] != temp_state[i]) {
            s_LastButtonState[i] = temp_state[i];
            s_LastStateChange[i] = CFXS::Time::ms;
        }

        if (s_LastStateChange[i] && (CFXS::Time::ms - s_LastStateChange[i]) > DEBOUNCE_TIME) {
            if (s_ButtonState[i] != s_LastButtonState[i]) {
                s_ButtonState[i] = s_LastButtonState[i];
                for (auto cb : s_StateCallbacks) {
                    cb(static_cast<Key>(i), s_ButtonState[i]);
                }
            }
        }
    }
}
