#include "audio.h"
#include "OSAL.h"
#include "hal_drivers.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include "iocc2530.h"

static void initAudioConfig(void);
static void palyAudioWel(void); // P0_0
static void playAudioExit(void); // P0_7
static void stopAudioWel(void);
static void stopAduioExit(void);
 
AUDIO_t Audio = {
  initAudioConfig,
  palyAudioWel,
  playAudioExit,
  stopAudioWel,
  stopAduioExit
};

static void initAudioConfig() {
  // P0_0 ���Ż�ӭ
  // p0_7 �����뿪
  
  P0SEL &= ~BV(0);
  P0DIR |= BV(0);
  P0_0 = 1;
  // P0INP |= BV(0);
  
  P0SEL &= ~BV(7);
  P0DIR |= BV(7);
  P0INP |= BV(7);
  P0_7 = 1;
}

// palyAudioWel ����"��ӭ"
static void palyAudioWel() {
  P0_0 = 0;
}

// palyAudioExit ����"�뿪"
static void playAudioExit() {
  P0_7 = 0;
}

// stopAudioWel ֹͣ"��ӭ"
static void stopAudioWel() {
  P0_0 = 1;
}

// stopAudioExit ֹͣ"�뿪"
static void stopAduioExit() {
  P0_7 = 1;
}