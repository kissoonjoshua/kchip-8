#include "kchip-8_common.h"
#include "kchip-8_core.h"
#include "kchip-8_sdl.h"

int main(int argc, char **argv) {
  if(argc != 2) exit(EXIT_FAILURE);

  // Initialize emulator state and read program
  Config cfg{};
  State state{};
  SDL sdl(state, cfg);  
  KChip8 emu(state, cfg, sdl);
  emu.load_rom(argv[1]);

  // Run emulator
  emu.run();

  return EXIT_SUCCESS;
}