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
 * @brief       Key初始化。硬件的初始化在驱动层完成，本函数仅完成处理
 *              Key消息的任务注册。
 *
 * @param       task_id: 处理Key消息的任务ID
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
 * @brief       Key消息处理。
 *
 * @param       keys: 用户按键ID
 *              isShiftPressed: =TRUE:按下了Shift key, =FALSE: 未按下Shift key
 *
 * @return      none
 */
static void process(uint8 keys, uint8 isShiftPressed)
{
  if(isShiftPressed)
  {//根据需要加入按下shift+keys 的处理代码
   
  }
  else
  {// 没有按下shift key 
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

