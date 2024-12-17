#include"main.h"
#include"clcd.h"
#include"matrix_keypad.h"
#include"adc.h"
#include"block_box.h"
#include"uart.h"
#include"ds1307.h"
#include"external_EEPROM.h"

extern char event[][3];
unsigned char store[12] = {};
int i = 0;
static char option[4][16] = {"view log      ", "clear log       ", "download log  ", "set time      "};
extern int key;
unsigned char add = 0;
short event_count = 0;
extern char time[9];
extern short adc_reg_val;
char logs[10][15];
int opt = 1; //for goining inside the options
//for scrolling event list menu list
int index = 0;
int flag = 0;
//for scrolling event list view list
int view = 0;
int key_press = 0;
int back_menu = 0;

//FOR SET TIME
unsigned int hour, sec, min;
int hms = 0, blink = 0;
int settime, set;

int exit_key=1;

int edit_time = 0;

extern int menu;

void print_init(void) {
    get_time();
    clcd_print("TIME", LINE1(0));       /*Initially printing the dash board*/
    clcd_print("EV", LINE1(10));
    clcd_print("SP", LINE1(14));
    clcd_print(time, LINE2(0));
    clcd_print(event[i], LINE2(10));
    clcd_print("00", LINE2(14));
    adc_reg_val=00;
    call_write();
}

