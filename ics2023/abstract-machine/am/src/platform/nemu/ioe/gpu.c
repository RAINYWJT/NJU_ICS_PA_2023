#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  /*
  int i;
	int temp = inl(VGACTL_ADDR);
	int w = temp >> 16;
	int h = temp & 0xFFFF;
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	for (i = 0; i < w * h; i++) fb[i] = i;
	outl(SYNC_ADDR, 1);
  */
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  int addr = inl(VGACTL_ADDR);
  int width = addr >> 16;
  int height = addr & 0xFFFF;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = width, .height = height,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int i, j;
	int screen_width = io_read(AM_GPU_CONFIG).width;
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	uint32_t *pixels = (uint32_t *)ctl->pixels;
	uint32_t *start = &fb[ctl->y * screen_width + ctl->x];
  uint32_t *row = start; 
  for (i = 0; i < ctl->h; i++) {
    for (j = 0; j < ctl->w; j++) {
      row[j] = pixels[i * ctl->w + j];
    }
    row += screen_width; 
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
