#include "hal_step_motor.h"
#include "iocc2530.h"
#include "hal_drivers.h"
#include "osal.h"

static uint8 coil_state[] = {0x01, 0x03, 0x02, 0x06, 0x04,0x0c, 0x08, 0x09};
static uint16 target_steps = 0; //��Ҫ�����Ĳ���,���Ϊ0����ʾһֱת����ȥ
static uint16 stepped_steps = 0;  //�Ѳ����Ĳ���
static uint8 stepping_direction = ANTI_CLOCK;  // ��������
static uint8 stepping_mode = STEP_MOTOR_NORMAL_MODE;
static uint8 curRoute = 0; //�����ع���ģʽ�µ�ǰ·��

STEP_MOTOR_ROUND_TRIP_ROOT_t route_trip_rout = {3,{ANTI_CLOCK, CLOCK_WISE,ANTI_CLOCK},{200,400,200}};

static void move_to_next_step( void );

void hal_step_motor_init( void )
{
  P1SEL &= 0x0f; 
  P1DIR |= 0x0f0;  
  P1 = (P1 & 0x0f ); //����ϵ�  
}

void hal_step_motor_start( uint8 direction, uint16 steps, uint16 speed )
{
  stepping_direction = direction;
  target_steps = steps;  
  stepped_steps = 0;
  stepping_mode = STEP_MOTOR_NORMAL_MODE;
  osal_start_reload_timer( Hal_TaskID, HAL_STEP_MOTOR_EVT,speed );
}
void hal_step_motor_round_trip(  uint16 speed )
{
  stepping_mode = STEP_MOTOR_ROUND_TRIP_MODE;
  curRoute = 0;
  stepped_steps = 0;
  target_steps = route_trip_rout.steps[ curRoute ];
  stepping_direction = route_trip_rout.dir[ curRoute ]; 
  osal_start_reload_timer( Hal_TaskID, HAL_STEP_MOTOR_EVT,speed );
}
void hal_step_motor_stop( void )
{
  osal_stop_timerEx(Hal_TaskID, HAL_STEP_MOTOR_EVT);
}

void hal_step_motor_drive( void )
{
 
  switch( stepping_mode )
  {
  case STEP_MOTOR_NORMAL_MODE:
    if( 0 == target_steps ) 
    {//target_steps == 0����һֱת��
      break;
    }
    if( stepped_steps < target_steps )
    {//��û������Ŀ�경�����������
      stepped_steps++;
      break;
    }
    //�������в�������ֹͣ����
    osal_stop_timerEx(Hal_TaskID, HAL_STEP_MOTOR_EVT); // ֹͣ����
     P1 = (P1 & 0x0f ); //����ϵ�  
    return;
  case STEP_MOTOR_ROUND_TRIP_MODE:
    if( stepped_steps < target_steps )
    {//��û������Ŀ�경�����������
      stepped_steps++;
      break;
    }
    if( curRoute < route_trip_rout.roudCount )
    {// ��û����������·�����������һ��
      curRoute++;
      stepped_steps = 0;
      target_steps = route_trip_rout.steps[ curRoute ];
      stepping_direction = route_trip_rout.dir[ curRoute ];
      break;
    }
    // ��������·��
    osal_stop_timerEx(Hal_TaskID, HAL_STEP_MOTOR_EVT); // ֹͣ����
    P1 = (P1 & 0x0f ); //����ϵ�  
    osal_set_event(Hal_TaskID,HAL_STEP_MOTOR_ROUND_TRIP_FINISH_EVT); //֪ͨ������������   
    return;    
  }
  // ����
  move_to_next_step();  
}

static void move_to_next_step( void )
{
  static int8 curState = 0; // ��ǰ����ͨ��״̬  
  if( ANTI_CLOCK == stepping_direction )
  {
    curState++;
    curState = (curState > 7)? 0: curState;
  }
  else
  {
    curState--;
    curState = ( curState < 0)? 7: curState;
  }
  P1 = (P1 & 0x0f ) | ( coil_state[curState] << 4 );  
}