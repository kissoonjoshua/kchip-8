#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <bit>
#include "kchip-8.h"

int main(int argc, char** argv) {
  // Enforce correct program usage
  if(argc != 2) {
    fprintf(stderr, "Chip-8 Emulator Usage: disasm <filename>\n");
    return EXIT_FAILURE;
  }

  // Get file size
  std::ifstream chipFile(argv[1], std::ios::binary | std::ios::ate); 
  if(!chipFile.is_open()) {
    fprintf(stderr, "Error occurred while opening chip-8 file\n");
    return EXIT_FAILURE;
  }
  std::streamsize fileSize = chipFile.tellg();
  chipFile.seekg(0, std::ios::beg);

  // Initialize emulator state
  State s{};
  s.pc = PC_START;
  s.sp = SP_START;
  s.memory = new uint8_t[MAX_MEM]{};
  if(!s.memory) {
    fprintf(stderr, "Memory allocation error\n");
    return EXIT_FAILURE;
  }
  s.display = &s.memory[DISPLAY_START];
  memcpy(s.memory + FONT_START, FONT, FONT_SIZE);
  srand(static_cast<unsigned>(time(nullptr)));

  // Read chip-8 file into emulator's memory
  if(!chipFile.read(reinterpret_cast<char*>(s.memory + PC_START), fileSize)) {
    fprintf(stderr, "Error occurred while reading chip-8 file\n");
    return EXIT_FAILURE;
  }

  // Run emulator
  while(s.pc < fileSize + s.pc) {
    emulate_op(s);
  }

  // Free memory
  delete s.memory; s.memory = nullptr;
  s.display = nullptr;

  return EXIT_SUCCESS;
}

