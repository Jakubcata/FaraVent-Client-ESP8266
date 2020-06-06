#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#define MAX_NUM_OF_INTERRUPTS       10

class Interrupt
{
    static Interrupt* iISRVectorTable[MAX_NUM_OF_INTERRUPTS];

    public:
        Interrupt()
        {}

        static void Register( const uint8_t aInterruptNumber, Interrupt* aIntThisPtr );

        static void Interrupt_0();
        
        virtual void ISR() = 0; 
};

#endif /* __INTERRUPT_H__ */