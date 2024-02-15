#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

//////////////////////////////////////////////////////////////keyboard
uint32_t NDL_GetTicks() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return time.tv_usec/1000;
}

//////////////////////////////////////////////////////////////event
int NDL_PollEvent(char *buf, int len) {
  if(len == 0){
    printf("Event len can't be zero!");
    assert(0);
  }
  int file = open("/dev/events",0,0);
  return read(file, buf, len);
  //return 0;
}

// 打开一张(*w) X (*h)的画布
// 如果*w和*h均为0, 则将系统全屏幕作为画布, 并将*w和*h分别设为系统屏幕的大小
void NDL_OpenCanvas(int *w, int *h) {
  int file = open("/proc/dispinfo", 0, 0);
  char readbuf[128];
  read(file, readbuf, sizeof(readbuf));
  sscanf(readbuf,"WIDTH:%d\nHEIGHT:%d\n", &screen_w, &screen_h);
  if(*w == 0 || *h == 0){
    *w = screen_w;
    *h = screen_h;
  }
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    printf("111:%d %d\n",screen_w,screen_h);
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  //printf("h and w:%d%d\n",screen_h,screen_w);
}

// 向画布`(x, y)`坐标处绘制`w*h`的矩形图像, 并将该绘制区域同步到屏幕上
// 图像像素按行优先方式存储在`pixels`中, 每个像素用32位整数以`00RRGGBB`的方式描述颜色
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int fp = open("/dev/fb", 0, 0);
  //printf("123:%d %d\n",x,y);
  int x1 = (screen_w - x) / 2;
  int y1 = (screen_h - y) / 2;
  lseek(fp, (x1 * y1), SEEK_SET);
  write(fp, pixels, (((uint32_t)w << 16) | ((uint32_t)h & 0xFFFF)));
  close(fp);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
