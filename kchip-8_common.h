#pragma once
#include <cstdint>
#include <fstream>

enum class Status: uint8_t {
  STOPPED,
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
  std::string rom_loc;
};

struct Config {
  uint32_t display_width = 64;
  uint32_t display_height = 32;
  uint32_t foreground = 0xFFFFFFFF;
  uint32_t background = 0x000000FF;
  uint32_t inst_per_sec = 700;
  uint32_t wave_freq = 440;
  uint32_t sample_rate = 44100;
  int32_t scale = 10;
  int16_t volume = 3000;
  bool outline = true;
};