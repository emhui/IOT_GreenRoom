#include "hal_key.h"
#include "my_hal_key_config.h"
#include "hal_adc.h"
#include "OnBoard.h"
#include "hal_drivers.h"

//#define NO_TASK_ID 0xFF

#if (defined HAL_KEY) && (HAL_KEY == TRUE)

bool Hal_KeyIntEnable = KEY_INPUT_INT_MODE;   
static bool HalKeyConfigured = FALSE;  // �����Ƿ��ʼ�����

/**************************************************************************************************
 *                                       ˽�к���
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
 * @brief   ������ʼ����
 *          ���ǰ�����ʼ����һ�׶Σ��������Ϊ��main()->HalDriverInit()->HalKeyInit()������������ʼ��ʱ
 *          �����á�
 *          ������ִ�к󣬰������Թ����ڲ�ѯģʽ���û�����ͨ������HalKeyRead()��ȡ����״̬�������Ϳ�������
 *          ĳЩ�����ڳ�ʼ��ʱ��Ҫ��ȡ�û�������������Ҫ��
 *          �ڶ��׶εĳ�ʼ�������ɺ���myHalKeyConfig()��ɡ�
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalKeyInit( void )
{
  uint8 i;
/****  ���������KEY_NUM, ���ʼ����ͨ����  ****/ 
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
  
/* ���������JOY_KEY_CONFIGURED�����ʼ�� joy stick��joy stick ����AD���� */ 
#if (JOY_KEY_CONFIGURED == TRUE)
  
      P0SEL |= BV(JOY_AD_CHANNEL);   // ����Ϊ�������ţ�AD����ͨ����P0
      P0INP |= BV(JOY_AD_CHANNEL);   // ����Ϊ��̬ģʽ����û����/��������  
      
#endif

}

/**************************************************************************************************
 * @fn      HalKeyConfig
 *
 * @brief   �������á�
 *          �˺���һ��Ϊ����ԭ���幤�̱��ּ��ݣ����ǽ������İ������ù�����Ҳ���ǵڶ��׶γ�ʼ�������Ƴ�
 *          3s���������Ϊ��main()->InitBoard( OB_READY )->HalKeyConfig()��
 *          ��ZStackԭϵͳ�У��˺�����Ϊ�����key�ĵڶ��׶γ�ʼ��������������������ж�����ģʽ�����ʼ��
 *          �жϿ��������������������������Բ�ѯ������
 *          ���ǵ������Դ˺��������˸��죺���ڶ��׶εĳ�ʼ�������Ƴ�3s���У��Ա���ϵͳ��ϵͳ��ʼ��ʱ�İ�
 *          �������������û�������
 *          �ڶ��׶εĳ�ʼ�������ɺ���myHalKeyConfig()��ɡ�
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalKeyConfig (bool interruptEnable, halKeyCBack_t cback)
{
  (void)(cback);            // �����ǵ����ô����У���ʹ��������������
  (void)(interruptEnable);  // ��������������ֻ��Ϊ����ԭϵͳ���ݡ����������ٲ���ԭϵͳ�İ����ص�����
  osal_start_timerEx( Hal_TaskID, HAL_KEY_EVENT,3000);  //�Ƴ�3s����hal�����HAL_KEY_EVENT�¼�
  return ;
}

/**************************************************************************************************
 * @fn      myHalKeyConfig
 *
 * @brief   �������ã������а����ĵڶ��׶γ�ʼ����
 *          ������������ж�����ģʽ�����ʼ�����жϿ��������������������������Բ�ѯ������
 *          �˺����ǰ���������ʼ���ĵڶ��׶Ρ��������Ϊ��
 *                HAL_KEY_EVENT�¼�����->Hal_ProcessEvent()->HalKeyPoll()->myHalKeyConfig()
 *          �����ĵ�һ�׶γ�ʼ����HalKeyInit()��ɡ�
 *   
 * @param   None
 *
 * @return  keys - current keys status
 **************************************************************************************************/
