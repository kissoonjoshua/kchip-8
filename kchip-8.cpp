#include "kchip-8_common.h"
#include "kchip-8_core.h"
#include "kchip-8_sdl.h"

int main(int argc, char **argv) {
  if(argc != 2) exit(EXIT_FAILURE);

  // Initialize emulator state and set rom location
  Config cfg{};
  State state{};
  SDL sdl(state, cfg);  
  KChip8 emu(state, cfg, sdl);
  emu.set_rom(argv[1]);

  // Run emulator
  emu.run();

  return EXIT_SUCCESS;
}