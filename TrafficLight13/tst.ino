#include <avr/io.h>  
//uint16_t cnt __attribute__ ((section (".noinit")));  //.noinit

//uint8_t cnt;	//global, .bss
uint8_t cnt __attribute__ ((section (".noinit")));  //.noinit
int main(){
	uint8_t cnt;	//local
    cnt=0;      // < setup()
    while(1){
        cnt++;  // < loop()
    }
}

//Notepad++ F5 (меню Запуск-Запуск)
//"C:\Users\...\Documents\Arduino\TrafficLight13\gcc\0_MAKE & asm-O0.cmd" $(FULL_CURRENT_PATH)
//Program:      58 bytes (5.7% Full) - local
//Program:      62 bytes (6.1% Full) - noinit
//Program:      78 bytes (7.6% Full) - global

// add uint16_t vars check for ROM paying
// uint16_t noinit Program:      74 bytes (7.2% Full)
// uint16_t local  Program:      64 bytes (6.3% Full)

//"C:\Users\...\Documents\Arduino\TrafficLight13\gcc\0_MAKE & asm-O0.cmd" $(FULL_CURRENT_PATH)
//ATMega328
// Program:     152 bytes (0.5% Full)  - local
// Program:     156 bytes (0.5% Full)  - noinit
// Program:     172 bytes (0.5% Full)  - global





