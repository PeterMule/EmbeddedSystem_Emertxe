#include "car_black_box_de.h"

//Holds the time in formart
//0  1  2
//HH MM SS
unsigned char clock_reg[3];

//Where the time in ASCII is stored
char time[6];

//Current Index of the Logged events
char log_index = -1;

//Number of events Logged
char no_events = 0;

//Log Event
unsigned char log[11];

void get_time()
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
    
    /* To store the time in HH:MM:SS format */
    // Hour -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    
    // Minute
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    
    // Second
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
}

void display_time()
{
    get_time();
    
    
    clcd_putch(time[0],LINE2(2));
    clcd_putch(time[1],LINE2(3));
    clcd_putch(':',LINE2(4));
    
    clcd_putch(time[2],LINE2(5));
    clcd_putch(time[3],LINE2(6));
    clcd_putch(':',LINE2(7));
    
    clcd_putch(time[4],LINE2(8));
    clcd_putch(time[5],LINE2(9));
}

unsigned char get_speed(void)
{
    return ((float)read_adc()/1023 * 99);
}

void display_dashboard(char* event,unsigned char speed)
{
    //Print header line
    clcd_print("  TIME     E  SP", LINE1(0));
    
    clcd_print("  ",LINE2(0));
    
    //Display time in format HH:MM:SS
    display_time();
    
    
    //Display Event
    clcd_putch(event[0],LINE2(11));
    clcd_putch(event[1],LINE2(12));
    clcd_putch(event[2],LINE2(13));
    
    //Display Speed
    clcd_putch((speed/10)+'0',LINE2(14));
    clcd_putch((speed%10)+'0',LINE2(15));
}

void log_event(void)
{
    log_index++;
    if(log_index > 9)
    {
        log_index = 0;
    }
        
    unsigned char addr = log_index*10 + 0x05;
    
    for(int i = 0; log[i] != '\0'; i++)
    {
        eeprom_write(addr+i,log[i]);
    }
    if(no_events < 10)
        no_events++;
}
unsigned char* read_log_car_event(char index)
{
    
    if(index<no_events)
    {
        //                       "9 20:17:99 ON 44"
        unsigned char edited[16];
        unsigned char addr = index*10 + 0x05;
        for(int i = 0; i < 10 ; i++)
        {
            log[i] = eeprom_read(addr+i);
        }
        edited[0] = index+'0';
        edited[1] = ' ';
        edited[2] = log[0];
        edited[3] = log[1];
        edited[4] = ':';
        edited[5] = log[2];
        edited[6] = log[3];
        edited[7] = ':';
        edited[8] = log[4];
        edited[9] = log[5];
        edited[10] = ' ';
        edited[11] = log[6];
        edited[12] = log[7];
        edited[13] = ' ';
        edited[14] = log[8];
        edited[15] = log[9];
        return edited;
    }
    return -1;
    
}
void log_car_event(char event[], unsigned char speed)
{
    get_time();
    //HHMMSSEESP
    //0123456789
    strncpy(log,time,6);
    
    strncpy(&log[6],event,2);
    
    //Write Speed
    log[8] = (speed/10)+'0';
    log[9] = (speed%10)+'0';
    
    log[10] = '\0';
    
    log_event();
}
char display_log_car_event(char index)
{
    if(index == -1)
    {
        //Print header line
        clcd_print("#  TIME     E  SP", LINE1(0));


        clcd_print("No Logs Available", LINE2(0));
        return 1;
    }
    if(index<no_events)
    {
        //Print header line
        clcd_print("#  TIME     E  SP", LINE1(0));


        clcd_print(read_log_car_event(index), LINE2(0));
        return 1;
    }
    return -1;

}
void clcd_clear_screen(void)
{
     clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(100);
}

void login(unsigned char key,unsigned char reset_flag)
{
    //Password entered
    static char npassword[4];
    //Number of attempts remaining
    static unsigned char attempt_rem, i;
    unsigned char same_password;
    
    if(reset_flag == RESET_PASSWORD)
    {
        attempt_rem = '3';
        
        i = 0;
        
        npassword[0] = '\0';
        npassword[1] = '\0';
        npassword[2] = '\0';
        npassword[3] = '\0';
        key = ALL_RELEASED;
    }
    else{
        if(i < 4)
        {
            if(key == SW4)
            {
                npassword[i] = PASS_SW4;
                clcd_putch('*', LINE2(6+i));
                i++;
            }
            else if(key == SW5)
            {
                npassword[i] = PASS_SW5;
                clcd_putch('*', LINE2(6+i));
                i++;
            }
        }
        if( i == 4 )
        {
            /*
             * for(int j = 0; (j < 4) ;j++)
             *  {
             *       spassword[i] = eeprom_read(j);
             *  }
             * if(strncmp(npassword, spassword, 4) == 0)
             * {
             * 
             * }`
             * 
             */
            same_password = 1;
            for(char j = 0; (j < 4) && (same_password == 1);j++)
            {
                same_password = eeprom_read(j) == npassword[j]?1:0;
            }
            if(same_password == 1)//Menu Screen
            {

            }
            else//Wrong password
            {
                attempt_rem--;
                if(attempt_rem == '0')//Block user for 15 minutes
                {
                    clcd_clear_screen();
                    clcd_print(" You are blocked ",LINE1(0));
                    clcd_print(" Wait for 60 seconds... ",LINE2(0));
                    sec = 60;
                    /* Switching on the Timer2 Interrupt */
                    TMR2IE = 1;
                    TMR2ON = 1;
                    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                    while(sec!=0)
                    {
                        clcd_putch((sec/10)+'0',LINE2(10));
                        clcd_putch((sec%10)+'0',LINE2(11));
                    }
                    clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
                    /* Switching off the Timer2 Interrupt */
                    TMR2IE = 0;
                    TMR2ON = 0;
                    attempt_rem = '3';
                }
                else
                {
                    clcd_clear_screen();
                    clcd_print(" WRONG PASSWORD ",LINE1(0));
                    clcd_putch(attempt_rem,LINE2(0));
                    clcd_print(" attempts remaining", LINE2(1));
                    __delay_ms(3000);           
                }
                clcd_clear_screen();
                clcd_print(" ENTER PASSWORD", LINE1(0));
                clcd_print(" ", LINE2(CURSOR_POS));
                i=0;
            }
        }
    }
    

}