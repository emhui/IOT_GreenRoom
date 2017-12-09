#include "hal_drivers.h"
#include "hal_switch_detect.h"
#include "iocc2530.h"
#include "osal.h"
#include "hal_mcu.h"
#include "body_detect_events.h"
#include "hal_step_motor.h"
#include "hal_led.h"
#include "GreenRoom.h"

extern bool led_state;  //��ǰ���ҵƹ⹤��״̬
extern uint8 GenericApp_TaskID;
bool pinPreDisturbState = false; 
 
void hal_switch_detect_init( void )
{
/***********************    ˵��   ******************************************* 
******************************************************************************/
  // �����Ž��տ��ص��ź�--��ģ����ҵĿ���
  P0SEL &= ~BV(5); // ����P0_5���Ź���Ϊͨ��I/O
  P0DIR &= ~BV(5); 
  P0INP &= ~BV(5);
}

void hal_switch_detect_state_machine( void )
{
    
  bool pinDisturbState;
  pinDisturbState = hal_disturb_read();
  
  // ��������Ϊ�����л�,��״̬�л�����Ϊ����״̬
  if (pinPreDisturbState != pinDisturbState) {
    if ( switch_state ) {
        osal_set_event( GenericApp_TaskID, STOP_WORK_EVT );  // ֪ͨӦ�ò㡰��������������
    } 
    
    if ( !switch_state ){       
      osal_set_event( GenericApp_TaskID, START_WORK_EVT );  // ֪ͨӦ�ò㡰��������������
    } 
    pinPreDisturbState = pinDisturbState;
  }
  
}

// ���ؿ���
bool hal_disturb_read( void )
{
  return P0_5;
}

