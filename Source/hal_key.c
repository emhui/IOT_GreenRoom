#include "hal_key.h"
#include "my_hal_key_config.h"
#include "hal_adc.h"
#include "OnBoard.h"
#include "hal_drivers.h"

//#define NO_TASK_ID 0xFF

#if (defined HAL_KEY) && (HAL_KEY == TRUE)

bool Hal_KeyIntEnable = KEY_INPUT_INT_MODE;   
static bool HalKeyConfigured = FALSE;  // 按键是否初始化完成

/**************************************************************************************************
 *                                       私有函数
 **************************************************************************************************/
uint8 halSendKeys_Msg( uint8 keys, uint8 state );
void myHalKeyConfig (void);
bool isNewKey( uint8 keys );

#if ( defined KEY_NUM) && (KEY_NUM > 0 )
  static uint8 halGetNomalKeyInput(uint8 port, uint8 pin);
#endif
  
#if ( JOY_KEY_CONFIGURED == TRUE )
  static uint8 halGetJoyKeyInput(uint8 adChannel);
#endif
#if ( KEY_INPUT_INT_MODE == TRUE ) 
  bool isKeyInt( uint8 port );
#endif

/**************************************************************************************************
 * @fn      HalKeyInit
 *
 * @brief   按键初始化。
 *          这是按键初始化第一阶段，其调用链为：main()->HalDriverInit()->HalKeyInit()，即在驱动初始化时
 *          被调用。
 *          本函数执行后，按键可以工作在查询模式。用户可以通过调用HalKeyRead()获取按键状态，这样就可以满足
 *          某些任务在初始化时需要获取用户按键动作的需要。
 *          第二阶段的初始化工作由函数myHalKeyConfig()完成。
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalKeyInit( void )
{
  uint8 i;
/****  如果定义了KEY_NUM, 则初始化普通按键  ****/ 
#if (defined KEY_NUM) && (KEY_NUM > 0)
  
  for( i = 0; i < KEY_NUM; i++)
  {
    switch (keyConfig[i].port)
    {
    case MY_PORT0:
      P0SEL &= ~keyConfig[i].pin; 
      P0DIR &= ~keyConfig[i].pin;
      break;
    case MY_PORT1:
      P1SEL &= ~keyConfig[i].pin; 
      P1DIR &= ~keyConfig[i].pin;     
      break;
    case MY_PORT2:
      P2SEL &= ~keyConfig[i].pin; 
      P2DIR &= ~keyConfig[i].pin;  
      break;
    }    
  }
#endif  
  
/* 如果定义了JOY_KEY_CONFIGURED，则初始化 joy stick。joy stick 采用AD输入 */ 
#if (JOY_KEY_CONFIGURED == TRUE)
  
      P0SEL |= BV(JOY_AD_CHANNEL);   // 设置为外设引脚，AD输入通道在P0
      P0INP |= BV(JOY_AD_CHANNEL);   // 设置为三态模式，即没有上/下拉电阻  
      
#endif

}

/**************************************************************************************************
 * @fn      HalKeyConfig
 *
 * @brief   按键配置。
 *          此函数一是为了与原样板工程保持兼容，二是将真正的按键配置工作，也就是第二阶段初始化工作推迟
 *          3s。其调用链为：main()->InitBoard( OB_READY )->HalKeyConfig()。
 *          在ZStack原系统中，此函数是为了完成key的第二阶段初始化，即：如果按键采用中断输入模式，则初始化
 *          中断控制器，否则，启动按键的周期性查询工作。
 *          我们的驱动对此函数进行了改造：将第二阶段的初始化工作推迟3s进行，以避免系统误将系统初始化时的按
 *          按键动作传给用户任务处理。
 *          第二阶段的初始化工作由函数myHalKeyConfig()完成。
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalKeyConfig (bool interruptEnable, halKeyCBack_t cback)
{
  (void)(cback);            // 在我们的配置代码中，不使用这两个参数，
  (void)(interruptEnable);  // 保留这两个参数只是为了与原系统兼容。本驱动不再采用原系统的按键回调机制
  osal_start_timerEx( Hal_TaskID, HAL_KEY_EVENT,3000);  //推迟3s触发hal任务的HAL_KEY_EVENT事件
  return ;
}

/**************************************************************************************************
 * @fn      myHalKeyConfig
 *
 * @brief   按键配置，即进行按键的第二阶段初始化。
 *          如果按键采用中断输入模式，则初始化其中断控制器，否则，启动按键的周期性查询工作。
 *          此函数是按键驱动初始化的第二阶段。其调用链为：
 *                HAL_KEY_EVENT事件发生->Hal_ProcessEvent()->HalKeyPoll()->myHalKeyConfig()
 *          按键的第一阶段初始化由HalKeyInit()完成。
 *   
 * @param   None
 *
 * @return  keys - current keys status
 **************************************************************************************************/
