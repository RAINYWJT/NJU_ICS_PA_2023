#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

////////////////////////////////////////////////////////////////////////serial_write chuankou
size_t serial_write(const void *buf, size_t offset, size_t len) {
  //yield();
  const char *p = (const char *)buf;
  for(size_t i=0; i<len; i++){
    putch(*(p + i));
  }
  return len;
}

////////////////////////////////////////////////////////////////////////events read
size_t events_read(void *buf, size_t offset, size_t len) {
    //printf("jijij\n");
    //yield();
    AM_INPUT_KEYBRD_T key = io_read(AM_INPUT_KEYBRD);
    if (key.keycode == 0) { 
        return 0;
    } else {
      char *kb_type = "";
      if(key.keydown == 0){
         kb_type = "ku";
      }
      else if(key.keydown == 1){
         kb_type = "kd";
      }
      //printf("%d\n",key.keycode);
      snprintf((char *)buf + offset, len - offset, "%s %2d\n", kb_type, key.keycode);
      return len - offset;
    }
}

////////////////////////////////////////////////////////////////////////dispinfo read
size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T vga_1 = io_read(AM_GPU_CONFIG);
  sprintf((char *)buf, "WIDTH:%d\nHEIGHT:%d\n",vga_1.width, vga_1.height);
  //printf("%d %d\n",vga_1.width, vga_1.height);
  int length =strlen(buf);
  //printf("%d\n",length);
  return length;
  //return 0;
}

////////////////////////////////////////////////////////////////////////fb write
size_t fb_write(const void *buf, size_t offset, size_t len) {
  //yield();
  AM_GPU_CONFIG_T vga_2 = io_read(AM_GPU_CONFIG);
  AM_GPU_FBDRAW_T vga_3;
  vga_3.pixels = (void *)buf;
  vga_3.sync = true;
  //printf("len: %d\n",len);
  //printf("w2 and h2: %d %d\n",vga_2.width, vga_2.height);
  vga_3.x = (offset % vga_2.width + vga_2.width/2); 
  vga_3.y = (offset / vga_2.width + vga_2.height/2); 

  //vga_3.x = (offset % vga_2.width ); 
  //vga_3.y = (offset / vga_2.width ); 
  vga_3.w = (len >> 16);
  vga_3.h = (len & 0xFFFF) ;
  //printf("device,w h x y:%d %d %d %d\n",vga_3.w, vga_3.h,vga_3.x, vga_3.y);
  int real_y = (vga_3.y - vga_3.h/2);
  int real_x = (vga_3.x - vga_3.w/2); 
  io_write(AM_GPU_FBDRAW, real_x, real_y,vga_3.pixels, vga_3.w, vga_3.h, vga_3.sync);
  return 0;
}

//  vga_3.x = (vga_2.width - vga_3.w) / 2;
//  vga_3.y = (vga_2.height - vga_3.h) / 2;

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
