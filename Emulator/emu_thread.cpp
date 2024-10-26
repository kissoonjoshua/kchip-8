#include "Emulator/emu_thread.h"
#include "Emulator/emu_core.h"
#include "IO/io.h"

EmuThread::EmuThread(Config *config, RenderCallback callback): cfg{config}, rcb{callback} {}

void EmuThread::run() {
  SDL sdl(&state, cfg); 
  KChip8 emu(&state, cfg, &sdl, rcb);
  emu.set_rom("C:\\Users\\Josh\\Downloads\\Code\\kchip-8\\Brix [Andreas Gustafsson, 1990].ch8");
  emu.run();
}