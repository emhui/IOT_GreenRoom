#include "KeyMsg.h"
#include "OnBoard.h"

static void init(uint8 task_id);  
static void process(uint8 keys, uint8 isShiftPressed); 

KEY_MSG_t KeyMsg={
  init,
  process
};
 

/*********************************************************************
 * @fn          KeyMsg.init()
 *
 * @brief       Key��ʼ����Ӳ���ĳ�ʼ������������ɣ�����������ɴ���
 *              Key��Ϣ������ע�ᡣ
 *
 * @param       task_id: ����Key��Ϣ������ID
 *
 * @return      none
 */
static void init(uint8 task_id)
{
  RegisterForKeys( task_id ); 
}

/*********************************************************************
 * @fn          KeyMsg.process()
 *
 * @brief       Key��Ϣ����
 *
 * @param       keys: �û�����ID
 *              isShiftPressed: =TRUE:������Shift key, =FALSE: δ����Shift key
 *
 * @return      none
 */
static void process(uint8 keys, uint8 isShiftPressed)
{
  if(isShiftPressed)
  {//������Ҫ���밴��shift+keys �Ĵ������
   
  }
  else
  {// û�а���shift key 
    switch( keys & ~0x02 )
    {
    case KEY_OK:
      Led.on(LED1);
      SerialCom.send( "LED1 is ON\n", sizeof("LED1 is ON\n") );
      break;
    case KEY_CANCEL:
      Led.off(LED1);
      SerialCom.send( "LED1 is OFF\n", sizeof("LED1 is OFF\n") );      
      break;
    case KEY_UP:
      SerialCom.send( "joy stick up\n", sizeof("joy stick up\n") );
      break;
    case KEY_DOWN:
      SerialCom.send( "joy stick down\n", sizeof("joy stick down\n") );
      break;
    case KEY_LEFT:
      SerialCom.send( "joy stick left\n", sizeof("joy stick left\n") );
      break;
    case KEY_RIGHT:
      SerialCom.send( "joy stick right\n", sizeof("joy stick right\n") );
      break;
    case KEY_CENTER:
      SerialCom.send( "joy stick center\n", sizeof("joy stick center\n") );
      break;
    }
  }
}

