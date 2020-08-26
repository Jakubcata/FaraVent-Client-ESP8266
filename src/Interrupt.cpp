#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include "Interrupt.h"
#include "MotionSensor.h"

Interrupt* Interrupt::iISRVectorTable[MAX_NUM_OF_INTERRUPTS];

void Interrupt::Interrupt_0()
{
    iISRVectorTable[0]->ISR();
}

void Interrupt::Register( const uint8_t aInterruptNumber, Interrupt* aIntThisPtr )
{
    iISRVectorTable[aInterruptNumber] = aIntThisPtr;
}