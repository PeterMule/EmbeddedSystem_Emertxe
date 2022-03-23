#include "car_black_box_de.h"

//Holds the time in formart
//0  1  2
//HH MM SS
unsigned char clock_reg[3];

//Where the time in ASCII is stored
char time[6];

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

void get_speed();
void display_dashboard(char* event,unsigned char speed)
{
    //Print header line
    clcd_print("  TIME     E  SP", LINE1(0));
    
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