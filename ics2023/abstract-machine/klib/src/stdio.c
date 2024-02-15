#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*I'd like to mention that in this prinf i have idea of myself but I tried it for long time and all of a waste
so I get the idea from one of blog from csdn and after that I composed my code by myself.I found it also have some bugs with 
sprintf,but now I don't have any way to solve it.
*/
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define ZERO_PAD 1
#define SIGN 2
#define PLUS 4
#define SPACE 8
#define MINUS 16
#define SPECIAL 32

struct format_info {
  int flags;
  int width;
  int precision;
  int qualifier;
};

bool isdigit(char c) {
    if (c >= '0' && c <= '9') {
        return true;
    }
    return false;
}

size_t get_len(const char * s, size_t count)
{
    const char *str = s;
    for (size_t i = 0; i<count && *str != '\0'; ++i, ++str)
    {
        if (((unsigned char)*str & 0xE0) == 0xC0)
        {
            str += 2;
        }
    }
    return str - s;
}

int get(const char **s)
{
    int i = 0;
    int c;
    while ('0' <= (c = **s) && c <= '9')
    {
        i = i*10 + c - '0';
        ++*s;
    }
    return i;
}

int _div(unsigned long* n, unsigned base){
    int __res = 0; 
    if (*n >= base){
        __res = *n % base;
        *n = *n / base;
    } else {
        __res = *n;
        *n = 0;
    }
    return __res;
}

#define dv_ct(n,base) _div(&n,base)
///////////////////////////////////////////////////////////////////////////////

int printf(const char *fmt, ...) { //write in terminal
  char buf[4096];
  va_list args;
  va_start(args, fmt);
  int ans = vsprintf(buf,fmt,args);
  va_end(args);
  putstr(buf);
  return ans;
}

int sprintf(char *out, const char *fmt, ...) { //write  into a string
  va_list list;
  va_start(list,fmt);
  int ans = vsprintf(out,fmt,list);
  va_end(list);
  return ans;
}

int snprintf(char *out, size_t n, const char *fmt, ...) { //limit the size of in
  va_list list;
  va_start(list,fmt);
  int ans = vsnprintf(out,n,fmt,list);
  va_end(list);
  return ans;
}

int vsprintf(char *out, const char *fmt, va_list ap) { //variable could send the para in dynamic,give the maxint as limit
  return vsnprintf(out,__INT_MAX__,fmt,ap);
}

////////////////////////////////////////////////////////////////////////////////////////////
char* delt(char* str, long num, int base, int size, int precision, int type){
  char fill_c, sign, tmp[66];
  //putch('0'+(num==0));
  //putch('0'+(num==2147483647));
  //putch('0'+(num==-1));
  //putch('\n');
  const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
  int i;
  if (type & MINUS){
    type &= ~ZERO_PAD;
    //printf("1:%d\n",type);
  }
  if (base < 2 || base > 36){
    return 0;
  }

  fill_c = (type & ZERO_PAD)?'0':' ';
  //printf("%c\n",fill_c);
  sign = 0;
  if (type & SIGN){
    if (num < 0){
      sign = '-';
      num = -num;
      size--;
    }
    else if (type & PLUS){
      sign = '+';
      size--;
    }
    else if (type & SPACE){
      sign = ' ';
      size--;
    }
  }
  if (type & SPECIAL){
    if (base == 16){
      size -= 2;
  }
  else if (base == 8){
  size -= 1;
    }
  }
  i = 0;
  if (num == 0){
    tmp[i++] = '0';
  }
  else{
    unsigned long nx = (unsigned long)num;
    while(nx != 0){
      tmp[i++] = digits[dv_ct(nx,base)];
    }
  }
  if (i > precision){
    precision = i;
  }
  size -= precision;
  if (!(type & (ZERO_PAD + MINUS))){
    while(size-- > 0){
      *str++ = ' ';
    }
  }
  if (sign){
    //printf("yes1!\n");
    *str++ = sign;
  }
  if (type & SPECIAL){
    if (base == 8){
      *str++ = '0';
    }
    if (base == 16){
      *str++ = '0';
      //printf("yes1!\n");
      *str++ = digits[33];
    }
  }
  if (!(type & MINUS)){
    while(size-->0){
      *str++ = fill_c;
    }
  }
  while(i < precision--){
    *str++ = '0';
  }
  while(i--){
    *str++ = tmp[i];
  }
  while(size-->0){
    *str++ = ' ';
  }
  return str;
}


