/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include "sys/alt_irq.h"
#include "system.h"
#define clk_mode 1 ;
#define chrono_mode 0;
unsigned char seven_seg[10]={0x3F , 0x06 ,0x5B ,0x4F , 0x66 , 0x6D ,0x7D, 0x07 ,0x7F ,0x6F};
unsigned char * seg_ptr = (unsigned char *) SEG_CONTROLLER_BASE;
volatile short *timer_ptr =(unsigned int *)TIMER_0_BASE;
volatile  int *key_ptr = (unsigned int*)KEY_BASE;
volatile int edge_capture_timer ;
volatile int edge_capture_key;
volatile int press_key;
unsigned int s = 0 ,s_clk = 0 , m_clk = 40, h_clk = 18 , m = 0 ;//minutes and secondes
unsigned char StartStop = 1 , mode = clk_mode;

void init_timer();
void handle_timer_interrupts(void * context , alt_u32 id);
void init_key();
void handle_key_interrupt(void* context , alt_u32 id);


int main()
{
  printf("Hello from Nios II!\n");
  *seg_ptr = 0x3F;
  *(seg_ptr + 1) = 0x06;
  *(seg_ptr + 2) = 0x5B;
  *(seg_ptr + 3) = 0x6F;
  init_timer();
  init_key();
  while(1);
}

void init_timer(){
	void *edge_capture_timer_ptr = (void*)&edge_capture_timer ;;
	*(timer_ptr +1 )=0x07; //ITO = 1 ,CONT = 1 ,START = 1 , STOP = 0
	alt_irq_register(TIMER_0_IRQ, edge_capture_timer_ptr  ,handle_timer_interrupts);
}
void handle_timer_interrupts(void * context , alt_u32 id){
	(*timer_ptr)= 0;
	if (mode == 1){ // testing if the mode is clk (m=1)
		*(seg_ptr) = seven_seg[(m_clk%10)];
		*(seg_ptr + 1) = seven_seg[(m_clk/10)];
		*(seg_ptr + 2) = seven_seg[(h_clk%10)];
		*(seg_ptr + 3) = seven_seg[(h_clk/10)];
	}
	else {
	*(seg_ptr) = seven_seg[(s%10)];
	*(seg_ptr + 1) = seven_seg[(s/10)];
	*(seg_ptr + 2) = seven_seg[(m%10)];
	*(seg_ptr + 3) = seven_seg[(m/10)];
	}
	if (StartStop == 1){
		s = (s+1)%60;
		if (s == 0) m =(m+1)%60;
	}
	s_clk = (s_clk +1)%60 ;
	if (s_clk == 0) m_clk = (m_clk +1)%60;
	if (m_clk == 0) h_clk = (h_clk +1)%24;

}
void init_key(){
	void  *edge_capture_key_ptr = (void*)&edge_capture_key;
	*(key_ptr + 2) = 0xF;
	alt_irq_register(KEY_IRQ,edge_capture_key_ptr,handle_key_interrupt);
}
void handle_key_interrupt(void* context , alt_u32 id){
	press_key = *(key_ptr +3);
	if(press_key & 0x8){ //testing for key 3 if it is pressed
		s = 0 ;
		m = 0;
        StartStop = 0;
		*(key_ptr + 3) = 0;
		return;
	}
	if(press_key & 0x4){ //testing for key 2 if it is pressed
		StartStop ^= 1;
		*(key_ptr + 3) = 0;
		return;
	}
	if(press_key & 0x2){ //testing for key 1 if it is pressed
		mode ^= 1;
		*(key_ptr + 3) = 0;
		return;
	}
	if(press_key & 0x1){ //testing for key 0 if it is pressed

		*(key_ptr + 3) = 0;
		return;
	}
}

