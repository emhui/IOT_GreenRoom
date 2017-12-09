#ifndef HAL_LIGHT_DETECT_H
#define HAL_LIGHT_DETECT_H

#define LIGHT_HIGH_STATE    0X04
#define LIGHT_LOW_STATE     0X01
#define LIGHT_MID_STATE     0X02



extern void hal_light_detect_init( void );
extern void hal_light_detect_state_machine( void );
extern bool hal_light_detector_read( void );
extern void hal_finish_light_event_process( void );


#endif
