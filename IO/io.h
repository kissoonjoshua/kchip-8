#pragma once
#include <cstdint>

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

  void delay(uint32_t delay) { SDL_Delay(delay); }

  void input_handler() {
    emuState->prevKeys = emuState->keys;

    while(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT: emuState->status = Status::STOPPED; return;
        case SDL_KEYDOWN:
          switch(e.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE: emuState->status = Status::STOPPED; return;
            case SDL_SCANCODE_SPACE: 
              if(emuState->status == Status::RUNNING) emuState->status = Status::PAUSED; 
              else emuState->status = Status::RUNNING; 
              return;
            case SDL_SCANCODE_BACKSPACE: emuState->status = Status::RESET; return; 
            case SDL_SCANCODE_1: emuState->keys |= 1 << 0x1; break;
            case SDL_SCANCODE_2: emuState->keys |= 1 << 0x2; break;
            case SDL_SCANCODE_3: emuState->keys |= 1 << 0x3; break;
            case SDL_SCANCODE_4: emuState->keys |= 1 << 0xC; break;
            case SDL_SCANCODE_Q: emuState->keys |= 1 << 0x4; break;
            case SDL_SCANCODE_W: emuState->keys |= 1 << 0x5; break;
            case SDL_SCANCODE_E: emuState->keys |= 1 << 0x6; break;
            case SDL_SCANCODE_R: emuState->keys |= 1 << 0xD; break;
            case SDL_SCANCODE_A: emuState->keys |= 1 << 0x7; break;
            case SDL_SCANCODE_S: emuState->keys |= 1 << 0x8; break;
            case SDL_SCANCODE_D: emuState->keys |= 1 << 0x9; break;
            case SDL_SCANCODE_F: emuState->keys |= 1 << 0xE; break;
            case SDL_SCANCODE_Z: emuState->keys |= 1 << 0xA; break;
            case SDL_SCANCODE_X: emuState->keys |= 1 << 0x0; break;
            case SDL_SCANCODE_C: emuState->keys |= 1 << 0xB; break;
            case SDL_SCANCODE_V: emuState->keys |= 1 << 0xF; break;
            default: break;
          };
          break;
        case SDL_KEYUP:
          switch(e.key.keysym.scancode) {
            case SDL_SCANCODE_1: emuState->keys &= ~(1 << 0x1); break;
            case SDL_SCANCODE_2: emuState->keys &= ~(1 << 0x2); break;
            case SDL_SCANCODE_3: emuState->keys &= ~(1 << 0x3); break;
            case SDL_SCANCODE_4: emuState->keys &= ~(1 << 0xC); break;
            case SDL_SCANCODE_Q: emuState->keys &= ~(1 << 0x4); break;
            case SDL_SCANCODE_W: emuState->keys &= ~(1 << 0x5); break;
            case SDL_SCANCODE_E: emuState->keys &= ~(1 << 0x6); break;
            case SDL_SCANCODE_R: emuState->keys &= ~(1 << 0xD); break;
            case SDL_SCANCODE_A: emuState->keys &= ~(1 << 0x7); break;
            case SDL_SCANCODE_S: emuState->keys &= ~(1 << 0x8); break;
            case SDL_SCANCODE_D: emuState->keys &= ~(1 << 0x9); break;
            case SDL_SCANCODE_F: emuState->keys &= ~(1 << 0xE); break;
            case SDL_SCANCODE_Z: emuState->keys &= ~(1 << 0xA); break;
            case SDL_SCANCODE_X: emuState->keys &= ~(1 << 0x0); break;
            case SDL_SCANCODE_C: emuState->keys &= ~(1 << 0xB); break;
            case SDL_SCANCODE_V: emuState->keys &= ~(1 << 0xF); break;
            default: break;
          };
          break;
        default: break;
      }
    }
  }

  void toggle_audio(bool on) {
    SDL_PauseAudioDevice(device, !on);
  }

private:
  static void audio_callback(void *userdata, uint8_t *stream, int len) {
    static uint32_t sample_index = 0;
    Config *cfg = reinterpret_cast<Config*>(userdata);
    int16_t *audioStream = reinterpret_cast<int16_t*>(stream);
    int32_t wavePeriod = cfg->sampleRate / cfg->waveFreq;
    int32_t halfWavePeriod = wavePeriod / 2;

    for(int i = 0; i < len / 2; i++, sample_index++) {
      audioStream[i] = (sample_index / halfWavePeriod % 2) ? cfg->volume : -cfg->volume;
    }
  }

  State *emuState;
  Config *cfg;
  SDL_Event e;
  SDL_AudioSpec want, have;
  SDL_AudioDeviceID device;
};
