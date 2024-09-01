#include <cstdlib>
#include <cstdint>
#include <fstream>

constexpr uint16_t PC_START = 0x200;
constexpr uint16_t SP_START = 0xEA0;
constexpr uint16_t DISPLAY_START = 0xF00;
constexpr uint16_t MAX_MEM = 0x1000;

void emulate_op(State&);
void not_implemented(State&, int8_t, int8_t);

struct State {
  uint16_t pc;
  uint16_t sp;
  uint16_t i;
  uint8_t delay;
  uint8_t sound;
  uint8_t v[16];
  uint8_t *memory;
  uint8_t *display;
};

int main(int argc, char** argv) {
  // Enforce correct program usage
  if(argc != 2) {
    fprintf(stderr, "Chip-8 Emulator Usage: disasm <filename>\n");
    return EXIT_FAILURE;
  }

  // Get file size
  std::ifstream chipFile(argv[1], std::ios::binary | std::ios::ate); 
  std::streamsize fileSize = chipFile.tellg();
  chipFile.seekg(0, std::ios::beg);

  // Create buffer and read chip-8 file
  uint8_t *chipBuffer = static_cast<uint8_t*>(malloc(fileSize + PC_START));
  if(!chipFile.read(reinterpret_cast<char*>(chipBuffer + PC_START), fileSize)) {
    fprintf(stderr, "Error while reading chip-8 file\n");
    return EXIT_FAILURE;
  }

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
  srand(static_cast<unsigned>(time(nullptr)));

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
  uint8_t *ip = &s.memory[s.pc];
  uint8_t opHigh = *ip >> 4;

  // Handle high bits of opcode
  switch(opHigh) {
    case 0x00:
      switch(ip[1]) {
        case 0xE0: printf("CLS"); break;
        case 0xEE: printf("RET"); break;
        default:
          not_implemented(s, *ip, ip[1]);
      }
      break;
    case 0x01: s.pc = (static_cast<uint16_t>(*ip & 0x0F) << 8) | static_cast<uint16_t>(ip[1]); break;
    case 0x20: printf("%-10s 0x%1X%02X", "CALL", *ip & 0x0F, ip[1]); break;
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
    case 0x0D: printf("%-10s V%1X, V%1x, 0x%1X", "SPRITE", *ip & 0x0F, ip[1] >> 4, ip[1] & 0x0F); break;
    case 0x0E:
      switch(ip[1]) {
        case 0x9E: printf("%-10s V%1X", "SKIP.KEY", *ip & 0x0F); break;
        case 0xA1: printf("%-10s V%1X", "SKIP.NOKEY", *ip & 0x0F); break;
        default:
          not_implemented(s, *ip, ip[1]);
      }
      break;
    case 0x0F:
      switch(ip[1]) {
        case 0x07: printf("%-10s V%1X, DELAY", "MOV", *ip & 0x0F); break;
        case 0x0A: printf("%-10s V%1X", "WAITKEY", *ip & 0x0F); break;
        case 0x15: printf("%-10s DELAY, V%1X", "MOV", *ip & 0x0F); break;
        case 0x18: printf("%-10s SOUND, V%1X", "MOV", *ip & 0x0F); break;
        case 0x1E: printf("%-10s I, V%1X", "ADD", *ip & 0x0F); break;
        case 0x29: printf("%-10s V%1X", "SPRITECHAR", *ip & 0x0F); break;
        case 0x33: printf("%-10s V%1X", "MOVBCD", *ip & 0x0F); break;
        case 0x55: printf("%-10s [I], V0-V%1X", "MOVM", *ip & 0x0F); break;
        case 0x65: printf("%-10s V0-V%1X, [I]", "MOVM", *ip & 0x0F); break;
        default:
          not_implemented(s, *ip, ip[1]);
      }
      break;
    default:
      not_implemented(s, *ip, ip[1]);
  }
}

void not_implemented(State &s, uint8_t high, uint8_t low) {
  fprintf(stderr, "%02X%02X opcode is not implemented\n", high, low);
  delete s.memory; s.memory = nullptr;
  s.display = nullptr;
  exit(EXIT_FAILURE);
}