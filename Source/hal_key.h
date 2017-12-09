#ifndef MY_HAL_KEY_H
#define MY_HAL_KEY_H
#include "hal_types.h"

typedef void (*halKeyCBack_t) (uint8 keys, uint8 state);
typedef struct{   //KEY类定义
  uint8 port;     // key所在的端口号，取什为MY_PORT0、MY_PORT1或MY_PORT2，对应于端口0、1、2
  uint8 pin;       // key所连接的引脚
  uint8 keyID;     // 按键 ID
  uint8 polarity;  // 按键有效极性，取值为MY_KEY_ACTIVE_HIGH和MY_KEY_ACTIVE_LOW，分别代表高电平有和低电平有效
}KEY_t;

typedef struct{
  uint8 keyID;
  uint8 lowerLimit; 
  uint8 upperLimit;
}JOY_STICK_AD_RANGE_t;


#define MY_PORT0    0x00  // GPIO P0
#define MY_PORT1    0x01  // GPIO P1
#define MY_PORT2    0x02  // GPIO P2
#define MY_KEY_ACTIVE_HIGH  0x01  // 按键有效输入极性：高电平
#define MY_KEY_ACTIVE_LOW  0x00   // 按键有效输入极性：低电平

/* 按键(Switch)ID定义(keyID）*/
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


/* 触发方式定义 */
#define RISSING_EDGE   0  // 上升沿触发
#define FALLING_EDGE   1  // 下降沿触发

/** 下面的定义是为了与原驱动保持兼容 **/
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
