/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* Global Varibles */
volatile int Pulse = 0;
volatile uint32_t t = 0; // to store TA0R value
volatile uint32_t Timer_ms = 3; //to store calculator

Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        32000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        3200
};


void PORT6_IRQHandler(void)
{
    //ultrasonic sensor handler
    if(P6IFG & BIT4)
    {

        TA0CTL |= MC__CONTINUOUS;       //start timer and set mode;
        while(P6IN == BIT4);
        TA0CTL |= MC__STOP;         //halts timer
        t = TA0R;
        Timer_ms = (((8)*(t))/(4096)) * 1000; //convert to ms
        TA0CTL |= TACLR;            //Reset Timer
        TA0CTL |= MC__STOP;         //halts timer
    }
    P6IFG &= ~BIT4;
}


void TA0_0_IRQHandler(void)
{
    if(TA0CCTL0 & CCIFG)
    {
        P2OUT ^= BIT2;
    }
    TA0CCTL0 &= ~CCIFG;         //Clears the interrupt
}



int main(void)
{
    WDT_A_holdTimer();          //Stop Watchdog timer


    /* Vibration Motor */
    P2->DIR |= BIT3;            //Sets PIN 2.3 as an OUTPUT
    P2OUT &= ~BIT3;             //Starts PIN 2.3 to be OFF

    /* BLUE LED */
    P2->DIR |= BIT2;            //Sets PIN 2.3 as an OUTPUT
    P2OUT &= ~BIT2;             //Starts PIN 2.3 to be OFF

    /* Clock signal for the Timer */
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_8);

    /* Timer Initialization */
    TA0CTL |= TASSEL__ACLK;         //Timer clock source
    TA0CTL |= ID__8;                //Timer divider
    CCIS_0

    TA0CCTL0 |= CM__BOTH;               //capture on both and store in a register

    TA0CCTL0 |= CCIE;               //Enable Interrupts
    TA0CCTL0 &= ~CCIFG;             //Clears the interrupt
    NVIC_EnableIRQ(TA0_0_IRQn);     //Calls the NVIC function and enables the interrupt


    /* Ultrasonic Sensor */
    P6->DIR &= ~BIT4;           //Set PIN 6.4 as an INPUT
    P6->OUT |= BIT4;            //and to also be a pull-up resistor
    P6->REN |= BIT4;
    P6IFG &= ~BIT4;             //Clears the interrupt
    P6IE |= BIT4;               //Sets the pin to be an interrupt
    P6IES &= ~BIT4;             //Sets the interrupt to be  low-to-high transition
    NVIC_EnableIRQ(PORT6_IRQn); //Calls the NVIC function and enables the port/pin interrupt


    __enable_interrupt();       //Enables global interrupts


    while(1)
    {

    }
}


