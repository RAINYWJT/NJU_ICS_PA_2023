#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

#define MAX_FD 4096

size_t open_offset[MAX_FD];  //remember the openoff

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_DISPINFO, FD_FB};//in out and error

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]    = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT]   = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR]   = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_EVENT]    = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_FB]       = {"/dev/fb", 0, 0, invalid_read, fb_write},
#include "files.h"
};

////////////////////////////////////////////////////////////////////write
size_t fs_write(int fd, const void *buf, size_t len){
  //printf("yes1\n");
  if (fd < 0 || fd >= MAX_FD) {
    printf("Invalid file descriptor.\n");
    return -1;
  }
  Finfo *file = file_table + fd;
  if(file->write != NULL){
    size_t result = file->write(buf, 0, len);
    return result;
  }
  else{ //if zero read ramdisk
    size_t result = ramdisk_write(buf, file->disk_offset + open_offset[fd], len);
    open_offset[fd] += len;
    return result;
  }
  /*
  size_t i;
  char *buffer = (char *)buf;
  if (fd == 1 || fd == 2) {
    for (i = 0; i < len; i++) {
      putch(buffer[i]);
      //printf("yes11111111111111111111111111111111111111\n");
    }
  } else {
    panic("invalid reach!\n");
    return -1;
  }
  return len;
  */
}

////////////////////////////////////////////////////////////////////lssek
size_t fs_lseek(int fd, size_t offset, int whence){
  if (fd < 0 || fd >= MAX_FD) {
    printf("Invalid file descriptor.\n");
    return -1;
  }
  //printf("seek\n");
  switch(whence){
    case SEEK_SET:{
      open_offset[fd] = offset;
      break;
    }
    case SEEK_CUR:{
      open_offset[fd] += offset;
      break;
    }
    case SEEK_END:{
      open_offset[fd] = file_table[fd].size + offset;
      break;
    }
    default: 
      printf("Invalid 'whence' parameter.\n");
      return -1;
  }
  return open_offset[fd];
}

////////////////////////////////////////////////////////////////////open
int fs_open(const char *pathname, int flags, int mode){
  int len = LENGTH(file_table);
  //printf("%d\n",len);
  for(int i=0;i<len;i++){
    if(strcmp(pathname, file_table[i].name) == 0){
      //printf("%s\n",file_table[i].name);
      return i;
    }
  }
  panic("NO SUCH A FILE!\n");
  return -1;
}

////////////////////////////////////////////////////////////////////close
int fs_close(int fd){
  open_offset[fd] = 0;
  return 0;
}

////////////////////////////////////////////////////////////////////read
size_t fs_read(int fd, void *buf, size_t len){
  Finfo *file = &file_table[fd];
  if(file->read != NULL){
    return file->read(buf, 0, len);
  }
  if(open_offset[fd] + len > file->size){
    len = file->size - open_offset[fd];
  }
  size_t ramoff = file->disk_offset + open_offset[fd];
  size_t result = ramdisk_read(buf, ramoff, len);
  open_offset[fd] = open_offset[fd] + result;
  //printf("read : %d\n",result);
  return result;  
}

////////////////////////////////////////////////////////////////////init
void init_fs() {
  // TODO: initialize the size of /dev/fbsize_t fs_write(int fd, const void *buf, size_t len)
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = cfg.width * cfg.height; 
  //printf("sz: %d\n",file_table[FD_FB].size);
}
