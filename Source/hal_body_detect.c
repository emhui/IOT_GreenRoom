#include "hal_drivers.h"
#include "hal_body_detect.h"
#include "iocc2530.h"
#include "osal.h"
#include "hal_mcu.h"
#include "body_detect_events.h"
#include "hal_step_motor.h"
#include "hal_led.h"

extern uint8 GenericApp_TaskID;
static uint8 curState = NOBODY_STATE;

void hal_body_detect_init( void )
{
/***********************    说明   ******************************************* 
* 我们将P1_2和P2_1用做人体传感器的输入引脚，并采用中断通信方式。其中：
* 1) P1_2为高电联触发，用于检测到有人活动时触发事件
* 2) P2_1为低电平触发，用于在没有人活动时触发事件
******************************************************************************/
  P0SEL &= ~BV(4); // 设置P0_4引脚功能为通用I/O
  P0DIR &= ~BV(4); 
  P0INP &= ~BV(4);
//  P0INP |= BV(4);
  P2INP |= BV(5);

}

void hal_body_detect_state_machine( void )
{

  bool pinState;      
  pinState = hal_detector_read();
  HalLedSet( HAL_LED_2, pinState );

  switch( curState )
  {
  case NOBODY_STATE:
    if( pinState )  // 如果为高电平，即有人（活动）
    {
      curState = SOMEBODY_STATE;  //切换到SOMEBODY_STATE
      osal_set_event( GenericApp_TaskID, SOMEBODY_EVT );  // 通知应用层“有人”
    }
    break;
  case SOMEBODY_STATE:
    if( !pinState ) // 如果为低电平，即没有人（活动）
    { 
      curState = SCAN_DELAY_STATE;  //切换到SCAN_DELAY_STATE
      osal_start_timerEx( Hal_TaskID, HAL_SCAN_DELAY_FINISH_EVT,6000 ); //进行扫描延时
    }
    break;
  case SCAN_DELAY_STATE:
    if( pinState ) // 如果为高电平，即扫描到有人（活动）
    { 
      curState = SOMEBODY_STATE;  //切换到SOMEBODY_STATE，
      osal_stop_timerEx(Hal_TaskID, HAL_SCAN_DELAY_FINISH_EVT);
    }   
    break;
  case AUTO_SCAN_STATE:
   if( pinState ) // 如果为高电平，即扫描到有人（活动）
    { 
      curState = SOMEBODY_STATE;  //切换到SOMEBODY_STATE
    }      
    break;     
  }

}


bool hal_detector_read( void )
{
  return P0_4;
}
void hal_scan_delay_event_process( void )
{
  curState = AUTO_SCAN_STATE;  //切换到AUTO_SCAN_STATE
  hal_step_motor_round_trip( 20 );  //启动自动扫描  
}

void hal_auto_scan_event_process( void )
{
  if( AUTO_SCAN_STATE == curState )
  {
    curState = NOBODY_STATE;  //切换到NOBODY_STATE
    osal_set_event( GenericApp_TaskID, NOBODY_EVT );  // 通知应用层“没人”
  }
}