void go_menu(void) {
    if (opt) //for displaying the menu
    {
        if (back_menu) {      //while exiting from menu option displaying menu option
            CLEAR_DISP_SCREEN;
            key = MK_SW11;
            back_menu = 0;
            index = 0;
            flag = 0;
        }
        if (key == MK_SW11 && index < 3) {
            if (!flag) //for  menu  scrolling down
            {
                clcd_putch(0x7E, LINE1(0));
                clcd_putch(' ', LINE2(0));
                clcd_print(option[index], LINE1(1));
                clcd_print(option[index + 1], LINE2(1));
                flag++;
            } else {
                clcd_putch(' ', LINE1(0));
                clcd_putch(0x7E, LINE2(0));
                clcd_print(option[index], LINE1(1));
                clcd_print(option[index + 1], LINE2(1));
                index++;
            }
        } 
        else if (key == MK_SW12 && index) { 
            //for  menu  scrolling up
            if (flag) {
                clcd_putch(0x7E, LINE1(0));     //indicating the arrow(ox7E)
                clcd_putch(' ', LINE2(0));
                clcd_print(option[index - 1], LINE1(1));
                clcd_print(option[index], LINE2(1));
                flag = 0;
            } else {
                clcd_putch(0x7E, LINE1(0));
                clcd_putch(' ', LINE2(0));
                clcd_print(option[index - 1], LINE1(1));
                clcd_print(option[index], LINE2(1));
                index--;
            }
        }
        if (key == MK_SW1) //enter into menu option
            opt = 0;
        if (key == MK_SW2) //exit from menu
        {
            opt = 0;
            view = 0;
        }
    } 
    else if (key == MK_SW1 && exit_key) //entering into the options
    {
        if (key == MK_SW1 && index == 0) //for view log
        {
            load_data();
            key_press = 0;
            if (event_count == 0)       //if no logs present 
            {
                clcd_print("No Data Present ", LINE1(0));
                clcd_print("                ", LINE2(0));
                for (unsigned long int i = 5000000; i--;);
                opt = 1;
                back_menu = 1;
                view = 0; //
            } 
            else            // entering into the view log if data present
            {
                clcd_print("Id  TIME  EV  SP ", LINE1(0));
                view = 1; //for scrolling  view log
                char ch = key_press + '0';
                clcd_putch(ch, LINE2(0));
                clcd_putch(' ', LINE2(1));
                clcd_print(logs[key_press++], LINE2(2));
            }


            //view_log();
        }
        else if (key == MK_SW1 && index == 1) //clearing data
        {
            if (event_count == 0)   //if no logs present 
            {  
                clcd_print("No Data Present ", LINE1(0));
                clcd_print("                ", LINE2(0));
                 for (unsigned long int i = 5000000; i--;);
                opt = 1;
                back_menu = 1;
            }
            else        // entering into the clear log if data present and clearing the data
            {
                clcd_print("Clearing data...", LINE1(0));
                clcd_print("                ", LINE2(0));
                add = 0;
                event_count = 0;
                for (unsigned long int i = 500000; i--;);
                clcd_print("Data Cleared !  ", LINE2(0));
                for (unsigned long int i = 500000; i--;);
                opt = 1;
                back_menu = 1;
                
            }
            
        } 
        else if (key == MK_SW1 && index == 2) //download log
        {
            if (event_count == 0)   //if no logs present 
            {   
                clcd_print("No Data Present ", LINE1(0));
                clcd_print("                ", LINE2(0));
                puts("No Data present .....\n\r");
                 for (unsigned long int i = 5000000; i--;);
                opt = 1;
                back_menu = 1;
            }
            else           // entering into the download log if data present and displaying on teraterm
             {
                 load_data();
                //displaying on clcd 
                clcd_print("Downloading data", LINE1(0));
                clcd_print("                ", LINE2(0));
                //displaying on display
                puts("Downloading data.....\n\r");
                puts("Id Time Ev  Sp\n\r");
                display_data();
                for (unsigned long int i = 500000; i--;);
                //displaying on clcd
                clcd_print("Data Downloaded!", LINE2(0));
                for (unsigned long int i = 500000; i--;);
                opt = 1;
                back_menu = 1;
             }
            
        }
        else if (key == MK_SW1 && index == 3) //set time log
        {
            clcd_print("                ", LINE1(0));
            clcd_print("                ", LINE2(0));
            // clcd_print("    HH:MM:SS    ", LINE1(0));
            clcd_print(time, LINE2(2));


            //set_time();
            settime = 1;
            edit_time = 1;
            //opt=1;
            //back_menu=1;

        }
    }
    else if (view) //view option displaying 
    {
        if (key == MK_SW11 && key_press < event_count) {
            if (key_press <= -1)    //for out of bound condtion
                key_press = 0;

            char ch = key_press + '0';
            clcd_putch(ch, LINE2(0));
            clcd_putch(' ', LINE2(1));
            clcd_print(logs[key_press++], LINE2(2));
        } 
        else if (key == MK_SW12 && key_press>-1) {
            if (key_press == event_count)    //for out of bound condtion
                key_press = event_count - 1;
            if (key_press == -1) //for out of bound condtion
                key_press = 0;

            char ch = key_press + '0';
            clcd_putch(ch, LINE2(0));
            clcd_putch(' ', LINE2(1));
            clcd_print(logs[key_press--], LINE2(2));
        } 
        else if (key == MK_SW2) //exit from view log 
        {
            view = 0;
            back_menu = 1; //for goining back to menu 
            opt = 1;
        }

    } 
    else if (edit_time) //to edit the time 
    {
        exit_key=0; 
        if (settime == 1)       
        {
            settime = 0;
            hour = (time[0] - 48)*10;
            hour = hour + (time[1] - 48);
            
            min = (time[3] - 48)*10;
            min = min + (time[4] - 48);

            sec = (time[6] - 48)*10;
            sec = sec + (time[7] - 48);

            clcd_print("HH:MM:SS", LINE1(0));
        }

        // key=read_switches(STATE_CHANGE);

        blink = !blink;

        if (key == MK_SW12)     //for changing the fields
        {
            if (hms >= 0 && hms < 2)
                hms++;
            else if (hms >= 2)
                hms = 0;
        }
        else if (key == MK_SW11)    //for incrementing the time
        {
            if (hms == 0)   //increasing hours
            {
                if (hour >= 0 && hour < 23) {
                    hour++;
                } else
                    hour = 0;
            } 
            else if (hms == 1)   //increasing minutes
            {
                if (min >= 0 && min < 59) {
                    min++;
                } else
                    min = 0;
            }
            else if (hms == 2)       //increasing seconds
            {
                if (sec >= 0 && sec < 59) {
                    sec++;
                } else
                    sec = 0;
            }
        }
         /*printing on clcd while incrementing time*/
        clcd_putch('0' + (hour / 10), LINE2(0));
        clcd_putch('0' + (hour % 10), LINE2(1));
        clcd_putch(' ', LINE2(2));
        clcd_putch('0' + (min / 10), LINE2(3));
        clcd_putch('0' + (min % 10), LINE2(4));
        clcd_putch(' ', LINE2(5));
        clcd_putch('0' + (sec / 10), LINE2(6));
        clcd_putch('0' + (sec % 10), LINE2(7));
        clcd_print("        ", LINE2(8));

        if (hms == 0)    //hours field blinking
        {
            if (blink) {
                clcd_putch(0xFF, LINE2(0));
                clcd_putch(0xFF, LINE2(1));
            }
        } 
        else if (hms == 1) {    //minutes field blinking
            if (blink) {
                clcd_putch(0xFF, LINE2(3));
                clcd_putch(0xFF, LINE2(4));
            }
        } 
        else if (hms == 2) {        //seconds field blinking
            if (blink) {
                clcd_putch(0xFF, LINE2(6));         //for  blinking black square(0xFF)
                clcd_putch(0xFF, LINE2(7));
            }
        }
        for (unsigned long int i = 5000; i--;);  
        if (key == MK_SW2) //exit from edit mode without save
        {
            exit_key=1;
            hms = 0;
            edit_time = 0;
            back_menu = 1; //for goining back to menu 
            opt = 1;
        }
        else if (key == MK_SW1) //exit from edit mode with save
        {
            exit_key=1;
            edit_time = 0;
            back_menu = 1; 
            opt = 1;
            /* writing hours ,minutes and seconds into RTC*/
            write_ds1307(HOUR_ADDR, (hour / 10) << 4 | (hour % 10));
            write_ds1307(MIN_ADDR, (min / 10) << 4 | (min % 10));
            write_ds1307(SEC_ADDR, (sec / 10) << 4 | (sec % 10));

            hms = 0;
            CLEAR_DISP_SCREEN;
           // clcd_print("  TIME    EV  SP", LINE1(0));
            

        }
       

    } 
    else if (key == MK_SW2) //exit from menu screen
    {
        menu = 0;
        clcd_print("                ", LINE1(0));
        clcd_print("                ", LINE2(0));
        print_init(); //printing dash board
    }
}

