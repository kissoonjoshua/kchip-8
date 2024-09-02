#pragma once
#include <cstdint>
#include <fstream>

enum class Status: uint8_t {
  STOPPED,
  PAUSED,
  RUNNING
};

struct State {
  Status status;
  std::streamsize fileSize;
  uint16_t pc;
  uint16_t sp;
  uint16_t i;
  uint8_t delay;
  uint8_t sound;
  uint8_t keys;
  uint8_t prev_keys;
  bool key_wait;
  uint8_t v[0x10];
  uint8_t *memory;
  uint8_t *display;
};

struct Config {
  uint32_t display_width = 0x40;
  uint32_t display_height = 0x20;
  uint32_t foreground = 0xFFFFFFFF;
  uint32_t background = 0x00000000;
  int32_t scale = 10; 
  bool outline = true;
};