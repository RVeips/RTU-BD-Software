#include <CFXS/Base/Debug.hpp>
#include <CFXS/Base/Memory.hpp>
#include <CFXS/Base/Utility.hpp>
#include <CFXS/Platform/CPU.hpp>
#include <CFXS/Platform/_CPU/ARMv7_M.hpp>
#include <core_cm4.h>

using CFXS::CPU::ARMv7_M::BusFaultStatusRegister;
using CFXS::CPU::ARMv7_M::ExceptionFrame;
using CFXS::CPU::ARMv7_M::ExceptionReturn;
using CFXS::CPU::ARMv7_M::ExtendedExceptionFrame;
using CFXS::CPU::ARMv7_M::MemManageFaultStatusRegister;
using CFXS::CPU::ARMv7_M::UsageFaultStatusRegister;

// TODO: check what faults are actually active
// https://developer.arm.com/documentation/dui0552/a/cortex-m3-peripherals/system-control-block/system-handler-control-and-state-register

volatile __noinit char g_BusFaultStatus[64];
volatile __noinit char g_MemFaultStatus[64];
volatile __noinit char g_FaultSources[64];
volatile __noinit uint32_t g_FaultReturn = 0;

extern "C" __used void __DecodeHardFault(uint32_t* stack_pointer, uint32_t* exc_return_val) {
    g_BusFaultStatus[0] = 0;
    g_FaultSources[0]   = 0;

    // auto& EXC_RETURN      = (const ExceptionReturn&)exc_return_val;
    auto& EXC_RETURN      = *(const ExceptionReturn*)exc_return_val;
    auto exception_number = SCB->ICSR & CFXS::CreateBitMask(__NVIC_PRIO_BITS);
    CFXS_printf("-------- HardFault D: [%lu] --------\n", exception_number);

    auto xPSR       = stack_pointer[7]; // &xPSR == SP + 7
    bool alignFrame = (xPSR & (1 << 9));
    CFXS_printf("xPSR [0x%08lX]\n", stack_pointer[7]);

    CFXS_printf("[EXC_RETURN] - 0x%08lX\n", (uint32_t)exc_return_val);
    CFXS_printf(" - SPSEL:      %s\n", EXC_RETURN.SPSEL ? "Process [PSP]" : "Main [MSP]");
    CFXS_printf(" - MODE:       %s\n", EXC_RETURN.MODE ? "Thread" : "Handler");
    CFXS_printf(" - FTYPE:      %s\n", EXC_RETURN.FTYPE ? "Normal" : "Extended");

    uint32_t fullFrameOffset                     = ((EXC_RETURN.FTYPE ? 0x20 : 0x68) + alignFrame ? 4 : 0) - 4;
    [[maybe_unused]] auto exceptionFrame         = (const ExceptionFrame&)stack_pointer[7];
    [[maybe_unused]] auto extendedExceptionFrame = (const ExtendedExceptionFrame&)stack_pointer[fullFrameOffset];

    auto& HFSR = SCB->HFSR;                                             // HardFault
    auto& MMSR = __memT(MemManageFaultStatusRegister, SCB_BASE + 0x28); // MemManage Fault
    auto& BFSR = __memT(BusFaultStatusRegister, SCB_BASE + 0x29);       // BusFault
    auto& UFSR = __memT(UsageFaultStatusRegister, SCB_BASE + 0x2A);     // UsageFault

    // CFXS_printf("HardFault | EXEC_RETURN: 0x%08lX | FRAME: 0x%08lX\n", (uint32_t)exec_return, (uint32_t)exception_frame);
    CFXS_printf("[Stack Frame]:\n");
    CFXS_printf(" - R0:     0x%08lX\n", extendedExceptionFrame.R0);
    CFXS_printf(" - R1:     0x%08lX\n", extendedExceptionFrame.R1);
    CFXS_printf(" - R2:     0x%08lX\n", extendedExceptionFrame.R2);
    CFXS_printf(" - R3:     0x%08lX\n", extendedExceptionFrame.R3);
    CFXS_printf(" - R12:    0x%08lX\n", extendedExceptionFrame.R12);
    CFXS_printf(" - LR:     0x%08lX\n", extendedExceptionFrame.LR);
    CFXS_printf(" - Return: 0x%08lX\n", extendedExceptionFrame.ReturnAddress);
    CFXS_printf(" - xPSR:   0x%08lX\n", extendedExceptionFrame.xPSR);

    g_FaultReturn = extendedExceptionFrame.ReturnAddress;

    CFXS_printf("HardFault Status:  [0x%08lX]\n", HFSR);
    CFXS_printf(" - DEBUGEVT: %u\n", !!(HFSR & (1 << 31)));
    CFXS_printf(" - FORCED:   %u\n", !!(HFSR & (1 << 30)));
    CFXS_printf(" - VECTTBL:  %u\n", !!(HFSR & (1 << 1)));

    if (__mem8(SCB_BASE + 0x29)) {
        strcat((char*)g_FaultSources, "Bus; ");
        CFXS_printf("BusFault Status:   [0x%02X]\n", __mem8(SCB_BASE + 0x29));
        CFXS_printf(" - IBUSERR:     %u\n", BFSR.IBUSERR);
        CFXS_printf(" - PRECISERR:   %u\n", BFSR.PRECISERR);
        CFXS_printf(" - IMPRECISERR: %u\n", BFSR.IMPRECISERR);
        CFXS_printf(" - UNSTKERR:    %u\n", BFSR.UNSTKERR);
        CFXS_printf(" - STKERR:      %u\n", BFSR.STKERR);
        CFXS_printf(" - LSPERR:      %u\n", BFSR.LSPERR);
        CFXS_printf(" - BFARVALID:   %u\n", BFSR.BFARVALID);
        CFXS_printf(" - BFAR:        0x%08lX\n", __mem32(0xE000ED38));
        snprintf((char*)g_BusFaultStatus,
                 sizeof(g_BusFaultStatus),
                 "PREC: %u | IMPR: %u | BFAR: 0x%lX (%s)",
                 BFSR.PRECISERR,
                 BFSR.IMPRECISERR,
                 __mem32(0xE000ED38),
                 BFSR.BFARVALID ? "valid" : "invalid");
    }

    if (__mem8(SCB_BASE + 0x28)) {
        strcat((char*)g_FaultSources, "MM; ");
        CFXS_printf("MemManage Status:  [0x%02X]\n", __mem8(SCB_BASE + 0x28));
        CFXS_printf(" - IACCVIOL:    %u\n", MMSR.IACCVIOL);
        CFXS_printf(" - DACCVIOL:    %u\n", MMSR.DACCVIOL);
        CFXS_printf(" - MUNSTKERR:   %u\n", MMSR.MUNSTKERR);
        CFXS_printf(" - MSTKERR:     %u\n", MMSR.MSTKERR);
        CFXS_printf(" - MLSPERR:     %u\n", MMSR.MLSPERR);
        CFXS_printf(" - MMARVALID:   %u\n", MMSR.MMARVALID);
        CFXS_printf(" - MMAR:        0x%08lX\n", __mem32(0xE000ED34));
        snprintf((char*)g_MemFaultStatus,
                 sizeof(g_MemFaultStatus),
                 "MMAR: 0x%lX (%s)",
                 __mem32(0xE000ED34),
                 MMSR.MMARVALID ? "valid" : "invalid");
    }

    if (__mem16(SCB_BASE + 0x2A)) {
        strcat((char*)g_FaultSources, "Usage; ");
        CFXS_printf("UsageFault Status: [0x%04X]\n", __mem16(SCB_BASE + 0x2A));
        CFXS_printf(" - DIVBYZERO:  %u\n", UFSR.DIVBYZERO);
        CFXS_printf(" - UNALIGNED:  %u\n", UFSR.UNALIGNED);
        CFXS_printf(" - NOCP:       %u\n", UFSR.NOCP);
        CFXS_printf(" - INVPC:      %u\n", UFSR.INVPC);
        CFXS_printf(" - INVSTATE:   %u\n", UFSR.INVSTATE);
        CFXS_printf(" - UNDEFINSTR: %u\n", UFSR.UNDEFINSTR);
    }

    CFXS_BREAK();
    while (1 < 2) {
    }
    CFXS::CPU::Reset();
}

__naked void __cfxs_HardFault() {
    register uint32_t* stack_pointer;
    register uint32_t* exc_return;
    asm volatile("mov %[exc_return], lr\n"
                 "tst lr, #4\n"
                 "ite eq\n"
                 "mrseq %[stack_pointer], msp\n"
                 "mrsne %[stack_pointer], psp\n"
                 : [ exc_return ] "=r"(exc_return), [ stack_pointer ] "=r"(stack_pointer)
                 :);
    __DecodeHardFault(stack_pointer, exc_return);
}
