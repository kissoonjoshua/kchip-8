#pragma once
#include "Common/common.h"
#include "Emulator/emu_core.h"

#include <QObject>
#include <QThread>
#include <mutex>
#include <condition_variable>

class EmuThread : public QThread {
  Q_OBJECT
public:
  EmuThread(Config *config, RenderCallback renderCB);
  ~EmuThread();
  void toggle_pause(bool pause);
  void reset();
  void set_rom(std::string filepath);
  bool stopped() const;
  void start_emu();
  void stop();
  void key_press(SDL_KeyCode key, bool keyDown);
signals:
  void error(const std::string &msg, const bool fatal);
protected:
  void run() override;
private:
  KChip8 *emu;
  State state{};
  Config *cfg;
  SDL *sdl;
  RenderCallback rcb;
  std::mutex mtx;
  std::condition_variable cv;
};