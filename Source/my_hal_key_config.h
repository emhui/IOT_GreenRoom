#ifndef MY_HAL_KEY_CONFIG_H
#define MY_HAL_KEY_CONFIG_H

#include "ioCC2530.h"
#include "hal_types.h"
#include "hal_defs.h"
#include "hal_key.h"

/**  按键配置步骤，共五步  **/

/***   第一步： 配置按键输入模式及相关参数 **/
    /*** 1.1步： 是否使用中断模式 ***/
      #define KEY_INPUT_INT_MODE     FALSE    // TRUE: 中断模式， FALSE: 查询模式

    /*** 1.2步： 如果使用了中断功能，则需要进一步给出哪些端口使用了中断以及触发模式**/
      #if ( KEY_INPUT_INT_MODE == TRUE )
      
          #define P0_INT_ENABLE  TRUE  // PO连接了按键，则需要定义为TRUE, 否则为FALSE
          #define P1_INT_ENABLE  FALSE  // P1连接了按键，则需要定义为TRUE, 否则为FALSE
          #define P2_INT_ENABLE  TRUE  // P2连接了按键，则需要定义为TRUE, 否则为FALSE
          #define EDGE_TRIGGER_MODE FALLING_EDGE   // 中断触发方式，取值：RISSING_EDGE 或 FALLING_EDGE
                                                   // 分别表示：上升沿触发、下降沿触发
      #endif

/***   第二步：配置普通按键（即以高低电平、而非模拟信号为输入信号的按键 **/
    /*** 2.1步， 给出按键数量，要求：KEY_NUM+joystick <=8 。 如启用了joy stick，则其数量为5，否则为0***/
      #define KEY_NUM     2     // 普通按键数量，该值决定了按键配置数组keyConfig[]的长度，
                           //  如果没有普通按键，则将此行注释掉，或设置为0
    /*** 2.2步，如果启用了普通按键功能，则需要用户进一步给出各按键所在的端口、引脚、key id、极性  */
      #if (defined KEY_NUM) && (KEY_NUM > 0)
      
          // 端口 -- 位序  --  按键名   --  低电平接通
          const KEY_t keyConfig[KEY_NUM] = { //每一行配置一个按键，
              MY_PORT0, BV(1),  HAL_KEY_SW_1,  MY_KEY_ACTIVE_LOW,   // 极性取值为：MY_KEY_ACTIVE_LOW或MY_KEY_ACTIVE_HIGTH
              MY_PORT2, BV(0),  HAL_KEY_SW_2,  MY_KEY_ACTIVE_LOW    // 分别表示低电平有效或高电平有效
            };
      
      #endif

/***   第三步：配置SHIFT按键 **/

  /*** 3.1步： 是否使用SHIFT按建功能 ***/
      #define SHIF_KEY_CONFIGURED    FALSE   // 是否启用shift key功能。如果是，则将其设置为TRUE 

   /*** 3.2步： 如果启用了shift key 功能，则需要进一步给出其使用的ID  ***/
      #if (SHIF_KEY_CONFIGURED == TRUE)

          #define MY_SHIFT_KEY_ID      MY_HAL_KEY3_ID    //**** 注意不要与其他key id重复，各种key ID的定义在hal_key.h中

       #endif

/***   第四步：配置joy stick 按键 **/

  /*** 4.1步： 是否使用 joy stick 功能 ***/
      #define JOY_KEY_CONFIGURED     FALSE   // 是否启用joy stick功能。如果是，则将其设置为TRUE

  /** 4.2步：如果启用了joy stick功能，则用户需要进一步给出其使用的AD通道和各键的AD取值范围  ***/
      #if (JOY_KEY_CONFIGURED == TRUE)
      
          #define JOY_AD_CHANNEL 0x06   // joy stick使用的AD通道
      
          const JOY_STICK_AD_RANGE_t joyStickADRange[5] = {  // 给出joy stick各键的AD取值范围
          // 格式： joy stick key id, 下限值(lower limit)， 上限值(upper limit)
            MY_HAL_JOY_DOWN_ID,   20,   30,
            MY_HAL_JOY_LEFT_ID,   45,   55,
            MY_HAL_JOY_CENTER_ID, 70,   80,
            MY_HAL_JOY_UP_ID,     95,   105,
            MY_HAL_JOY_RIGHT_ID,  117,  127
          };
      
      #endif 
        
/***   第五步：配置按键自动重复间隔时间 **/       
#define KEY_REPETITION_INTERVAL  5     //  按键重复间隔，单位：100ms。即按住一个键不放，
                                       // 多长时间后开始自动重复，该参数仅对查询模式有效.


#endif