unsigned int delay = 0;

void display_data(void) {           //displaying data on tera term
    for (int i = 0; i < event_count; i++) {
        puts(logs[i]);
        puts("\n\r");
    }
    for (int i = 5000; i--;);
    return;
}

void load_data(void) {
    CLEAR_DISP_SCREEN;          //loading logs into logs array
    int store = 0;
    for (int i = 0; i < event_count; i++) {
        for (int j = 0; j < 14; j++) {
            if (j == 8 || j == 11) {
                logs[i][j] = ' ';
            } else {
                logs[i][j] = read_extenal_EEPROM(store++);     //reading from the external eeprom
            }
        }
        logs[i][14] = '\0';
    }
}

void call_write(void) {
    //void write_internal_eeprom(unsigned char address, unsigned char data)  
    if (event_count == 10) {
        //updating the memory deleting 1st event and  shifting the all the events  
        for (int i = 0; i < (9 * 12); i++) {
            char temp = read_extenal_EEPROM(i + 12);
            write_extenal_EEPROM(i, temp);

        }
        event_count--;
        add = 9 * 12;
    }
    if (event_count < 10) {
        //        //for storing time 00:00:00 need to run 8 times
        //        for(int j=0;j<8;j++)
        //        {
        //            write_extenal_EEPROM(add++,time[j]); 
        //        }
        //         //for storing event need to run 2 times
        //         for(int j=0;j<2;j++)
        //        {
        //            write_extenal_EEPROM(add++,event[i][j]); 
        //        }
        //        if(adc_reg_val==100)
        //            adc_reg_val=99;
        //        char ch=adc_reg_val/10 +'0';
        //       write_extenal_EEPROM(add++,ch); 
        //        ch=adc_reg_val%10 +'0';
        //        write_extenal_EEPROM(add++,ch);  
        //       //for checking properly stored or not 
        //        add-=12;
        //        for(int i =0;i<12;i++)
        //        {
        //            clcd_putch(read_extenal_EEPROM(add++),LINE1(i));
        //        } 
        //FOR HOUR
        store[0] = time[0];
        store[1] = time[1];
        //FOR MIN
        store[2] = ':';
        store[3] = time[3];
        store[4] = time[4];
        //FOR SEC
        store[5] = ':';
        store[6] = time[6];
        store[7] = time[7];

        //FOR GEAR
        store[8] = event[i][0];
        store[9] = event[i][1];

        //FOR SPEED
        store[10] = (adc_reg_val / 10) + '0';
        store[11] = (adc_reg_val % 10) + '0';
        store[12] = '\0'; //END WITH '\0'

        int s = 0;
        while (store[s]) {
            // clcd_putch(read_extenal_EEPROM(add++),LINE1(s));
            write_extenal_EEPROM(add++, store[s++]); //WRITING TO THE external  EEPROM

        }
        event_count++;      //incrementing the event count
    }


}

void inc_gear(void) {        //increasing the gear
    if (i == 8)     //after collision starts from nutral(GN)
        i = 1;
    else if (i < 7)
        i++;
}

void dec_gear(void)      //decreasing the gear
{       
    if (i == 8)     //after collision starts from nutral(GN)
        i = 1;
    else if (i > 0) {
        //        clcd_print(event[i], LINE2(10));
        if (i > 1)
            i--;
    }

}

void collision(void) {      //collision mode goes to nutral
    i = 8;
    //    clcd_print(event[i], LINE2(10));
    //    i=1;
}

void change_speed(short val) {      //changed speed displying on clcd
    if (val == 100)
        val = 99;
    char digit = (val / 10) + '0';
    clcd_putch(digit, LINE2(14));
    digit = (val % 10) + '0';
    clcd_putch(digit, LINE2(15));
}