void myHalKeyConfig (void)
{

#if ( KEY_INPUT_INT_MODE == TRUE )
  /**** 如果采用中断输入模式，则需要初始化其中断控制器 *****/
  uint8 i;
  uint8 temp;
  for( i = 0; i < KEY_NUM; i++ )
  {//扫描用户的按键配置数组
    switch (keyConfig[i].port)
    {
    case MY_PORT0:
      temp = PICTL;
      PICTL = ( temp & ~BV(0) ) | ( EDGE_TRIGGER_MODE << 0 );  // 设置边沿触发方式：上升沿或下降沿  
      P0IFG &= ( ~keyConfig[i].pin ); // 清除残留的中断标志
      P0IEN |= keyConfig[i].pin; // 开放相应引脚中断
      IEN1 |= BV(5); // 开放P0中断
      break;
    case MY_PORT1:
      if( keyConfig[i].pin < 0x10 )
      {// P1 bit0-bit3 中断触发方式
        temp = PICTL;
        PICTL = ( temp & ~BV(1) ) | ( EDGE_TRIGGER_MODE << 1);  // 设置边沿触发方式：上升沿或下降沿        
      }
      else
      {// P1 bit4-bit7 中断触发方式
        temp = PICTL;
        PICTL = ( temp & ~BV(2) ) | ( EDGE_TRIGGER_MODE << 2);  // 设置边沿触发方式：上升沿或下降沿        
      }
      
      P1IFG &= ( ~keyConfig[i].pin ); // 清除残留的中断标志
      P1IEN |= keyConfig[i].pin; // 开放相应引脚中断  
      IEN2 |= BV(4); // 开放P1中断   
      break;
    case MY_PORT2:
        temp = PICTL;
        PICTL = ( temp & ~BV(3) ) | ( EDGE_TRIGGER_MODE << 3);  // 设置边沿触发方式：上升沿或下降沿      
        P2IFG &= ( ~keyConfig[i].pin ); // 清除残留的中断标志
        P2IEN |= keyConfig[i].pin; // 开放相应引脚中断        
        IEN2 = BV(1); // 开放P2中断           
      break;
    }   
  }
#endif
  HalKeyConfigured = TRUE;  // 按键两个阶段的初始化均已完成
  return ;
}




/**************************************************************************************************
 * @fn      HalKeyRead
 *
 * @brief   读取当前按键状态。
 *          不管是按键工作在查询输入模式还是中断输入模式，应用程序都可以调用此函数读取当前按键状态。
 *          部分任务在初始化时就是调用该函数获知用户的按键操作从而决定如何初始化自己，如ZDApp任务。
 *
 * @param   None
 *
 * @return  keys - 当前按键状态
 **************************************************************************************************/
uint8 HalKeyRead ( void )
{
  uint8 keys = 0; //已按键集合
  uint8 keyState; //按键当前状态
  uint8 i;

/****  如果定义了KEY_NUM, 则获取普通按键ID  ****/   
#if (defined KEY_NUM) && (KEY_NUM > 0)
  
  for( i = 0; i < KEY_NUM; i++)
  {
    keyState = halGetNomalKeyInput( keyConfig[i].port, keyConfig[i].pin );  // 读取普通按键状态

    if( !( keyState ^ keyConfig[i].polarity ) ) // 按键状态与极性异或后取反，其结果为真，则键按下，否则没有按下。
    {
      keys |= keyConfig[i].keyID; // 将按键状态转换为按键编码 
    }
  }
  
#endif
  
/* 如果定义了JOY_KEY_CONFIGURED，则获取joy stick ID */ 
#if (JOY_KEY_CONFIGURED == TRUE)
  
  keys |= halGetJoyKeyInput(JOY_AD_CHANNEL);
  
#endif 

  return keys;
}


/**************************************************************************************************
 * @fn      HalKeyPoll
 *
 * @brief   查询按键状态，如果有按键动作，则将按键包装成消息发送给上层用户任务进行处理。另外，本函数
 *          还承担按键的第二阶段初始化工作，即如果使用中断输入模式，则调用myHalKeyConfig()初始化中断
 *          控制器。
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalKeyPoll (void)
{
  uint8 keys = 0;   //当前按键
  bool shiftKeyState = FALSE ; // 记录shift key状态，TRUE:按下，FALSE: 未按下
  
  if( !HalKeyConfigured ) //如果还没有初始化完
  {
    myHalKeyConfig(); // 进行按键的第二阶段初始化
    return;
  }
  keys = HalKeyRead(); // 读取按键状态

/*** 如果配置了 shift key， 则检测 shift key是否按下 ***/
  
  
#if ( KEY_INPUT_INT_MODE == TRUE )
  if( keys )  //如果采用中断输入模式，则直接将按键封装成消息发送
