#pragma once
#include <cstdint>

#include "SDL.h"
#include "kchip-8_common.h"

class SDL {
public:
  SDL(State &state, Config &config): emuState{&state}, cfg{&config} {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
      fprintf(stderr, "SDL failed to initialize. SDL_Error: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    } 
    
    window = SDL_CreateWindow("kchip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
              cfg->display_width * cfg->scale, cfg->display_height * cfg->scale, SDL_WINDOW_SHOWN);
    if(!window) {
      fprintf(stderr, "Could not create SDL window. SDL_Error: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    } 
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
      fprintf(stderr, "Could not create SDL renderer. SDL_Error: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }

    want.freq = cfg->sample_rate;
    want.format = AUDIO_S16LSB;
    want.channels = 1;
    want.samples = 4096;
    want.callback = audio_callback;
    want.userdata = this->cfg;
    device = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if(!device || want.format != have.format || want.channels != have.channels) {
      fprintf(stderr, "Could not create SDL audio device. SDL_Error: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }

    clear();
  }

  ~SDL() {
    SDL_CloseAudioDevice(device);
    SDL_DestroyRenderer(renderer); renderer = nullptr;
    SDL_DestroyWindow(window); window = nullptr;
    SDL_Quit();
  }

  void clear() {
    const uint8_t r = static_cast<uint8_t>(cfg->background >> 24);
    const uint8_t g = static_cast<uint8_t>(cfg->background >> 16);
    const uint8_t b = static_cast<uint8_t>(cfg->background >> 8);
    const uint8_t a = static_cast<uint8_t>(cfg->background);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer); 
  }

  void redraw(uint8_t const *display) { 
    SDL_Rect pixel = { .x = 0, .y = 0, .w = cfg->scale, .h = cfg->scale };
    const uint8_t bg_r = static_cast<uint8_t>(cfg->background >> 24);
    const uint8_t bg_g = static_cast<uint8_t>(cfg->background >> 16);
    const uint8_t bg_b = static_cast<uint8_t>(cfg->background >> 8);
    const uint8_t bg_a = static_cast<uint8_t>(cfg->background);
    const uint8_t fg_r = static_cast<uint8_t>(cfg->foreground >> 24);
    const uint8_t fg_g = static_cast<uint8_t>(cfg->foreground >> 16);
    const uint8_t fg_b = static_cast<uint8_t>(cfg->foreground >> 8);
    const uint8_t fg_a = static_cast<uint8_t>(cfg->foreground);

    for(uint16_t p = 0; p < cfg->display_height * cfg->display_width; p++) {
      pixel.x = p % cfg->display_width * cfg->scale;
      pixel.y = p / cfg->display_width * cfg->scale;
      uint8_t bytePos = p / 8, bitPos = 7 - p % 8;

      if(display[bytePos] >> bitPos & 1) {
        SDL_SetRenderDrawColor(renderer, fg_r, fg_g, fg_b, fg_a);
        SDL_RenderFillRect(renderer, &pixel);
        if(cfg->outline) {
          SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, bg_a);
          SDL_RenderDrawRect(renderer, &pixel);
        }
      } else {
        SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, bg_a);
        SDL_RenderFillRect(renderer, &pixel);
      }
    }

    SDL_RenderPresent(renderer);
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
    int16_t *audio_stream = reinterpret_cast<int16_t*>(stream);
    int32_t wave_period = cfg->sample_rate / cfg->wave_freq;
    int32_t half_wave_period = wave_period / 2;

    for(int i = 0; i < len / 2; i++, sample_index++) {
      audio_stream[i] = (sample_index / half_wave_period % 2) ? cfg->volume : -cfg->volume;
    }
  }

  State *emuState;
  Config *cfg;
  SDL_Event e;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_AudioSpec want, have;
  SDL_AudioDeviceID device;
};
