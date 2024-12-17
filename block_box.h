#ifndef BLOCK_BOX_H
#define	BLOCK_BOX_H

void  print_init(void); 

void inc_gear(void);
void dec_gear(void);

void  collision(void);
void change_speed(short val);

void go_menu(void);
void  call_write(void);
void load_data(void);
void  display_data(void);
//void view_log(void);
void set_time(void);

#endif	/* BLOCK_BOX_H */