#else    
 if( isNewKey(keys) )   //如果采用查询工作查式，进一步判断是否为新的按键
#endif
 { // 如果是新的按键则发送给App
#if (SHIF_KEY_CONFIGURED == TRUE)
  
  shiftKeyState = ( keys & MY_SHIFT_KEY_ID ) ? TRUE:FALSE;   // 判断shift key状态，TRUE:按下，FALSE: 未按下
  keys = keys & (~MY_SHIFT_KEY_ID);         // 清除按键表中的shift key对应的位
  
#endif   
   OnBoard_SendKeys( keys, shiftKeyState );  //向应用App发送按键消息
 }
 
  return ;
}


/**************************************************************************************************
 * @fn      isNewKey
 *
 * @brief  本函数判断是否为新的按键。
 *        当用户按住一个键的时间<0.8s，则认为是一次按键，超过0.8s，则认为是需要自动重复按键
 * @param   keys，当前按键状态
 *
 * @return  TRUE or FALSE
 **************************************************************************************************/
#if ( KEY_INPUT_INT_MODE == FALSE ) 
bool isNewKey( uint8 keys )
{
  static uint8 sameKeyCount = 0;   // 相同按键计数
  static uint8 preKey = 0;  // 上一次按键  
  if( keys == 0 )
  { // 没有按按键动作
    preKey = 0;
    sameKeyCount = 0;
    return FALSE; 
  }
  if( keys != preKey )
  { // 如果有按键动作，且与前一次按下的键不相同，则判定为新的按键
    preKey = keys;
    sameKeyCount = 0;
    return TRUE;
  }
  if( (keys == preKey) && (sameKeyCount < KEY_REPETITION_INTERVAL) )
  { // 如果当前按键与前一次相同，且间隔时间 < KEY_REPETITION_INTERVAL*100 mS,则认为是同一次按键，不发送给App
    sameKeyCount++;
    return FALSE;
  }
  if((keys == preKey) && (sameKeyCount >= KEY_REPETITION_INTERVAL) )
  {// 如果当前按键与前一次相同，且间隔时间 > KEY_REPETITION_INTERVAL*100mS, 则认为用户是要自动重复按键，发送给App
    return TRUE;    
  }
  return FALSE;
}
#endif

/**************************************************************************************************
 * @fn      halGetNomalKeyInput
 *
 * @brief  读取普通按键输入状态。普通按键是指以高低电平为输入的按键。非普通按键是指以模拟信号为输入的按键
 *        当用户按住一个键的时间<0.8s，则认为是一次按键，超过0.8s，则认为是需要自动重复按键
 * @param   port：按键所连接的端口
*           pin: 按键所连接的引脚
 *
 * @return  keyState： 按键状态，即哪个引脚有按键动作
 **************************************************************************************************/ 
#if (defined KEY_NUM) && (KEY_NUM > 0)
uint8 halGetNomalKeyInput(uint8 port, uint8 pin)
{
    uint8 keyState = 0;
    switch (port)
    {
    case MY_PORT0:
      keyState =  ( P0 & pin ); // 读取P0口按键状态      
      break;
    case MY_PORT1:
      keyState =  ( P1 & pin ); // 读取P1口按键状态          
      break;
    case MY_PORT2:
      keyState =  ( P2 & pin ); // 读取P2口按键状态       
      break;    
    }  
  return keyState;
}
#endif



/**************************************************************************************************
 * @fn      halGetJoyKeyInput
 *
 * @brief   读取joy stick 按键输入状态。joy stick采用模拟信号输入。
 *
 * @param   adChannel：joy stick所接入的模拟通道号
 *
 * @return  keyID - 用户按键所对应的编码
 **************************************************************************************************/
#if (JOY_KEY_CONFIGURED == TRUE)
uint8 halGetJoyKeyInput(uint8 adChannel)
{

  uint8 keyID = 0;   
  uint8 adc;

  uint8 preKeyID; 
  uint8 i;

  /* 连续读取两次按键状态，如果两次均相同，则认为是有效按键动作 */
  do
  {
    preKeyID = keyID;    /* save previouse key reading */

    adc = HalAdcRead ( adChannel, HAL_ADC_RESOLUTION_8 );

    for( i = 0; i<5; i++ )
    {
      if( (adc >= joyStickADRange[i].lowerLimit) && (adc <= joyStickADRange[i].upperLimit) )
      {
        keyID |= joyStickADRange[i].keyID;  // 将模拟输入值映射为按键ID
      }    
    }  
  } while (keyID != preKeyID);

  return keyID;
}
#endif


