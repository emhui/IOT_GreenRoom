#include "hal_drivers.h"
#include "hal_light_detect.h"
#include "iocc2530.h"
#include "osal.h"
#include "hal_mcu.h"
#include "body_detect_events.h"
#include "hal_led.h"

extern uint8 GenericApp_TaskID;
static uint8 curLightState = LIGHT_HIGH_STATE;
uint8 count = 0;
void hal_finish_light_event_process( void );

void hal_light_detect_init( void )
{
/***********************    说明   ******************************************* 
* 1) P0_6为低电平触发，用于检测到有光照强触发事件
* 2) P0_6为高电平触发，用于在没有光照弱触发事件
******************************************************************************/
  P0SEL &= ~BV(6); // 设置P0_6引脚功能为通用I/O
  P0DIR &= ~BV(6); 
  P0INP |= 0x40;
  
}

void hal_light_detect_state_machine( void )
{

  // !pinState：光照强
  // pinState : 光照弱
  bool pinState = hal_light_detector_read();
  
  switch ( curLightState ) {
    case LIGHT_LOW_STATE :
      // 取消延时
      count = 0;
      // 强光下
      if ( !pinState ) {
          curLightState = LIGHT_MID_STATE;
          osal_set_event( GenericApp_TaskID, LIGHT_MID_EVT ); 
          // 开一半的灯，然后1分钟后进行判断，看是否还是亮灯，如果还亮就关掉全部的
          // osal_start_timerEx( Hal_TaskID, HAL_LIGTH_DELAY_SCAN_EVT,10000 ); //进行扫描延时
      }
      
      break;
    case LIGHT_MID_STATE :
      if ( ++count == 20 ) {
        hal_finish_light_event_process();
        count = 0;
      }
      
      if ( pinState ) {
        curLightState = LIGHT_LOW_STATE;
        osal_set_event( GenericApp_TaskID, LIGHT_LOW_EVT );
        // osal_stop_timerEx(Hal_TaskID, HAL_LIGTH_DELAY_SCAN_EVT);
      }
      break;
      
    case LIGHT_HIGH_STATE :
      // 光照弱
      if ( pinState ) {
        curLightState = LIGHT_LOW_STATE;
        osal_set_event( GenericApp_TaskID, LIGHT_LOW_EVT ); 
      }
      
      break;
  }
  
}


// 返回光敏的结果
bool hal_light_detector_read( void )
{
  return P0_6;
}

void hal_finish_light_event_process( void )
{
  if ( curLightState == LIGHT_MID_STATE ) {
        curLightState = LIGHT_HIGH_STATE;
        osal_set_event( GenericApp_TaskID, LIGHT_HIGH_EVT ); 
  }
}