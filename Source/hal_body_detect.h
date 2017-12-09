#ifndef HAL_BODY_DETECT_H
#define HAL_BODI_DETECT_H

#define NOBODY_STATE        0
#define SOMEBODY_STATE      1
#define SCAN_DELAY_STATE    2
#define AUTO_SCAN_STATE     3

extern void hal_body_detect_init( void );
extern void hal_body_detect_state_machine( void );
extern void hal_scan_delay_event_process( void );
extern void hal_auto_scan_event_process( void );
extern bool hal_detector_read( void );


#endif
