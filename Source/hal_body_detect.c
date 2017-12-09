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
/***********************    ˵��   ******************************************* 
* ���ǽ�P1_2��P2_1�������崫�������������ţ��������ж�ͨ�ŷ�ʽ�����У�
* 1) P1_2Ϊ�ߵ������������ڼ�⵽���˻ʱ�����¼�
* 2) P2_1Ϊ�͵�ƽ������������û���˻ʱ�����¼�
******************************************************************************/
  P0SEL &= ~BV(4); // ����P0_4���Ź���Ϊͨ��I/O
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
    if( pinState )  // ���Ϊ�ߵ�ƽ�������ˣ����
    {
      curState = SOMEBODY_STATE;  //�л���SOMEBODY_STATE
      osal_set_event( GenericApp_TaskID, SOMEBODY_EVT );  // ֪ͨӦ�ò㡰���ˡ�
    }
    break;
  case SOMEBODY_STATE:
    if( !pinState ) // ���Ϊ�͵�ƽ����û���ˣ����
    { 
      curState = SCAN_DELAY_STATE;  //�л���SCAN_DELAY_STATE
      osal_start_timerEx( Hal_TaskID, HAL_SCAN_DELAY_FINISH_EVT,6000 ); //����ɨ����ʱ
    }
    break;
  case SCAN_DELAY_STATE:
    if( pinState ) // ���Ϊ�ߵ�ƽ����ɨ�赽���ˣ����
    { 
      curState = SOMEBODY_STATE;  //�л���SOMEBODY_STATE��
      osal_stop_timerEx(Hal_TaskID, HAL_SCAN_DELAY_FINISH_EVT);
    }   
    break;
  case AUTO_SCAN_STATE:
   if( pinState ) // ���Ϊ�ߵ�ƽ����ɨ�赽���ˣ����
    { 
      curState = SOMEBODY_STATE;  //�л���SOMEBODY_STATE
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
  curState = AUTO_SCAN_STATE;  //�л���AUTO_SCAN_STATE
  hal_step_motor_round_trip( 20 );  //�����Զ�ɨ��  
}

void hal_auto_scan_event_process( void )
{
  if( AUTO_SCAN_STATE == curState )
  {
    curState = NOBODY_STATE;  //�л���NOBODY_STATE
    osal_set_event( GenericApp_TaskID, NOBODY_EVT );  // ֪ͨӦ�ò㡰û�ˡ�
  }
}

