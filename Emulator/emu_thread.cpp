#include "Emulator/emu_thread.h"
#include "IO/io.h"

EmuThread::EmuThread(Config *config, RenderCallback renderCB): cfg{config}, rcb{renderCB} {}

EmuThread::~EmuThread() {
  delete sdl;
  delete emu;
}

void EmuThread::toggle_pause(bool pause) { state.status = pause ? Status::PAUSED : Status::RUNNING; }
void EmuThread::reset() { state.status = Status::RESET; }
void EmuThread::set_rom(std::string filepath) { state.romLoc = filepath; }
bool EmuThread::stopped() const { return state.status == Status::STOPPED; }
void EmuThread::key_press(SDL_KeyCode key, bool keyDown) { 
  SDL_Event sdlEvent;
  sdlEvent.type = keyDown ? SDL_KEYDOWN : SDL_KEYUP;
  sdlEvent.key.keysym.sym = key;
  SDL_PushEvent(&sdlEvent);
}
void EmuThread::start_emu() { 
  std::unique_lock lock(mtx);
  state.status = Status::RUNNING; 
  cv.notify_all();
}
void EmuThread::stop() { state.status = Status::STOPPED; }

void EmuThread::run() {
  state.status = Status::STOPPED;
  ErrorCallback ecb = [&](const std::string &msg, const bool fatal){ error(msg, fatal); };
  sdl = new SDL(&state, cfg, ecb); 
  emu = new KChip8(&state, cfg, sdl, rcb, ecb, &mtx, &cv);
  emu->run();
}