#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[128] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 128, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n");
  sh_printf("Please cin the tokens like this 'bird'.And if you want to quit just cin thing not include.\n\n");
}

static void sh_prompt() {
  sh_printf("(cin) ");
}

static void sh_handle_cmd(const char *cmd) {
  char command[128];
  strcpy(command, cmd);
  command[strlen(command) - 1] = '\0';//把'\n'搞掉
  char *argv[16];
  int argc = 0;
  char *p = command;
  while (*p != '\0') {
    while (*p == ' ') {
      p++;
    }
    if (*p == '\0') {
      break;
    }
    argv[argc++] = p;
    while (*p != ' ' && *p != '\0') {
      p++;
    }
    if (*p != '\0') {
      *p = '\0';
      p++;
    }
  }
  argv[argc] = NULL;
  execvp(argv[0], argv);
}

static void sh_setenv(){
  setenv("PATH", "/bin:/usr/bin", 0);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();
  sh_setenv();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