void parse_format(const char **fmt, struct format_info *info, va_list args){
  info->flags = 0;
  info->width = -1;
  info->precision = -1;
  info->qualifier = -1;

  Loop:
    (*fmt)++;
    switch(**fmt){
      case '0': info->flags |= ZERO_PAD;goto Loop;
      case '#': info->flags |= SPECIAL ;goto Loop;
      case ' ': info->flags |= SPACE   ;goto Loop;
      case '+': info->flags |= PLUS    ;goto Loop;
      case '-': info->flags |= MINUS   ;goto Loop;
    }
    
  /*width*/
  if(isdigit(**fmt)){
    info->width=get(fmt);
  }
  else if (**fmt == '*')              
  {             
    (*fmt)++;
    info->width = va_arg(args, int);
    if (info->width < 0) 
    {	
      info->width = -info->width;
      info->flags |= MINUS;
    }
  }
  
  /*presicion*/
  if (**fmt == '.') 
  {
    (*fmt)++;	
    if ('0' <= **fmt && **fmt <= '9')
      info->precision = get(fmt);
    else if (**fmt == '*') 
    {
      (*fmt)++;
      info->precision = va_arg(args, int);
    }
    if (info->precision < 0)
      info->precision = 0;
  }

  /*decoration*/
  if (**fmt == 'l' && *(*fmt + 1) == 'l') 
  {
    info->qualifier = '1';
    *fmt += 2; // long long
  } else if (**fmt == 'h' || **fmt == 'l') 
  {
    info->qualifier = **fmt;
    (*fmt)++;
  }
}

int vsnprintf(char *out, size_t size, const char *fmt, va_list ap) {
  int length, i, base = 10;
  unsigned long num = 0;
  char* ptr;
  const char* s;

  struct format_info info; 

  for (ptr = out; *fmt; fmt++) {
    if (*fmt != '%') {
      if (ptr - out < size - 1) { 
        *ptr++ = *fmt;
      } else {
        return -1;
      }
      continue;
    }

    parse_format(&fmt, &info, ap);

    switch(*fmt) {
      case 's':
        s = va_arg(ap, char*);
        if (s == NULL) {
          assert(0);
        }
        length = get_len(s, info.precision);
        if (info.flags & MINUS) {
          while (length < info.width--) {
            if (ptr - out < size - 1) { 
              *ptr++ = ' ';
              info.flags |= SIGN;
            } else {
              return -1;
            }
          }
        } else {
          while (length < info.width--) {
            if (ptr - out < size - 1) { 
              *ptr++ = ' ';
              info.flags &= ~SIGN;
            } else {
              return -1;
            }
          }
        }
        for (i = 0; i < length; ++i) {
          if (ptr - out < size - 1) { 
            *ptr++ = *s++;
          } else {
            return -1;
          }
        }
        continue;

      case 'c':
        if (info.flags & MINUS) {
          while (--info.width > 0) {
            if (ptr - out < size - 1) { 
              *ptr++ = ' ';
              info.flags |= SIGN;
            } else {
              return -1;
            }
          }
        } else {
          while (--info.width > 0) {
            if (ptr - out < size - 1) { 
              *ptr++ = ' ';
              info.flags &= ~SIGN;
            } else {
              return -1;
            }
          }
        }
        if (ptr - out < size - 1) { 
          *ptr++ = (unsigned char)va_arg(ap, int);
        } else {
          return -1;
        }
        while (--info.width > 0) {
          if (ptr - out < size - 1) { 
            *ptr++ = ' ';
          } else {
            return -1;
          }
        }
        continue;

      case 'p':
        if (info.width == -1) { 
          info.width = 2 * sizeof(void*);
          info.flags |= ZERO_PAD; 
        }
        ptr = delt(ptr, (unsigned long)va_arg(ap, void*), 16, info.width, info.precision, info.flags);
        continue;

      case 'n':
        if (info.qualifier == 'l') {
          long* temp = va_arg(ap, long *);
          *temp = (ptr - out);
        } else {
          int* temp = va_arg(ap, int *);
          *temp = (ptr - out);
        }
        continue;

      case '%':
        if (ptr - out < size - 1) { 
          *ptr++ = '%';
        } else {
          return -1;
        }
        continue;
      
      default:
        if (info.width && (info.width % 2 == 0)) {
          info.flags |= ZERO_PAD;
        } else {
          info.flags &= ~ZERO_PAD;
        }

        if (info.qualifier == 'l') {
          num = va_arg(ap, unsigned long);
          if (info.flags & SIGN) {
            num = (signed long)num;
          }
        } else if (info.qualifier == '1') {
          num = va_arg(ap, unsigned long long);
          if (info.flags & SIGN) {
            num = (signed long long)num;
          }
        } else if (info.qualifier == 'h') {
          num = (unsigned short)va_arg(ap, int);
          if (info.flags & SIGN) {
            num = (signed short)num;
          }
        } else {
          num = va_arg(ap, unsigned int);
          if (info.flags & SIGN) {
            num = (signed int)num;
          }
        }

        base = info.width % 3 == 0 ? 2 : 10;

        if (info.width > 5) {
          info.precision = info.width / 2;
        } else {
          info.precision = info.width - 1;
        }
        
        ptr = delt(ptr, num, base, info.width, info.precision, info.flags);
    }
  }
  *ptr = '\0';
  
  return ptr - out;
}

#undef ZERO_PAD
#undef SIGN
#undef PLUS
#undef SPACE
#undef MINUS
#undef SPECIAL 

#endif