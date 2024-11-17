#pragma once
#include "Common/common.h"
#include "IO/io.h"

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <mutex>
#include <condition_variable>

constexpr uint16_t PC_START = 0x200;
constexpr uint16_t SP_START = 0xEA0;
constexpr uint16_t FONT_START = 0x50;
constexpr size_t FONT_SIZE = 0x50;
constexpr uint16_t DISPLAY_START = 0xF00;
constexpr uint16_t DISPLAY_SIZE = 0x100;
constexpr uint16_t MAX_MEM = 0x1000;
constexpr uint8_t FONT[] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, 
  0x20, 0x60, 0x20, 0x20, 0x70, 
  0xF0, 0x10, 0xF0, 0x80, 0xF0, 
  0xF0, 0x10, 0xF0, 0x10, 0xF0, 
  0x90, 0x90, 0xF0, 0x10, 0x10, 
  0xF0, 0x80, 0xF0, 0x10, 0xF0, 
  0xF0, 0x80, 0xF0, 0x90, 0xF0, 
  0xF0, 0x10, 0x20, 0x40, 0x40, 
  0xF0, 0x90, 0xF0, 0x90, 0xF0, 
  0xF0, 0x90, 0xF0, 0x10, 0xF0, 
  0xF0, 0x90, 0xF0, 0x90, 0x90, 
  0xE0, 0x90, 0xE0, 0x90, 0xE0, 
  0xF0, 0x80, 0x80, 0x80, 0xF0, 
  0xE0, 0x90, 0x90, 0x90, 0xE0, 
  0xF0, 0x80, 0xF0, 0x80, 0xF0, 
  0xF0, 0x80, 0xF0, 0x80, 0x80  
};

class KChip8 {
public:
  KChip8(State *state, Config *config, SDL *sdlObj, RenderCallback cb, std::mutex *mutex, std::condition_variable *convar)
    : s{state}, cfg{config}, sdl{sdlObj}, rcb{cb}, mtx{mutex}, cv{convar} {
    s->memory = new uint8_t[MAX_MEM];
    if(!s->memory) {
      fprintf(stderr, "Error occurred while allocating memory for the emulator.\n");
      exit(EXIT_FAILURE);
    }
  }

  ~KChip8() {
    delete s->memory; s->memory = nullptr;
    s->display = nullptr;
  }

  void run() {
    while(true) {
      switch(s->status) {
      case Status::STOPPED: {
        render(nullptr);
        std::unique_lock lock(*mtx);
        cv->wait(lock, [&](){ return s->status == Status::RUNNING; });
        init_emu();
        break;
      }
      case Status::RESET:
        init_emu();
        render(nullptr);
        s->status = Status::RUNNING;
        break;
      case Status::RUNNING: {
        sdl->handle_input();
        uint64_t start = SDL_GetPerformanceCounter();
        for(uint32_t i = 0; i < cfg->instPerSec / 60; i++) {
          emulate_op();
        }
        uint64_t end = SDL_GetPerformanceCounter();
        double runtime = static_cast<double>((end - start) * 1000) / static_cast<double>(SDL_GetPerformanceFrequency());
        sdl->delay(std::max(16.67 - runtime, 0.0));

        if(s->draw) {
          render(s->memory + DISPLAY_START);
          s->draw = false;
        }

        update_timers();
        s->prevKeys = s->keys;
        break;
      }
      default: break;
      }
    }

    render(nullptr);
  }

private:
  void init_emu() {
    s->pc = PC_START; 
    s->sp = SP_START;
    s->display = &s->memory[DISPLAY_START];
    memset(s->memory, 0, MAX_MEM);
    memcpy(s->memory + FONT_START, FONT, FONT_SIZE);
    srand(static_cast<unsigned>(time(nullptr)));
    // Get file size
    std::ifstream chipFile(s->romLoc, std::ios::binary | std::ios::ate); 
    if(!chipFile.is_open()) {
      fprintf(stderr, "Error occurred while opening chip-8 file.\n");
      exit(EXIT_FAILURE);
    }
    s->fileSize = chipFile.tellg();
    chipFile.seekg(0, std::ios::beg);
    // Read ROM
    if(!chipFile.read(reinterpret_cast<char*>(s->memory + PC_START), s->fileSize)) {
      fprintf(stderr, "Error occurred while reading chip-8 file.\n");
      exit(EXIT_FAILURE);
    }
  }

