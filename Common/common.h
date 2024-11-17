#pragma once
#include <cstdint>
#include <fstream>
#include <functional>

enum class Status: uint8_t {
  STOPPED,
  STOPPING,
  PAUSED,
  RUNNING,
  RESET,
};

struct State {
  Status status;
  std::streamsize fileSize;
  uint16_t pc;
  uint16_t sp;
  uint16_t i;
  uint8_t delay;
  uint8_t sound;
  uint16_t keys;
  uint16_t prevKeys;
  uint8_t v[0x10];
  uint8_t *memory;
  uint8_t *display;
  bool draw;
  std::string romLoc;
};

struct Config {
  static constexpr uint8_t displayWidth = 64;
  static constexpr uint8_t displayHeight = 32;
  static constexpr uint32_t displaySize = 2048;
  uint32_t foreground = 0xFFFFFFFF;
  uint32_t background = 0x000000FF;
  uint32_t instPerSec = 700;
  uint32_t waveFreq = 440;
  uint32_t sampleRate = 44100;
  int32_t scale = 10;
  int16_t volume = 3000;
};

using RenderCallback = std::function<void(const uint8_t*)>;
using ErrorCallback = std::function<void(const std::string&, const bool)>;