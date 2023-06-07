
#include <stdarg.h>
#include <stdio.h>
#include "SEGGER_RTT.h"
#include <CFXS/Platform/CPU.hpp>

int __cfxs_putchar(int c) {
    if (!CFXS::CPU::IsDebugEnabled())
        return 0;
    char buf[1];
    buf[0] = (char)c;
    SEGGER_RTT_Write(0, buf, 1);
    return c;
}