  void emulate_op() {
    // Get opcode from first nibble (Big Endian)
    uint8_t *ip = s->memory + s->pc;
    uint8_t opHigh = *ip >> 4;
    uint8_t carry = 0;

    // Handle high bit of opcode
    switch(opHigh) {
      case 0x00:
        switch(ip[1]) {
          case 0xE0:
            memset(s->memory + DISPLAY_START, 0, DISPLAY_SIZE);
            s->draw = true;
            break;
          case 0xEE:
            s->sp -= 2;
            s->pc = static_cast<uint16_t>(s->memory[s->sp]) << 8 | static_cast<uint16_t>(s->memory[s->sp + 1]);
            break;
          default:
            not_implemented(*ip, ip[1]);
        }
        s->pc += 2;
        break;
      case 0x01: s->pc = (static_cast<uint16_t>(*ip & 0x0F) << 8) | static_cast<uint16_t>(ip[1]); break;
      case 0x02:
        s->memory[s->sp] = static_cast<uint8_t>(s->pc >> 8);
        s->memory[s->sp + 1] = static_cast<uint8_t>(s->pc & 0xFF);
        s->sp += 2;
        s->pc = (static_cast<uint16_t>(*ip & 0x0F) << 8) | static_cast<uint16_t>(ip[1]);
        break;
      case 0x03: s->pc += (s->v[*ip & 0x0F] == ip[1]) ? 4 : 2; break;
      case 0x04: s->pc += (s->v[*ip & 0x0F] != ip[1]) ? 4 : 2; break;
      case 0x05: s->pc += (s->v[*ip & 0x0F] == s->v[ip[1] >> 4]) ? 4 : 2; break;
      case 0x06:
        s->v[*ip & 0x0F] = ip[1]; 
        s->pc += 2;
        break;
      case 0x07: 
        s->v[*ip & 0x0F] += ip[1]; 
        s->pc += 2;
        break;
      case 0x08:
        switch(ip[1] & 0x0F) {
          case 0x00: s->v[*ip & 0x0F] = s->v[ip[1] >> 4]; break;
          case 0x01: 
            s->v[*ip & 0x0F] |= s->v[ip[1] >> 4]; 
            s->v[0x0F] = 0;
            break;
          case 0x02: 
            s->v[*ip & 0x0F] &= s->v[ip[1] >> 4]; 
            s->v[0x0F] = 0;
            break;
          case 0x03: 
            s->v[*ip & 0x0F] ^= s->v[ip[1] >> 4]; 
            s->v[0x0F] = 0;
            break;
          case 0x04: {
            uint8_t res = s->v[*ip & 0x0F] + s->v[ip[1] >> 4]; 
            uint8_t flag = res < s->v[*ip & 0x0F]; 
            s->v[*ip & 0x0F] = res;
            s->v[0x0F] = flag; 
            break;
          } 
          case 0x05:
            carry = s->v[*ip & 0x0F] >= s->v[ip[1] >> 4]; 
            s->v[*ip & 0x0F] -= s->v[ip[1] >> 4]; 
            s->v[0x0F] = carry;
            break;
          case 0x06:
            carry = s->v[ip[1] >> 4] & 0x01; 
            s->v[*ip & 0x0F] = s->v[ip[1] >> 4] >> 1; 
            s->v[0x0F] = carry;
            break;
          case 0x07:
            carry = s->v[*ip & 0x0F] <= s->v[ip[1] >> 4]; 
            s->v[*ip & 0x0F] = s->v[ip[1] >> 4] - s->v[*ip & 0x0F];
            s->v[0x0F] = carry;
            break;
          case 0x0E: 
            carry = static_cast<int8_t>(s->v[ip[1] >> 4]) < 0; 
            s->v[*ip & 0x0F] = s->v[ip[1] >> 4] << 1; 
            s->v[0x0F] = carry;
            break;
          default:
            not_implemented(*ip, ip[1]);
        }
        s->pc += 2;
        break;
      case 0x09: s->pc += (s->v[*ip & 0x0F] != s->v[ip[1] >> 4]) ? 4 : 2; break;
      case 0x0A:
        s->i = (static_cast<uint16_t>(*ip & 0x0F) << 8) | static_cast<uint16_t>(ip[1]); 
        s->pc += 2;
        break;
      case 0x0B: {
        uint16_t imm = (static_cast<uint16_t>(*ip & 0x0F) << 8) | static_cast<uint16_t>(ip[1]);
        s->pc = static_cast<uint16_t>(s->v[0x00]) + imm; 
        break;
      }
      case 0x0C: 
        s->v[*ip & 0x0F] = static_cast<uint8_t>(rand()) & ip[1];
        s->pc += 2;
        break;
      case 0x0D: {
        uint8_t x = s->v[*ip & 0x0F] % cfg->displayWidth, y = s->v[ip[1] >> 4] % cfg->displayHeight, n = ip[1] & 0x0F;
        s->v[0x0F] = 0;
        for(uint8_t dy = 0; dy < n && y + dy < cfg->displayHeight; dy++) {
          uint8_t spriteByte = s->memory[s->i + dy];
          for(uint8_t dx = 0; dx < 8 && x + dx < cfg->displayWidth; dx++) {
            uint8_t xBytes = (x + dx) / 8, yBytes = (y + dy) * cfg->displayWidth / 8;
            uint8_t *displayLoc = s->memory + DISPLAY_START + yBytes + xBytes;
            uint8_t xBitPos = 7 - (x + dx) % 8, spriteBitPos = 7 - dx;
            uint8_t spriteBit = spriteByte >> spriteBitPos & 1, displayBit = *displayLoc >> xBitPos & 1;
            s->v[0x0F] |= spriteBit & displayBit;
            *displayLoc ^= spriteBit << xBitPos;
          }
        }
        s->draw = true;
        s->pc += 2;
        break;
      }
      case 0x0E:
        switch(ip[1]) {
          case 0x9E: s->pc += s->keys & (1 << s->v[*ip & 0x0F]) ? 4 : 2; break;
          case 0xA1: s->pc += s->keys & (1 << s->v[*ip & 0x0F]) ? 2 : 4; break;
          default:
            not_implemented(*ip, ip[1]);
        }
        break;
      case 0x0F:
        switch(ip[1]) {
          case 0x07: s->v[*ip & 0x0F] = s->delay; break;
          case 0x0A:
            static int8_t lastPressedKey = -1;
            if(std::popcount(s->keys) > std::popcount(s->prevKeys)) {
              lastPressedKey = static_cast<int8_t>(s->keys ^ s->prevKeys);
            }
            if(lastPressedKey != -1 && !(s->keys & lastPressedKey)) {
              uint8_t key = static_cast<uint8_t>(lastPressedKey);
              s->v[*ip & 0x0F] = static_cast<uint8_t>(std::countr_zero(key));
              lastPressedKey = -1;
              break;
            }
            return;
          case 0x15: s->delay = s->v[*ip & 0x0F]; break;
          case 0x18: s->sound = s->v[*ip & 0x0F]; break;
          case 0x1E: s->i += static_cast<uint16_t>(s->v[*ip & 0x0F]); break;
          case 0x29: s->i = FONT_START + 5 * static_cast<uint16_t>(s->v[*ip & 0x0F]); break;
          case 0x33:
            s->memory[s->i] = s->v[*ip & 0x0F];
            s->memory[s->i + 2] = s->memory[s->i] % 10;
            s->memory[s->i] /= 10;
            s->memory[s->i + 1] = s->memory[s->i] % 10;
            s->memory[s->i] /= 10;
            break;
          case 0x55:
            memcpy(s->memory + s->i, s->v, 1 + static_cast<size_t>(*ip & 0x0F));
            s->i += 1 + static_cast<size_t>(*ip & 0x0F);
            break;
          case 0x65:
            memcpy(s->v, s->memory + s->i, 1 + static_cast<size_t>(*ip & 0x0F));
            s->i += 1 + static_cast<size_t>(*ip & 0x0F);
            break;
          default:
            not_implemented(*ip, ip[1]);
        }
        s->pc += 2;
        break;
      default:
        not_implemented(*ip, ip[1]);
    }
  }

  void update_timers() {
    s->delay -= s->delay > 0;
    sdl->toggle_audio(s->sound > 0);
    s->sound -= s->sound > 0;
  }

  void render(uint8_t const *display) { 
    rcb(display);
  }

  void not_implemented(uint8_t high, uint8_t low) {
    fprintf(stderr, "Opcode(%02X%02X) is not implemented.\n", high, low);
    exit(EXIT_FAILURE);
  }

  State *s;
  Config *cfg;
  SDL *sdl;
  RenderCallback rcb;
  std::mutex *mtx;
  std::condition_variable *cv;
};