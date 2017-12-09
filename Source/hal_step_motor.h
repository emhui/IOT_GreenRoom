#ifndef HAL_STEP_MOTOR_H
#define HAL_STEP_MOTOR_H
#include "hal_types.h"

typedef struct{  
  uint8 roudCount; //路径数量 
  bool dir[3];  //方向
  uint16 steps[3];  //步数  
}STEP_MOTOR_ROUND_TRIP_ROOT_t;  

#define ANTI_CLOCK      0   //逆时针
#define CLOCK_WISE      1   //顺时针
#define STEP_MOTOR_NORMAL_MODE        0
#define STEP_MOTOR_ROUND_TRIP_MODE    1


extern void hal_step_motor_init( void );
extern void hal_step_motor_start( uint8 direction, uint16 steps, uint16 speed );
extern void hal_step_motor_round_trip(  uint16 speed );
extern void hal_step_motor_stop( void );
extern void hal_step_motor_drive( void );



#endif
