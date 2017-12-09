#include "GreenRoom.h"
#include "hal_types.h"
#include "iocc2530.h"
#include "hal_drivers.h"
#include "osal.h"
#include "hal_led.h"
#include "hal_light_detect.h"

#define LOW  0x00
#define MID  0x01
#define HIGH 0x02

extern bool changedshift;
bool body_state;
uint8 light_state;
bool switch_state;
bool led_state;
void green_room_init( void );

void green_room_init( void ) {
  P0SEL &= ~BV(1);
  P0DIR |= BV(1);
  P0_1 = 1;
  
  P0SEL &= ~BV(3);
  P0DIR |= BV(3);
  P0_3 = 1;
}
void green_room_control( void ){
	
  if ( body_state && switch_state ) {
    switch ( light_state ) {
        case LIGHT_LOW_STATE :
          open_all_led();
          break;
        case LIGHT_MID_STATE :
          open_half_led();
          break;
        case LIGHT_HIGH_STATE :
          close_led();
          break;
    }
  } else {
    close_led();
  }
  
  if ( !body_state && !switch_state ) {
    switch_state = true;
  }
  
}


void open_half_led() {
    led_state = true;
    HalLedSet( HAL_LED_1, led_state ); 
    P0_1 = 0;
    P0_3 = 1;
}

void close_led() {
    led_state = false;
    HalLedSet( HAL_LED_1, led_state );
    P0_1 = 1;
    P0_3 = 1;
}

void open_all_led() {
    led_state = true;
    HalLedSet( HAL_LED_1, led_state ); 
    P0_1 = 0;
    P0_3 = 0;
}
