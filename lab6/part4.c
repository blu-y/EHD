#include "address_map_nios2.h"

static volatile int *HEX3_0_ptr = (int*) HEX3_HEX0_BASE;
static volatile int *HEX5_4_ptr = (int*) HEX5_HEX4_BASE;
static volatile int *TIMER_ptr = (int*) TIMER_BASE;
static volatile int *KEY_ptr = (int*) KEY_BASE;
static int *TIMER_ctrl = (int*)(TIMER_BASE+4);
static int *TIMER_pl = (int*)(TIMER_BASE+8);
static int *TIMER_ph = (int*)(TIMER_BASE+12);
// memmory mapped IO to volatile variable
// TIMER setting don't have to be volatile variable

int read_key(void){
    return *KEY_ptr & 0b1111;
} // read last 4 bit of KEY_BASE

int power(int a, int b){
    int p = 1;
    for (int i=0; i<b; i++) p *= a;
    return p;
} // get a^b

int HEX[11] = {
    0b00111111, 0b00000110, 0b01011011, 0b01001111,
    0b01100110, 0b01101101, 0b01111101, 0b00000111,
    0b01111111, 0b01100111, 0b00000000
    }; // BIT_CODES : 0 1 2 3 4 5 6 7 8 9 NULL

void display_hex(int n){
    // display 6 digit number to hex
    int h[6] = {0}; // array for 6 digit
    unsigned int hex0 = 0; // bit codes for HEX 3-0
    // unsigned int hex4 = 0; // bit codes for HEX 5-4
    for (int i = 0; i < 6; i++){
        h[i] = n / power(10, 5-i);
        n = n - h[i] * power(10, 5-i);
    } // separate 6 digit by dividing 10^(5~0)
    for (int i = 2; i <= 5; i++){
        hex0 += HEX[h[i]]<<(8*(5-i));
    } // HEX3-0 = HEX3<<24 + HEX2<<16 + HEX1<<8 + HEX0
    *HEX3_0_ptr = hex0;
    // save HEX3-0 to HEX3_HEX0_BASE
    /*
    for (int i = 0; i <= 1; i++){
        hex4 += HEX[h[i]]<<(8*(1-i));
    } // HEX5-4 = HEX5<<8 + HEX4
    *HEX5_4_ptr = hex4;
    // save HEX5-4 to HEX5_HEX4_BASE
    */
    // no need to save HEX5-4
}

void set_timer(int us){
    // input : period in microsec
    *TIMER_ptr = 0; // initialize
    us = 100*us; // unit time 0.01us so us*100
    *TIMER_pl = us;
    *TIMER_ph = us>>16;
    // low 16 bit to periodl, high 16 bit to periodh
    *TIMER_ctrl = 0b0110; // STOP 0, START 1, CONT 1, ITO 0
}

int timer(int sec){
    int t = *TIMER_ptr;
    if (t&1){
        sec+=1;
        *TIMER_ptr = 0;
    } // if TO==1 then sec+=1 and make TO=0
    if (sec == 3600) sec = 0;
    // if 1:00:00 then make 00:00
    return sec;
}

int sec2time(int sec){
    // make sec to MMSS by dividing 60
    // quotient=MM, remainder=SS
    int min = sec/60;
    sec = sec%60;
    return min*100+sec;
}

int main(void){
    int sec = 0;
    int run = 1;
    *HEX3_0_ptr = 0; // initialize
    *HEX5_4_ptr = 0; // initialize
    *TIMER_ptr = 0; // initialize
    set_timer(1000000);
    while(1){
        if (read_key()) {
            // if KEY pressed
            while (read_key()) read_key(); // wait until KEY release
            if (run){
                run = 0;
                *TIMER_ctrl = 0b1010; // STOP 1, START 0, CONT 1, ITO 0
            } // if running then make run=0, stop timer
            else{
                run = 1;
                *TIMER_ctrl = 0b0110; // STOP 0, START 1, CONT 1, ITO 0
            } // if not running than make run=1, start timer
        }
        if (run) sec = timer(sec); // if running, then sec+=1 when TO=1 
        display_hex(sec2time(sec)); // display MMSS to HEX
    }
}

