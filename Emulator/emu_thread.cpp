#include "Emulator/emu_thread.h"
#include "IO/io.h"

EmuThread::EmuThread(Config *config, RenderCallback callback): cfg{config}, rcb{callback} {}

EmuThread::~EmuThread() {
  delete sdl;
  delete emu;
}

void EmuThread::toggle_pause(bool pause) { state.status = pause ? Status::PAUSED : Status::RUNNING; }
void EmuThread::reset() { state.status = Status::RESET; }
void EmuThread::set_rom(std::string filepath) { state.romLoc = filepath; }
bool EmuThread::stopped() const { return state.status == Status::STOPPED; }
void EmuThread::start_emu() { 
  std::unique_lock lock(mtx);
  state.status = Status::RUNNING; 
  cv.notify_all();
}
void EmuThread::stop() { state.status = Status::STOPPED; }

void EmuThread::run() {
  state.status = Status::STOPPED;
  sdl = new SDL(&state, cfg); 
  emu = new KChip8(&state, cfg, sdl, rcb, &mtx, &cv);
  emu->run();
}