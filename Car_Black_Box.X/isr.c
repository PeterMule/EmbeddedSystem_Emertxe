#include <xc.h>
#include "main.h"

void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {
        if (++count == 125) // 1sec
        {
            if(sec!=0)
            {
                sec--;
            }
        }
        
        TMR2IF = 0;
    }
}