void myHalKeyConfig (void)
{

#if ( KEY_INPUT_INT_MODE == TRUE )
  /**** ��������ж�����ģʽ������Ҫ��ʼ�����жϿ����� *****/
  uint8 i;
  uint8 temp;
  for( i = 0; i < KEY_NUM; i++ )
  {//ɨ���û��İ�����������
    switch (keyConfig[i].port)
    {
    case MY_PORT0:
      temp = PICTL;
      PICTL = ( temp & ~BV(0) ) | ( EDGE_TRIGGER_MODE << 0 );  // ���ñ��ش�����ʽ�������ػ��½���  
      P0IFG &= ( ~keyConfig[i].pin ); // ����������жϱ�־
      P0IEN |= keyConfig[i].pin; // ������Ӧ�����ж�
      IEN1 |= BV(5); // ����P0�ж�
      break;
    case MY_PORT1:
      if( keyConfig[i].pin < 0x10 )
      {// P1 bit0-bit3 �жϴ�����ʽ
        temp = PICTL;
        PICTL = ( temp & ~BV(1) ) | ( EDGE_TRIGGER_MODE << 1);  // ���ñ��ش�����ʽ�������ػ��½���        
      }
      else
      {// P1 bit4-bit7 �жϴ�����ʽ
        temp = PICTL;
        PICTL = ( temp & ~BV(2) ) | ( EDGE_TRIGGER_MODE << 2);  // ���ñ��ش�����ʽ�������ػ��½���        
      }
      
      P1IFG &= ( ~keyConfig[i].pin ); // ����������жϱ�־
      P1IEN |= keyConfig[i].pin; // ������Ӧ�����ж�  
      IEN2 |= BV(4); // ����P1�ж�   
      break;
    case MY_PORT2:
        temp = PICTL;
        PICTL = ( temp & ~BV(3) ) | ( EDGE_TRIGGER_MODE << 3);  // ���ñ��ش�����ʽ�������ػ��½���      
        P2IFG &= ( ~keyConfig[i].pin ); // ����������жϱ�־
        P2IEN |= keyConfig[i].pin; // ������Ӧ�����ж�        
        IEN2 = BV(1); // ����P2�ж�           
      break;
    }   
  }
#endif
  HalKeyConfigured = TRUE;  // ���������׶εĳ�ʼ���������
  return ;
}




/**************************************************************************************************
 * @fn      HalKeyRead
 *
 * @brief   ��ȡ��ǰ����״̬��
 *          �����ǰ��������ڲ�ѯ����ģʽ�����ж�����ģʽ��Ӧ�ó��򶼿��Ե��ô˺�����ȡ��ǰ����״̬��
 *          ���������ڳ�ʼ��ʱ���ǵ��øú�����֪�û��İ��������Ӷ�������γ�ʼ���Լ�����ZDApp����
 *
 * @param   None
 *
 * @return  keys - ��ǰ����״̬
 **************************************************************************************************/
uint8 HalKeyRead ( void )
{
  uint8 keys = 0; //�Ѱ�������
  uint8 keyState; //������ǰ״̬
  uint8 i;

/****  ���������KEY_NUM, ���ȡ��ͨ����ID  ****/   
#if (defined KEY_NUM) && (KEY_NUM > 0)
  
  for( i = 0; i < KEY_NUM; i++)
  {
    keyState = halGetNomalKeyInput( keyConfig[i].port, keyConfig[i].pin );  // ��ȡ��ͨ����״̬

    if( !( keyState ^ keyConfig[i].polarity ) ) // ����״̬�뼫������ȡ��������Ϊ�棬������£�����û�а��¡�
    {
      keys |= keyConfig[i].keyID; // ������״̬ת��Ϊ�������� 
    }
  }
  
#endif
  
/* ���������JOY_KEY_CONFIGURED�����ȡjoy stick ID */ 
#if (JOY_KEY_CONFIGURED == TRUE)
  
  keys |= halGetJoyKeyInput(JOY_AD_CHANNEL);
  
#endif 

  return keys;
}


