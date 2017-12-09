#ifndef GREENROOM_H
#define GREENROOM_H

#include "hal_types.h"

extern bool body_state;
extern bool light_state;
extern bool switch_state;

extern void green_room_init( void );
extern void green_room_control( void );
extern void open_half_led ( void );
extern void close_led (void);
extern void open_all_led ( void );

#endif