#if ( P0_INT_ENABLE == TRUE )
/**************************************************************************************************
 * @fn      halKeyPort0Isr
 *
 * @brief   Port0 ISR
 *
 * @param
 *
 * @return
 **************************************************************************************************/
HAL_ISR_FUNCTION( halKeyPort0Isr, P0INT_VECTOR )
{

  HAL_ENTER_ISR();

  if( isKeyInt( MY_PORT0) ) // 如果是P0的按键中断
  { 
    osal_start_timerEx (Hal_TaskID, HAL_KEY_EVENT, KEY_REPETITION_INTERVAL); //延迟读取按键，即消抖
  }
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}  
#endif

#if ( P1_INT_ENABLE == TRUE )
/**************************************************************************************************
 * @fn      halKeyPort1Isr
 *
 * @brief   Port1 ISR
 *
 * @param
 *
 * @return
 **************************************************************************************************/      
HAL_ISR_FUNCTION( halKeyPort1Isr, P1INT_VECTOR )
{

  HAL_ENTER_ISR();

  if( isKeyInt(MY_PORT1) ) // 如果是P1的按键中断
  { 
    osal_start_timerEx ( Hal_TaskID, HAL_KEY_EVENT, KEY_REPETITION_INTERVAL ); //延迟读取按键，即消抖
  }
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}  
#endif


#if ( P2_INT_ENABLE == TRUE )
/**************************************************************************************************
 * @fn      halKeyPort2Isr
 *
 * @brief   Port2 ISR
 *
 * @param
 *
 * @return
 **************************************************************************************************/      
HAL_ISR_FUNCTION( halKeyPort2Isr, P2INT_VECTOR )
{

  HAL_ENTER_ISR();

  if( isKeyInt(MY_PORT2) ) // 如果是P2的按键中断
  { 
    osal_start_timerEx (Hal_TaskID, HAL_KEY_EVENT, KEY_REPETITION_INTERVAL); //延迟读取按键，即消抖
  }
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}   
#endif
  


#if ( KEY_INPUT_INT_MODE == TRUE )

/**************************************************************************************************
 * @fn      isKeyInt
 *
 * @brief   判断中断源是否为按键，如果是则同时清除相应标志位
 *
 * @param   None
 *
 * @return  isKeyInt - 如果按键产生的中断，则返回TRUE, 否则为FALSE 
 **************************************************************************************************/
bool isKeyInt( uint8 port )
{
  uint8 i;
  bool isKeyInt = FALSE;
 for( i = 0; i < KEY_NUM; i++ )
  {
    if( port == keyConfig[i].port )
    {
      switch (keyConfig[i].port)
      {
      case MY_PORT0:
        isKeyInt |= ( P0IFG & keyConfig[i].pin ) ? TRUE: FALSE;
        if( isKeyInt )
        {
          P0IFG &= ~keyConfig[i].pin; // 清除P0端口对应引脚的中断标志
          P0IF = 0; // 清除P0的总中断标志
        }
        break;
      case MY_PORT1:
        isKeyInt |= ( P1IFG & keyConfig[i].pin ) ? TRUE: FALSE;
        if( isKeyInt )
        {
          P1IFG &= ~keyConfig[i].pin; // 清除P0端口对应引脚的中断标志
          P1IF = 0; // 清除P0的总中断标志
        }        
        break;   
      case MY_PORT2:
        isKeyInt |= ( P2IFG & keyConfig[i].pin ) ? TRUE: FALSE;
        if( isKeyInt )
        {
          P2IFG &= ~keyConfig[i].pin; // 清除P0端口对应引脚的中断标志
          P2IF = 0; // 清除P0的总中断标志
        }         
        break;
      }     
    }
  }

 return isKeyInt;

}

/**************************************************************************************************
 * @fn      HalKeyEnterSleep
 *
 * @brief  - Get called to enter sleep mode
 *
 * @param
 *
 * @return
 **************************************************************************************************/
void HalKeyEnterSleep ( void )
{
}

/**************************************************************************************************
 * @fn      HalKeyExitSleep
 *
 * @brief   - Get called when sleep is over
 *
 * @param
 *
 * @return  - return saved keys
 **************************************************************************************************/
uint8 HalKeyExitSleep ( void )
{
  /* Wake up and read keys */
  return ( HalKeyRead () );
}

#endif


#else


void HalKeyInit(void){}
void HalKeyConfig(bool interruptEnable, halKeyCBack_t cback){}
uint8 HalKeyRead(void){ return 0;}
void HalKeyPoll(void){}

#endif /* HAL_KEY */
