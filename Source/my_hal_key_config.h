#ifndef MY_HAL_KEY_CONFIG_H
#define MY_HAL_KEY_CONFIG_H

#include "ioCC2530.h"
#include "hal_types.h"
#include "hal_defs.h"
#include "hal_key.h"

/**  �������ò��裬���岽  **/

/***   ��һ���� ���ð�������ģʽ����ز��� **/
    /*** 1.1���� �Ƿ�ʹ���ж�ģʽ ***/
      #define KEY_INPUT_INT_MODE     FALSE    // TRUE: �ж�ģʽ�� FALSE: ��ѯģʽ

    /*** 1.2���� ���ʹ�����жϹ��ܣ�����Ҫ��һ��������Щ�˿�ʹ�����ж��Լ�����ģʽ**/
      #if ( KEY_INPUT_INT_MODE == TRUE )
      
          #define P0_INT_ENABLE  TRUE  // PO�����˰���������Ҫ����ΪTRUE, ����ΪFALSE
          #define P1_INT_ENABLE  FALSE  // P1�����˰���������Ҫ����ΪTRUE, ����ΪFALSE
          #define P2_INT_ENABLE  TRUE  // P2�����˰���������Ҫ����ΪTRUE, ����ΪFALSE
          #define EDGE_TRIGGER_MODE FALLING_EDGE   // �жϴ�����ʽ��ȡֵ��RISSING_EDGE �� FALLING_EDGE
                                                   // �ֱ��ʾ�������ش������½��ش���
      #endif

/***   �ڶ�����������ͨ���������Ըߵ͵�ƽ������ģ���ź�Ϊ�����źŵİ��� **/
    /*** 2.1���� ��������������Ҫ��KEY_NUM+joystick <=8 �� ��������joy stick����������Ϊ5������Ϊ0***/
      #define KEY_NUM     2     // ��ͨ������������ֵ�����˰�����������keyConfig[]�ĳ��ȣ�
                           //  ���û����ͨ�������򽫴���ע�͵���������Ϊ0
    /*** 2.2���������������ͨ�������ܣ�����Ҫ�û���һ���������������ڵĶ˿ڡ����š�key id������  */
      #if (defined KEY_NUM) && (KEY_NUM > 0)
      
          // �˿� -- λ��  --  ������   --  �͵�ƽ��ͨ
          const KEY_t keyConfig[KEY_NUM] = { //ÿһ������һ��������
              MY_PORT0, BV(1),  HAL_KEY_SW_1,  MY_KEY_ACTIVE_LOW,   // ����ȡֵΪ��MY_KEY_ACTIVE_LOW��MY_KEY_ACTIVE_HIGTH
              MY_PORT2, BV(0),  HAL_KEY_SW_2,  MY_KEY_ACTIVE_LOW    // �ֱ��ʾ�͵�ƽ��Ч��ߵ�ƽ��Ч
            };
      
      #endif

/***   ������������SHIFT���� **/

  /*** 3.1���� �Ƿ�ʹ��SHIFT�������� ***/
      #define SHIF_KEY_CONFIGURED    FALSE   // �Ƿ�����shift key���ܡ�����ǣ���������ΪTRUE 

   /*** 3.2���� ���������shift key ���ܣ�����Ҫ��һ��������ʹ�õ�ID  ***/
      #if (SHIF_KEY_CONFIGURED == TRUE)

          #define MY_SHIFT_KEY_ID      MY_HAL_KEY3_ID    //**** ע�ⲻҪ������key id�ظ�������key ID�Ķ�����hal_key.h��

       #endif

/***   ���Ĳ�������joy stick ���� **/

  /*** 4.1���� �Ƿ�ʹ�� joy stick ���� ***/
      #define JOY_KEY_CONFIGURED     FALSE   // �Ƿ�����joy stick���ܡ�����ǣ���������ΪTRUE

  /** 4.2�������������joy stick���ܣ����û���Ҫ��һ��������ʹ�õ�ADͨ���͸�����ADȡֵ��Χ  ***/
      #if (JOY_KEY_CONFIGURED == TRUE)
      
          #define JOY_AD_CHANNEL 0x06   // joy stickʹ�õ�ADͨ��
      
          const JOY_STICK_AD_RANGE_t joyStickADRange[5] = {  // ����joy stick������ADȡֵ��Χ
          // ��ʽ�� joy stick key id, ����ֵ(lower limit)�� ����ֵ(upper limit)
            MY_HAL_JOY_DOWN_ID,   20,   30,
            MY_HAL_JOY_LEFT_ID,   45,   55,
            MY_HAL_JOY_CENTER_ID, 70,   80,
            MY_HAL_JOY_UP_ID,     95,   105,
            MY_HAL_JOY_RIGHT_ID,  117,  127
          };
      
      #endif 
        
/***   ���岽�����ð����Զ��ظ����ʱ�� **/       
#define KEY_REPETITION_INTERVAL  5     //  �����ظ��������λ��100ms������סһ�������ţ�
                                       // �೤ʱ���ʼ�Զ��ظ����ò������Բ�ѯģʽ��Ч.


#endif
