#pragma once

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "Common/common.h"

class SDL {
public:
  SDL(State *state, Config *config): emuState{state}, cfg{config} {
    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
      fprintf(stderr, "SDL failed to initialize. SDL_Error: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    } 

    want.freq = cfg->sampleRate;
    want.format = AUDIO_S16LSB;
    want.channels = 1;
    want.samples = 512;
    want.callback = audio_callback;
    want.userdata = this->cfg;
    device = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if(!device || want.format != have.format || want.channels != have.channels) {
      fprintf(stderr, "Could not create SDL audio device. SDL_Error: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }
  }

  ~SDL() {
    SDL_CloseAudioDevice(device);
    SDL_Quit();
  }

  void handle_input();
  void delay(uint32_t delay) { SDL_Delay(delay); }
  void toggle_audio(bool on);

private:
  static void audio_callback(void *userdata, uint8_t *stream, int len);

  State *emuState;
  Config *cfg;
  SDL_Event e;
  SDL_AudioSpec want, have;
  SDL_AudioDeviceID device;
};