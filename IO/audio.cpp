#include "io.h"
#include "Common/common.h"

void SDL::toggle_audio(bool on) {
  SDL_PauseAudioDevice(device, !on);
}

void SDL::audio_callback(void *userdata, uint8_t *stream, int len) {
  static uint32_t sample_index = 0;
  Config *cfg = reinterpret_cast<Config*>(userdata);
  int16_t *audioStream = reinterpret_cast<int16_t*>(stream);
  int32_t wavePeriod = cfg->sampleRate / cfg->waveFreq;
  int32_t halfWavePeriod = wavePeriod / 2;

  for(int i = 0; i < len / 2; i++, sample_index++) {
    audioStream[i] = (sample_index / halfWavePeriod % 2) ? cfg->volume : -cfg->volume;
  }
}