#ifndef KEY_MSG_H
#define KEY_MMSG_H
#include "hal_types.h"
#include "hal_key.h"

/** 按应用逻辑定义 KEY 符号   **/
#define KEY_OK            HAL_KEY_SW_1    // OK键
#define KEY_CANCEL        HAL_KEY_SW_2    // CANCEL键
#define KEY_UP            MY_HAL_JOY_UP_ID  
#define KEY_DOWN          MY_HAL_JOY_DOWN_ID
#define KEY_LEFT          MY_HAL_JOY_LEFT_ID
#define KEY_RIGHT         MY_HAL_JOY_RIGHT_ID
#define KEY_CENTER        MY_HAL_JOY_CENTER_ID


typedef struct{   //KeyMsg类定义
  void (*init)(uint8 task_id);
  void (*process)(uint8 keys, uint8 isShiftPressed); 
}KEY_MSG_t;

extern KEY_MSG_t KeyMsg;  //声明KeyMsg对象

#endif