/**************************************************************************************************
 * @fn      HalKeyPoll
 *
 * @brief   ��ѯ����״̬������а����������򽫰�����װ����Ϣ���͸��ϲ��û�������д������⣬������
 *          ���е������ĵڶ��׶γ�ʼ�������������ʹ���ж�����ģʽ�������myHalKeyConfig()��ʼ���ж�
 *          ��������
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalKeyPoll (void)
{
  uint8 keys = 0;   //��ǰ����
  bool shiftKeyState = FALSE ; // ��¼shift key״̬��TRUE:���£�FALSE: δ����
  
  if( !HalKeyConfigured ) //�����û�г�ʼ����
  {
    myHalKeyConfig(); // ���а����ĵڶ��׶γ�ʼ��
    return;
  }
  keys = HalKeyRead(); // ��ȡ����״̬

/*** ��������� shift key�� ���� shift key�Ƿ��� ***/
  
  
#if ( KEY_INPUT_INT_MODE == TRUE )
  if( keys )  //��������ж�����ģʽ����ֱ�ӽ�������װ����Ϣ����
#else    
 if( isNewKey(keys) )   //������ò�ѯ������ʽ����һ���ж��Ƿ�Ϊ�µİ���
#endif
 { // ������µİ������͸�App
#if (SHIF_KEY_CONFIGURED == TRUE)
  
  shiftKeyState = ( keys & MY_SHIFT_KEY_ID ) ? TRUE:FALSE;   // �ж�shift key״̬��TRUE:���£�FALSE: δ����
  keys = keys & (~MY_SHIFT_KEY_ID);         // ����������е�shift key��Ӧ��λ
  
#endif   
   OnBoard_SendKeys( keys, shiftKeyState );  //��Ӧ��App���Ͱ�����Ϣ
 }
 
  return ;
}


/**************************************************************************************************
 * @fn      isNewKey
 *
 * @brief  �������ж��Ƿ�Ϊ�µİ�����
 *        ���û���סһ������ʱ��<0.8s������Ϊ��һ�ΰ���������0.8s������Ϊ����Ҫ�Զ��ظ�����
 * @param   keys����ǰ����״̬
 *
 * @return  TRUE or FALSE
 **************************************************************************************************/
#if ( KEY_INPUT_INT_MODE == FALSE ) 
bool isNewKey( uint8 keys )
{
  static uint8 sameKeyCount = 0;   // ��ͬ��������
  static uint8 preKey = 0;  // ��һ�ΰ���  
  if( keys == 0 )
  { // û�а���������
    preKey = 0;
    sameKeyCount = 0;
    return FALSE; 
  }
  if( keys != preKey )
  { // ����а�������������ǰһ�ΰ��µļ�����ͬ�����ж�Ϊ�µİ���
    preKey = keys;
    sameKeyCount = 0;
    return TRUE;
  }
  if( (keys == preKey) && (sameKeyCount < KEY_REPETITION_INTERVAL) )
  { // �����ǰ������ǰһ����ͬ���Ҽ��ʱ�� < KEY_REPETITION_INTERVAL*100 mS,����Ϊ��ͬһ�ΰ����������͸�App
    sameKeyCount++;
    return FALSE;
  }
  if((keys == preKey) && (sameKeyCount >= KEY_REPETITION_INTERVAL) )
  {// �����ǰ������ǰһ����ͬ���Ҽ��ʱ�� > KEY_REPETITION_INTERVAL*100mS, ����Ϊ�û���Ҫ�Զ��ظ����������͸�App
    return TRUE;    
  }
  return FALSE;
}
#endif

/**************************************************************************************************
 * @fn      halGetNomalKeyInput
 *
 * @brief  ��ȡ��ͨ��������״̬����ͨ������ָ�Ըߵ͵�ƽΪ����İ���������ͨ������ָ��ģ���ź�Ϊ����İ���
 *        ���û���סһ������ʱ��<0.8s������Ϊ��һ�ΰ���������0.8s������Ϊ����Ҫ�Զ��ظ�����
 * @param   port�����������ӵĶ˿�
*           pin: ���������ӵ�����
 *
 * @return  keyState�� ����״̬�����ĸ������а�������
 **************************************************************************************************/ 
