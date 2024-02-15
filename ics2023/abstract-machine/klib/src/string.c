#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int i=0;
  while(s[i]!='\0'){
    i++;
  }
  return i;
}

char *strcpy(char *dst, const char *src) { //copy
    char * temp =  dst;
    while(*src!='\0'){
      *dst++ = *src++;
    }
    *dst = '\0';
    return temp;
}

char *strcat(char *dst, const char *src) { //connect
    char * temp = dst;
    while(*dst != '\0'){
      dst++;
    }
    while(*src != '\0'){
      *dst++ = *src++;
    }
    *dst = '\0';
    return temp;
}

int strcmp(const char *s1, const char *s2) { //the same???
    while(*s1!='\0' && *s2!='\0'){
      if(*s1 != *s2){
        return *s1 - *s2;
      }
      s1++;
      s2++;
    }
    return *s1 - *s2;
}

void *memset(void *s, int c, size_t n) { //fill
    char *dst = (char *)s;
    while(n!=0){
      *dst++ = c;
      n--;
    }
    return s;
}

int memcmp(const void *s1, const void *s2, size_t n) { //compare
    int i=0;
    while(i<n){
      if(((char *)s1)[i] != ((char *)s2)[i]){
        return ((char *)s1)[i] - ((char *)s2)[i];
      }    
      i++;
    }
    return 0;
}

/////////////////////////////////////////////////////////////haven't been check
void *memcpy(void *out, const void *in, size_t n) {
    uint8_t* pout = (uint8_t*)out;
    const uint8_t* pin = (const uint8_t*)in;
    for(;n>0;--n){
      *pout++ = *pin++;
    }
    return out;
}

void *memmove(void *dst, const void *src, size_t n) {
    void * ret = dst;
    if(dst < src){
      while(n--){
        *(char*)dst = *(char*)src;
        dst = (char*)dst +1;
        src = (char*)src +1;
      }
    }
    else{
      while (n--)
      {
        *((char*)dst + n) = *((char*)src +n);
      }
    }
    return ret;
}

char *strncpy(char *dst, const char *src, size_t n) { // copy partial
  if(src == NULL || dst == NULL){
    return dst;
  }
  char *start = dst;
  while(n && (*dst++ = *src++)){
    n--;
  }
  if(n){
    while(n){
      *dst++ = '\0';
      n--;
    }
  }
  return start;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  if(s1 == NULL || s2 == NULL){
    return 0;
  }
  int i;
  for (i = 0; i < n && s1[i] != '\0' && s2[i] != '\0'; i++){
    if (s1[i] != s2[i])
      return (int)(s1[i]) - (int)(s2[i]);
  }
  if (i == n)
    return 0;
  return (int)(s1[i]) - (int)(s2[i]);
}

#endif
