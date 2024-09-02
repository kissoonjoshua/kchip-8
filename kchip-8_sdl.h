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

    clear();
  }

  ~SDL() {
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

  void delay() { SDL_Delay(16); }

  void input_handler() {
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
            default: break;
          };
          break;
        case SDL_KEYUP: [[fallthrough]];
        default: break;
      }
    }
  }

private:
  State *emuState;
  Config *cfg;
  SDL_Event e;
  SDL_Window *window;
  SDL_Renderer *renderer;
};