#if (defined KEY_NUM) && (KEY_NUM > 0)
uint8 halGetNomalKeyInput(uint8 port, uint8 pin)
{
    uint8 keyState = 0;
    switch (port)
    {
    case MY_PORT0:
      keyState =  ( P0 & pin ); // ��ȡP0�ڰ���״̬      
      break;
    case MY_PORT1:
      keyState =  ( P1 & pin ); // ��ȡP1�ڰ���״̬          
      break;
    case MY_PORT2:
      keyState =  ( P2 & pin ); // ��ȡP2�ڰ���״̬       
      break;    
    }  
  return keyState;
}
#endif



/**************************************************************************************************
 * @fn      halGetJoyKeyInput
 *
 * @brief   ��ȡjoy stick ��������״̬��joy stick����ģ���ź����롣
 *
 * @param   adChannel��joy stick�������ģ��ͨ����
 *
 * @return  keyID - �û���������Ӧ�ı���
 **************************************************************************************************/
#if (JOY_KEY_CONFIGURED == TRUE)
uint8 halGetJoyKeyInput(uint8 adChannel)
{

  uint8 keyID = 0;   
  uint8 adc;

  uint8 preKeyID; 
  uint8 i;

  /* ������ȡ���ΰ���״̬��������ξ���ͬ������Ϊ����Ч�������� */
  do
  {
    preKeyID = keyID;    /* save previouse key reading */

    adc = HalAdcRead ( adChannel, HAL_ADC_RESOLUTION_8 );

    for( i = 0; i<5; i++ )
    {
      if( (adc >= joyStickADRange[i].lowerLimit) && (adc <= joyStickADRange[i].upperLimit) )
      {
        keyID |= joyStickADRange[i].keyID;  // ��ģ������ֵӳ��Ϊ����ID
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

  if( isKeyInt( MY_PORT0) ) // �����P0�İ����ж�
  { 
    osal_start_timerEx (Hal_TaskID, HAL_KEY_EVENT, KEY_REPETITION_INTERVAL); //�ӳٶ�ȡ������������
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

  if( isKeyInt(MY_PORT1) ) // �����P1�İ����ж�
  { 
    osal_start_timerEx ( Hal_TaskID, HAL_KEY_EVENT, KEY_REPETITION_INTERVAL ); //�ӳٶ�ȡ������������
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

  if( isKeyInt(MY_PORT2) ) // �����P2�İ����ж�
  { 
    osal_start_timerEx (Hal_TaskID, HAL_KEY_EVENT, KEY_REPETITION_INTERVAL); //�ӳٶ�ȡ������������
  }
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}   
#endif
  


#if ( KEY_INPUT_INT_MODE == TRUE )

/**************************************************************************************************
 * @fn      isKeyInt
 *
 * @brief   �ж��ж�Դ�Ƿ�Ϊ�������������ͬʱ�����Ӧ��־λ
 *
 * @param   None
 *
 * @return  isKeyInt - ��������������жϣ��򷵻�TRUE, ����ΪFALSE 
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
          P0IFG &= ~keyConfig[i].pin; // ���P0�˿ڶ�Ӧ���ŵ��жϱ�־
          P0IF = 0; // ���P0�����жϱ�־
        }
        break;
      case MY_PORT1:
        isKeyInt |= ( P1IFG & keyConfig[i].pin ) ? TRUE: FALSE;
        if( isKeyInt )
        {
          P1IFG &= ~keyConfig[i].pin; // ���P0�˿ڶ�Ӧ���ŵ��жϱ�־
          P1IF = 0; // ���P0�����жϱ�־
        }        
        break;   
      case MY_PORT2:
        isKeyInt |= ( P2IFG & keyConfig[i].pin ) ? TRUE: FALSE;
        if( isKeyInt )
        {
          P2IFG &= ~keyConfig[i].pin; // ���P0�˿ڶ�Ӧ���ŵ��жϱ�־
          P2IF = 0; // ���P0�����жϱ�־
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
