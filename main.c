/*
 * Name: N.Bhargavi
 * Date: 14-12-24
 * Project Name: Car Block Box
 */

#include"main.h"
#include"clcd.h"
#include"uart.h"
#include"matrix_keypad.h"
#include"adc.h"
#include"block_box.h"
#include"i2c.h"
#include"ds1307.h"
/*event array */
char event[9][3]={"ON","GN","G1","G2","G3","G4","G5","GR","C_"};
extern int i;
char time[9];   /* time array for storing time*/
unsigned char clock_reg[3];
extern int opt;

void init_config(void)      /*initially configuring all pheripherals*/
{
    init_clcd();
    init_matrix_keypad();
    init_adc();
    init_uart();
    init_i2c();
    init_ds1307();
}
int key;
short adc_reg_val;
extern unsigned char add;
short menu=0;
extern int back_menu;

void get_time(void)   /*getting time from the RTC*/
{
	clock_reg[0] = read_ds1307(HOUR_ADDR);/*getting hour from RTC*/
	clock_reg[1] = read_ds1307(MIN_ADDR); /*getting minutes from RTC*/
	clock_reg[2] = read_ds1307(SEC_ADDR);/*getting seconds from RTC*/

	if (clock_reg[0] & 0x40)    /*storing into time array*/
	{
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	else
	{
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	time[2] = ':';
	time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
	time[4] = '0' + (clock_reg[1] & 0x0F);
	time[5] = ':';
	time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
	time[7] = '0' + (clock_reg[2] & 0x0F);
	time[8] = '\0';
}

void main(void)
{
    init_config();
    print_init(); 
    //puts("Welcome. This is UART Test Code.\n\r");
	short count=1;
    while(1)
    {
        
        adc_reg_val = (read_adc(CHANNEL4))/10.23;       //reading potentiometer value
        key=read_switches(STATE_CHANGE);
        get_time();
        if(key==MK_SW11)
        {
            if(count)           //first time entering into menu
            {
                //CLEAR_DISP_SCREEN;
                count=0;
               // clcd_print("ddddddddddddd", LINE1(0));
                opt=1;
                back_menu=1;
            }
            menu=1;       
            go_menu(); 
        }
        else if(menu)       //going to menu screen until exit
        {
              go_menu(); 
        }
        else
        {
            //get_time();
            clcd_print(event[i], LINE2(10));
            clcd_print(time, LINE2(0));
            if(key==MK_SW2)         //increasing the gear
            { 
                inc_gear();
                call_write();
            }
            else if(key==MK_SW3)    //decreasing the gear
            {
                dec_gear();
                call_write();
            }
            else if(key==MK_SW1)    //collision mode
            {
                collision();
                call_write();
            }
            count=1;
            change_speed(adc_reg_val); //reading speed and displaying in function
                        
        }
    }
}
