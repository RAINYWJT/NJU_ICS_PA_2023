#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  /*get size*/
  FILE* file = fopen(filename,"r");
  if(file == NULL){
    printf("NO SUCH A FILE!\n");
    return NULL;
  }
  fseek(file, 0 ,SEEK_END);
  size_t filesize = ftell(file);
  //printf("ys!\n");

  /*malloc size*/
  char* buf = (char*)malloc(filesize);
  fseek(file, 0, SEEK_SET);
  if(buf == NULL){
    fclose(file);
    printf("buf len null!\n");
    return NULL;
  }

  /*read into buf*/
  if(fread(buf, 1, filesize,file) != filesize){
    free(buf);
    fclose(file);
    return NULL;
  }

  /*use as parameter*/
  SDL_Surface* surface = STBIMG_LoadFromMemory(buf, filesize);

  /*close*/
  free(buf);
  fclose(file);
  return surface;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
