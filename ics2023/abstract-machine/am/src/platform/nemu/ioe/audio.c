#include <am.h>
#include <nemu.h>
#include <klib.h>
#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

static int head = 0;

void __am_audio_init() {
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = 0;
  cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  outl(AUDIO_FREQ_ADDR,ctrl->freq);  //write
  outl(AUDIO_SAMPLES_ADDR,ctrl->samples);
  outl(AUDIO_CHANNELS_ADDR,ctrl->channels);
  outl(AUDIO_INIT_ADDR,1);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  int length = ctl->buf.end - ctl->buf.start;
  int size = inl(AUDIO_SBUF_SIZE_ADDR);
  int count = inl(AUDIO_COUNT_ADDR);
  int nr_write = length;
  while(size - count < nr_write){ //wait for space
  }
  if(nr_write + head < size){
    memcpy((void *)(uintptr_t)(AUDIO_SBUF_ADDR + head), ctl->buf.start, nr_write);
    head+=nr_write;
  }//copy if enough
  else{ 
    int cpy = size - head;
    memcpy((void *)(uintptr_t)(AUDIO_SBUF_ADDR + head), ctl->buf.start, cpy);
    memcpy((void *)(uintptr_t)AUDIO_SBUF_ADDR, ctl->buf.start + cpy, nr_write - cpy);
    head = nr_write - cpy;
  }//copy if not enough
  count+=nr_write;
  outl(AUDIO_COUNT_ADDR, count);
}
