#ifndef MY_HAL_KEY_H
#define MY_HAL_KEY_H
#include "hal_types.h"

typedef void (*halKeyCBack_t) (uint8 keys, uint8 state);
typedef struct{   //KEY�ඨ��
  uint8 port;     // key���ڵĶ˿ںţ�ȡʲΪMY_PORT0��MY_PORT1��MY_PORT2����Ӧ�ڶ˿�0��1��2
  uint8 pin;       // key�����ӵ�����
  uint8 keyID;     // ���� ID
  uint8 polarity;  // ������Ч���ԣ�ȡֵΪMY_KEY_ACTIVE_HIGH��MY_KEY_ACTIVE_LOW���ֱ����ߵ�ƽ�к͵͵�ƽ��Ч
}KEY_t;

typedef struct{
  uint8 keyID;
  uint8 lowerLimit; 
  uint8 upperLimit;
}JOY_STICK_AD_RANGE_t;


#define MY_PORT0    0x00  // GPIO P0
#define MY_PORT1    0x01  // GPIO P1
#define MY_PORT2    0x02  // GPIO P2
#define MY_KEY_ACTIVE_HIGH  0x01  // ������Ч���뼫�ԣ��ߵ�ƽ
#define MY_KEY_ACTIVE_LOW  0x00   // ������Ч���뼫�ԣ��͵�ƽ

/* ����(Switch)ID����(keyID��*/
#define HAL_KEY_SW_1 0x01  // Joystick up
#define HAL_KEY_SW_2 0x02  // Joystick right
#define HAL_KEY_SW_3 0x04  // Joystick center
#define HAL_KEY_SW_4 0x08  // Joystick left
#define HAL_KEY_SW_5 0x10  // Joystick down
#define HAL_KEY_SW_6 0x20  // Button S1 if available
#define HAL_KEY_SW_7 0x40  // Button S2 if available
#define HAL_KEY_SW_8 0x80  // Button S2 if available

#define MY_HAL_SHIFT_KEY_ID   0x04
#define MY_HAL_JOY_CENTER_ID 0x08
#define MY_HAL_JOY_UP_ID     0x10
#define MY_HAL_JOY_DOWN_ID   0x20
#define MY_HAL_JOY_RIGHT_ID  0x40
#define MY_HAL_JOY_LEFT_ID   0x80


/* ������ʽ���� */
#define RISSING_EDGE   0  // �����ش���
#define FALLING_EDGE   1  // �½��ش���

/** ����Ķ�����Ϊ����ԭ�������ּ��� **/
#define HAL_KEY_INTERRUPT_DISABLE FALSE
#define HAL_KEY_INTERRUPT_ENABLE    TRUE

extern bool Hal_KeyIntEnable;
/*
 * Initialize the Key Service
 */
extern void HalKeyInit( void );

/*
 * Configure the Key Service
 */
extern void HalKeyConfig( bool interruptEnable, const halKeyCBack_t cback);

/*
 * Read the Key status
 */
extern uint8 HalKeyRead( void);

/*
 * Enter sleep mode, store important values
 */
extern void HalKeyEnterSleep ( void );

/*
 * Exit sleep mode, retore values
 */
extern uint8 HalKeyExitSleep ( void );

/*
 * This is for internal used by hal_driver
 */
extern void HalKeyPoll ( void );

/*
 * This is for internal used by hal_sleep
 */
extern bool HalKeyPressed( void );


#endif
