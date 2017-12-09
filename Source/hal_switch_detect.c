#include "hal_drivers.h"
#include "hal_switch_detect.h"
#include "iocc2530.h"
#include "osal.h"
#include "hal_mcu.h"
#include "body_detect_events.h"
#include "hal_step_motor.h"
#include "hal_led.h"
#include "GreenRoom.h"

extern bool led_state;  //当前教室灯光工作状态
extern uint8 GenericApp_TaskID;
bool pinPreDisturbState = false; 
 
void hal_switch_detect_init( void )
{
/***********************    说明   ******************************************* 
******************************************************************************/
  // 该引脚接收开关的信号--》模拟教室的开关
  P0SEL &= ~BV(5); // 设置P0_5引脚功能为通用I/O
  P0DIR &= ~BV(5); 
  P0INP &= ~BV(5);
}

void hal_switch_detect_state_machine( void )
{
    
  bool pinDisturbState;
  pinDisturbState = hal_disturb_read();
  
  // 进行了人为开关切换,将状态切换到人为干扰状态
  if (pinPreDisturbState != pinDisturbState) {
    if ( switch_state ) {
        osal_set_event( GenericApp_TaskID, STOP_WORK_EVT );  // 通知应用层“传感器不工作”
    } 
    
    if ( !switch_state ){       
      osal_set_event( GenericApp_TaskID, START_WORK_EVT );  // 通知应用层“传感器不工作”
    } 
    pinPreDisturbState = pinDisturbState;
  }
  
}

// 返回开关
bool hal_disturb_read( void )
{
  return P0_5;
}

