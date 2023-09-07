#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

int main(void)
{
    SYSCTL_RCGC2_R |= 0x00000020;       /* enable clock to GPIOF */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTF_CR_R = 0x1F;             /* make PORTF0 configurable */
    GPIO_PORTF_DEN_R = 0x0E;            /* set PORTF pins 4-3-2-1-0 as digital pins */
    GPIO_PORTF_DIR_R = 0x0E;            /* set PORTF3+PORTF2+PORTF1 pin as output (LED) pin */

    int i=0;

    while(1){
     GPIO_PORTF_DATA_R = 0X04;          /* White */

     for(i=0; i<800; i++)               /*delay of 0.5ms*/
     {}

     GPIO_PORTF_DATA_R = 0X00;          /* Dark */

     for(i=0; i<800; i++)               /*delay of 0.5ms*/
     {}
    }
return 0;
}
