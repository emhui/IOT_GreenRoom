#ifndef AUDIO_H
#define AUDIO_H
#include "hal_types.h"

typedef struct{   //Led¿‡∂®“Â
  void (*initAudioConfig)(void);
  void (*palyAudioWel)(void); // P0_5
  void (*playAudioExit)(void); // P0_7
  void (*stopAudioWel)(void);
  void (*stopAduioExit)(void);
}AUDIO_t;

extern AUDIO_t Audio;
#endif