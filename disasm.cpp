#include <cstdlib>
#include <cstdint>
#include <fstream>

constexpr uint16_t PROGRAM_START = 0x200;

void disassemble_op(uint8_t*, int);

int main(int argc, char** argv) {
  // Enforce correct program usage
  if(argc != 2) {
    fprintf(stderr, "Chip-8 Disassembler Usage: disasm <filename>\n");
    return EXIT_FAILURE;
  }

  // Get file size
  std::ifstream chipFile(argv[1], std::ios::binary | std::ios::ate); 
  std::streamsize fileSize = chipFile.tellg();
  chipFile.seekg(0, std::ios::beg);

  // Create buffer and read chip-8 file
  uint8_t *chipBuffer = new uint8_t[fileSize + PROGRAM_START];
  if(!chipFile.read(reinterpret_cast<char*>(chipBuffer + PROGRAM_START), fileSize)) {
    fprintf(stderr, "Error while reading chip-8 file\n");
    return EXIT_FAILURE;
  }

  uint16_t pc = PROGRAM_START;

  // Run disassembler
  while(pc < fileSize + PROGRAM_START) {
    disassemble_op(chipBuffer, pc);
    pc += 2;
  }

  // Free memory
  delete chipBuffer; chipBuffer = nullptr;

  return EXIT_SUCCESS;
}

void disassemble_op(uint8_t *buf, int pc) {
  // Get opcode from first nibble (Big Endian)
  uint8_t *ip = &buf[pc];
  uint8_t opHigh = *ip >> 4;
  printf("%04X %02X %02X ", pc, *ip, ip[1]);

  // Handle high bits of opcode
  switch(opHigh) {
    case 0x00:
      switch(ip[1]) {
        case 0xE0: printf("CLS"); break;
        case 0xEE: printf("RET"); break;
        default:
          printf("unimplemented instruction");
      }
      break;
    case 0x01: printf("%-10s 0x%1X%02X", "JMP", *ip & 0x0F, ip[1]); break;
    case 0x20: printf("%-10s 0x%1X%02X", "CALL", *ip & 0x0F, ip[1]); break;
    case 0x03: printf("%-10s V%1X, 0x%02X", "SKIP.EQ", *ip & 0x0F, ip[1]); break;
    case 0x04: printf("%-10s V%1X, 0x%02X", "SKIP.NE", *ip & 0x0F, ip[1]); break;
    case 0x05: printf("%-10s V%1X, V%1X", "SKIP.EQ", *ip & 0x0F, ip[1] >> 4); break;
    case 0x06: printf("%-10s V%1X, 0x%02X", "MOVI", *ip & 0x0F, ip[1]); break;
    case 0x07: printf("%-10s V%1X, 0x%02X", "ADDI", *ip & 0x0F, ip[1]); break;
    case 0x08:
      switch(ip[1] & 0x0F) {
        case 0x00: printf("%-10s V%1X, V%1X", "MOV", *ip & 0x0F, ip[1] >> 4); break;
        case 0x01: printf("%-10s V%1X, V%1X", "OR", *ip & 0x0F, ip[1] >> 4); break;
        case 0x02: printf("%-10s V%1X, V%1X", "AND", *ip & 0x0F, ip[1] >> 4); break;
        case 0x03: printf("%-10s V%1X, V%1X", "XOR", *ip & 0x0F, ip[1] >> 4); break;
        case 0x04: printf("%-10s V%1X, V%1X", "ADD.", *ip & 0x0F, ip[1] >> 4); break;
        case 0x05: printf("%-10s V%1X, V%1X", "SUB.", *ip & 0x0F, ip[1] >> 4); break;
        case 0x06: printf("%-10s V%1X", "SHR.", *ip & 0x0F); break;
        case 0x07: printf("%-10s V%1X, V%1X", "SUBB.", *ip & 0x0F, ip[1] >> 4); break;
        case 0x0E: printf("%-10s V%1X", "SHL.", *ip & 0x0F); break;
        default:
          printf("unimplemented instruction");
      }
      break;
    case 0x09: printf("%-10s V%1X, V%02X", "SKIP.NE", *ip & 0x0F, ip[1] >> 4); break;
    case 0x0A: printf("%-10s I, 0x%1X%02X", "MOVI", *ip & 0x0F, ip[1]); break;
    case 0x0B: printf("%-10s [V0 + 0x%1X%02X]", "JMP", *ip & 0x0F, ip[1]); break;
    case 0x0C: printf("%-10s V%1X, RAND, 0x%02X", "MOVI.RAND", *ip & 0x0F, ip[1]); break;
    case 0x0D: printf("%-10s V%1X, V%1x, 0x%1X", "SPRITE", *ip & 0x0F, ip[1] >> 4, ip[1] & 0x0F); break;
    case 0x0E:
      switch(ip[1]) {
        case 0x9E: printf("%-10s V%1X", "SKIP.KEY", *ip & 0x0F); break;
        case 0xA1: printf("%-10s V%1X", "SKIP.NOKEY", *ip & 0x0F); break;
        default:
          printf("unimplemented instruction");
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
          printf("unimplemented instruction");
      }
      break;
    default:
      printf("unimplemented instruction");
  }

  printf("\n");
}