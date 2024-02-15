#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>
#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

uint8_t keystate[(int)(sizeof(keyname)/sizeof(keyname[0]))];
//////////////////////////////////////////////////////////////////////event wrong
int SDL_PollEvent(SDL_Event *ev) {
  char buf[128];
  if (NDL_PollEvent(buf, sizeof(buf)/sizeof(buf[0]) - 1) != 0) {
    if (strncmp(buf, "kd ", 3) == 0) {
      ev->type = SDL_KEYDOWN;
      int key_op;
      sscanf(buf + 3, "%2d %s", &key_op, buf + 6);
      ev->key.keysym.sym = key_op;
      keystate[key_op] = 1;
      return 1;
    }
    else if (strncmp(buf, "ku ", 3) == 0) {
      ev->type = SDL_KEYUP;
      int key_op;
      sscanf(buf + 3, "%2d %s", &key_op, buf + 6);
      ev->key.keysym.sym = key_op;
      keystate[key_op] = 0;
      return 1;
    }
  }
  return 0;
}

/*int SDL_PollEvent(SDL_Event *ev) {
  //printf("yes!!\n");
  char buf[1024];
  static int key_op = 0;
  if(NDL_PollEvent(buf, sizeof(buf)/sizeof(buf[0])-1)!=0){
    if (strncmp(buf, "kd ", 3) == 0) {
      //printf("yes1!!\n");
      ev->type = SDL_KEYDOWN;
      sscanf(buf+3,"%2d %s",&key_op,buf+6);
      ev->key.keysym.sym = key_op;
      return 1;
    }
      else if (strncmp(buf, "ku ", 3) == 0) {
      //printf("yes2!!\n");
      ev->type = SDL_KEYUP;
      uint8_t keycode = sscanf(buf+3,"%2d %s",&key_op,buf+6);
      if (keycode == key_op) {
      key_op = 0;
      return 1;
      }
    } 
  }
  return 0;
}*/


int SDL_WaitEvent(SDL_Event *event) {
  //printf("waitevent!\n");
  while (SDL_PollEvent(event) == 0){
    
  };
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  printf("SDL_Getkey don't delete this info!\n");
  if(numkeys != NULL){*numkeys = (int)(sizeof(keyname)/sizeof(keyname[0]));}
  return keystate;
}
