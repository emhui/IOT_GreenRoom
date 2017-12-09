#ifndef HAL_SWITCH_DETECT_H
#define HAL_SWITCH_DETECT_H

extern void hal_switch_detect_init( void );
extern void hal_switch_detect_state_machine( void );
extern bool hal_disturb_read( void );

#endif
