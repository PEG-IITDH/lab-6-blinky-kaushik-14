#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define MASK_BITS   0x11        //  mask bits for user switch 1 and 2

int main(void)
{
    // GPIO PORT F config
    SYSCTL_RCGC2_R |= 0x00000020;       /* enable clock to GPIOF */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTF_CR_R = 0x01;             /* make PORTF0 configurable */
    GPIO_PORTF_DEN_R = 0x1F;            /* set PORTF pins 4 pin */
    GPIO_PORTF_DIR_R = 0x0E;            /* set PORTF4 pin as input user switch pin */
    GPIO_PORTF_PUR_R = 0x11;            /* PORTF4 is pulled up */

    //GPIO PORT F interrupt config
    GPIO_PORTF_IM_R &= ~MASK_BITS;      // mask interrupts for sw1 and sw2
    GPIO_PORTF_IS_R &= ~MASK_BITS;      // edge sensitive interrupts
    GPIO_PORTF_IEV_R  &= ~MASK_BITS;    // falling edge triggers the interrupt
    GPIO_PORTF_IBE_R &= ~MASK_BITS;     // Interrupt is controlled by GPIOIEV
    NVIC_EN0_R |= (1 << 30);            // enable interrupt on port F
    GPIO_PORTF_ICR_R = MASK_BITS;       // clears RIS and MIS for edge sensitive interrupt
    GPIO_PORTF_IM_R |= MASK_BITS;       // unmask interrupts for sw1 and sw2

    //UART7 config
    SYSCTL_RCGCUART_R |= 0x80;          //UART module 7  is enabled
    SYSCTL_RCGC2_R |= 0x10;             //Port E receives clock
    GPIO_PORTE_DEN_R |= 0x03;
    GPIO_PORTE_DIR_R |= 0x02;

    GPIO_PORTE_AFSEL_R |= 0x03;         //Port E0 and E1 alternate function
    GPIO_PORTE_PCTL_R |= 0x000011;          //UartRx(Tx) selected on PE0(1)
    UART7_CTL_R &= 0xFFFFFFFE;          //Disable UART
    UART7_IBRD_R = 104;
    UART7_FBRD_R = 11;
    UART7_LCRH_R |= 0x62;               // 8 bit, odd parity enable
    UART7_CC_R &= 0xFFFFFFF0;            //System clock is source for baud
    UART7_CTL_R |= ((1<<0)|(1<<8)|(1<<9));                 //UART enable,transmit and receive enable

    //UART7 receive interrupt enable
    UART7_IM_R &= 0x00;                 //mask UART interrupts
    UART7_ICR_R &= 0x00;                //clear all the UART interrupts
    UART7_IM_R |=(1<<4);                //unmask UART receive interrupt
    NVIC_EN1_R |= (1<<31);              //enable UART7 interrupt in NVIC

    while(1)
    {

    }
}

void GPIOF_INT_Handler(void)
{
    GPIO_PORTF_IM_R &= ~MASK_BITS;                  // mask interrupts for sw2
    int i;

     if(GPIO_PORTF_RIS_R & 0x01)                     // interrupt on user switch 2
     {
         for(i=0;i<16000;i++){}                      //debounce delay
         if(~(GPIO_PORTF_DATA_R)&0x01)               // i/p on switch 2 is 0
         {
             UART7_DR_R = 0xAA;                     // transmit hexadecimal AA on UART
         }
     }

     if(GPIO_PORTF_RIS_R & 0x10)                     // interrupt on user switch 1
     {
         for(i=0;i<16000;i++){}                     //delay for debounce
         if(~(GPIO_PORTF_DATA_R)&0x10)              // i/p on switch 1 is 0
         {
             UART7_DR_R = 0xF0;                     //transmit hexadecimal FO on UART
         }
     }
     GPIO_PORTF_ICR_R = MASK_BITS;                  //clear the interrupts
     GPIO_PORTF_IM_R |= MASK_BITS;                  //unmask interrupts
}

void UART7_Handler(void)
{
    UART7_IM_R &= 0x00;                             //mask receive interrupt

    if(UART7_FR_R & (1<<6))
    {
        if(UART7_DR_R == 0xF0)                      //check if received data is FO
        {
            GPIO_PORTF_DATA_R |= 0x04;              //turn blue LED ON
        }
        else if(UART7_DR_R == 0xAA)                 //check if received data is AA
        {
            GPIO_PORTF_DATA_R |= 0x08;              //turn green LED ON
        }
        else if(UART7_RSR_R & 0xF)                  //check for errors
        {
            GPIO_PORTF_DATA_R |= 0x02;              //turn red LED ON
        }
    }
    int i;
    for(i=0;i<800000;i++){}                         //delay
    GPIO_PORTF_DATA_R &= 0x00;                      //turn the LEDs off
    UART7_ECR_R &= ~(0xF);                          //clear the error bits
    UART7_ICR_R &= 0x00;                            //clear UARt interrupt flags
    UART7_IM_R |= (1<<4);                           //unmask UART receive interrupt
}