void emulate_op(State &s) {
  // Get opcode from first nibble (Big Endian)
  uint8_t *ip = s.memory + s.pc;
  uint8_t opHigh = *ip >> 4;

  // Handle high bit of opcode
  switch(opHigh) {
    case 0x00:
      switch(ip[1]) {
        case 0xE0: redraw(s.memory + DISPLAY_START); break;
        case 0xEE:
          s.sp -= 2;
          s.pc = static_cast<uint16_t>(s.memory[s.sp]) << 8 | static_cast<uint16_t>(s.memory[s.sp + 1]);
          break;
        default:
          not_implemented(s, *ip, ip[1]);
        s.pc += 2;
      }
      break;
    case 0x01: s.pc = (static_cast<uint16_t>(*ip & 0x0F) << 8) | static_cast<uint16_t>(ip[1]); break;
    case 0x20:
      s.memory[s.sp] = s.pc >> 8;
      s.memory[s.sp + 1] = s.pc & 0xFF;
      s.sp += 2;
      s.pc = (static_cast<uint16_t>(*ip & 0x0F) << 8) | static_cast<uint16_t>(ip[1]);
      break;
    case 0x03: s.pc += (s.v[*ip & 0x0F] == ip[1]) ? 4 : 2; break;
    case 0x04: s.pc += (s.v[*ip & 0x0F] != ip[1]) ? 4 : 2; break;
    case 0x05: s.pc += (s.v[*ip & 0x0F] == s.v[ip[1] >> 4]) ? 4 : 2; break;
    case 0x06:
      s.v[*ip & 0x0F] = ip[1]; 
      s.pc += 2;
      break;
    case 0x07: 
      s.v[*ip & 0x0F] += ip[1]; 
      s.pc += 2;
      break;
    case 0x08:
      switch(ip[1] & 0x0F) {
        case 0x00: s.v[*ip & 0x0F] = s.v[ip[1] >> 4]; break;
        case 0x01: s.v[*ip & 0x0F] |= s.v[ip[1] >> 4]; break;
        case 0x02: s.v[*ip & 0x0F] &= s.v[ip[1] >> 4]; break;
        case 0x03: s.v[*ip & 0x0F] ^= s.v[ip[1] >> 4]; break;
        case 0x04: {
          uint8_t res = s.v[*ip & 0x0F] + s.v[ip[1] >> 4]; 
          s.v[0x0F] = res < s.v[*ip & 0x0F]; 
          s.v[*ip & 0x0F] = res;
          break;
        } 
        case 0x05: 
          s.v[0x0F] = s.v[*ip & 0x0F] >= s.v[ip[1] >> 4]; 
          s.v[*ip & 0x0F] -= s.v[ip[1] >> 4]; 
          break;
        case 0x06:
          s.v[0x0F] = s.v[*ip & 0x0F] & 0x01; 
          s.v[*ip & 0x0F] >>= 1; 
          break;
        case 0x07: 
          s.v[0x0F] = s.v[*ip & 0x0F] <= s.v[ip[1] >> 4]; 
          s.v[*ip & 0x0F] = s.v[ip[1] >> 4] - s.v[*ip & 0x0F]; 
          break;
        case 0x0E: 
          s.v[0x0F] = static_cast<int8_t>(s.v[*ip & 0x0F]) < 0; 
          s.v[*ip & 0x0F] <<= 1; 
          break;
        default:
          not_implemented(s, *ip, ip[1]);
        s.pc += 2;
      }
      break;
    case 0x09: s.pc += (s.v[*ip & 0x0F] != s.v[ip[1] >> 4]) ? 4 : 2; break;
    case 0x0A:
      s.i = (static_cast<uint16_t>(*ip & 0x0F) << 8) | static_cast<uint16_t>(ip[1]); 
      s.pc += 2;
      break;
    case 0x0B: {
      uint16_t imm = (static_cast<uint16_t>(*ip & 0x0F) << 8) | static_cast<uint16_t>(ip[1]);
      s.pc = static_cast<uint16_t>(s.v[0x00]) + imm; 
      break;
    }
    case 0x0C: 
      s.v[*ip & 0x0F] = static_cast<uint8_t>(rand()) & ip[1];
      s.pc += 2;
      break;
    case 0x0D: {
      uint8_t x = s.v[*ip & 0x0F], y = s.v[ip[1] >> 4], n = ip[1] & 0x0F;
      for(uint8_t i = 0; i < n && y + i < DISPLAY_HEIGHT; i++) {
        for(uint8_t j = 0; j < 8 && x + j < DISPLAY_WIDTH; j++) {
          uint8_t xByte = (x + j) / 8, xBitPos = 7 - (x + j) % 8, yPos = y + i, spriteBitPos = 7 - j;
          uint8_t *displayLoc = s.memory + DISPLAY_START + yPos * DISPLAY_WIDTH + xByte;
          uint8_t spriteBit = s.memory[s.i + i] >> spriteBitPos & 1, displayBit = *displayLoc >> xBitPos & 1;
          s.v[0x0F] |= spriteBit & displayBit;
          *displayLoc ^= spriteBit << xBitPos;
        }
      }
      redraw(s.memory + DISPLAY_START);
      break;
    }
    case 0x0E:
      switch(ip[1]) {
        case 0x9E: s.pc += s.keys & (1 << s.v[*ip & 0x0F]) ? 2 : 0; break;
        case 0xA1: s.pc += s.keys & (1 << s.v[*ip & 0x0F]) ? 0 : 2; break;
        default:
          not_implemented(s, *ip, ip[1]);
      }
      break;
    case 0x0F:
      switch(ip[1]) {
        case 0x07: s.v[*ip & 0x0F] = s.delay; break;
        case 0x0A:
          if(s.key_wait) {
            if(std::popcount(s.keys) > std::popcount(s.prev_keys)) {
              s.v[*ip & 0x0F] = s.keys ^ s.prev_keys;
              s.key_wait = false;
              break;
            }
          } else {
            s.key_wait = true;
            s.prev_keys = s.keys;
          }
          return;
        case 0x15: s.delay = s.v[*ip & 0x0F]; break;
        case 0x18: s.sound = s.v[*ip & 0x0F]; break;
        case 0x1E: s.i += static_cast<uint16_t>(s.v[*ip & 0x0F]); break;
        case 0x29: s.i = FONT_START + 5 * static_cast<uint16_t>(s.v[*ip & 0x0F]); break;
        case 0x33:
          s.memory[s.i] = s.v[*ip & 0x0F];
          s.memory[s.i + 2] = s.memory[s.i] % 10;
          s.memory[s.i] /= 10;
          s.memory[s.i + 1] = s.memory[s.i] % 10;
          s.memory[s.i] /= 10;
          break;
        case 0x55:
          memcpy(s.memory + s.i, s.v, 1 + static_cast<size_t>(*ip & 0x0F));
          break;
        case 0x65:
          memcpy(s.v, s.memory + s.i, 1 + static_cast<size_t>(*ip & 0x0F));
          break;
        default:
          not_implemented(s, *ip, ip[1]);
        s.pc += 2;
      }
      break;
    default:
      not_implemented(s, *ip, ip[1]);
  }
}

void redraw(uint8_t *display) {}

void not_implemented(State &s, uint8_t high, uint8_t low) {
  fprintf(stderr, "%02X%02X opcode is not implemented\n", high, low);
  delete s.memory; s.memory = nullptr;
  s.display = nullptr;
  exit(EXIT_FAILURE);
}