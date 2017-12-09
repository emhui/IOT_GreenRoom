#ifndef KEY_MSG_H
#define KEY_MMSG_H
#include "hal_types.h"
#include "hal_key.h"

/** ��Ӧ���߼����� KEY ����   **/
#define KEY_OK            HAL_KEY_SW_1    // OK��
#define KEY_CANCEL        HAL_KEY_SW_2    // CANCEL��
#define KEY_UP            MY_HAL_JOY_UP_ID  
#define KEY_DOWN          MY_HAL_JOY_DOWN_ID
#define KEY_LEFT          MY_HAL_JOY_LEFT_ID
#define KEY_RIGHT         MY_HAL_JOY_RIGHT_ID
#define KEY_CENTER        MY_HAL_JOY_CENTER_ID


typedef struct{   //KeyMsg�ඨ��
  void (*init)(uint8 task_id);
  void (*process)(uint8 keys, uint8 isShiftPressed); 
}KEY_MSG_t;

extern KEY_MSG_t KeyMsg;  //����KeyMsg����

#endif
