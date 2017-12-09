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
/***********************    ˵��   ******************************************* 
* 1) P0_6Ϊ�͵�ƽ���������ڼ�⵽�й���ǿ�����¼�
* 2) P0_6Ϊ�ߵ�ƽ������������û�й����������¼�
******************************************************************************/
  P0SEL &= ~BV(6); // ����P0_6���Ź���Ϊͨ��I/O
  P0DIR &= ~BV(6); 
  P0INP |= 0x40;
  
}

void hal_light_detect_state_machine( void )
{

  // !pinState������ǿ
  // pinState : ������
  bool pinState = hal_light_detector_read();
  
  switch ( curLightState ) {
    case LIGHT_LOW_STATE :
      // ȡ����ʱ
      count = 0;
      // ǿ����
      if ( !pinState ) {
          curLightState = LIGHT_MID_STATE;
          osal_set_event( GenericApp_TaskID, LIGHT_MID_EVT ); 
          // ��һ��ĵƣ�Ȼ��1���Ӻ�����жϣ����Ƿ������ƣ���������͹ص�ȫ����
          // osal_start_timerEx( Hal_TaskID, HAL_LIGTH_DELAY_SCAN_EVT,10000 ); //����ɨ����ʱ
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
      // ������
      if ( pinState ) {
        curLightState = LIGHT_LOW_STATE;
        osal_set_event( GenericApp_TaskID, LIGHT_LOW_EVT ); 
      }
      
      break;
  }
  
}


// ���ع����Ľ��
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