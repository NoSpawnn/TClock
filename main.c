#include "characters.h"
#include "cursor.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  int hourTens;
  int hourOnes;
  int minuteTens;
  int minuteOnes;
  int secondsTens;
  int secondsOnes;
} TimeDigits;

static const int PADDING = 5;
struct termios savedAttrs;

CursorPos printChar(const char s[]) {
  CursorPos startPos = getCursorPos();

  char tokenable[strlen(s)];
  strcpy(tokenable, s);

  char *tok = strtok(tokenable, "\n");
  while (tok) {
    printf("%s", tok);
    tok = strtok(NULL, "\n");
    if (tok) {
      moveDown(1);
      setCursorCol(startPos.col);
    }
  }

  return getCursorPos();
}

TimeDigits getTimeDigits(struct tm timeInfo) {
  return (TimeDigits){
      .hourTens = timeInfo.tm_hour / 10,
      .hourOnes = timeInfo.tm_hour % 10,
      .minuteTens = timeInfo.tm_min / 10,
      .minuteOnes = timeInfo.tm_min % 10,
      .secondsTens = timeInfo.tm_sec / 10,
      .secondsOnes = timeInfo.tm_sec % 10,
  };
}

struct tm getTime() {
  time_t time_t = time(NULL);
  return *localtime(&time_t);
}

void printTimeASCII(int rowSize, int colSize) {
  TimeDigits td = getTimeDigits(getTime());
  int startRow = rowSize * 0.5 - 2,
      startCol = colSize * 0.5 - 53; // TODO: make less hardcoded
  CursorPos endPos;

  eraseScreen();
  setCursorPos(startRow, startCol);

  endPos = printChar(CHARS[td.hourTens + 1]);
  setCursorPos(startRow, endPos.col + PADDING);
  endPos = printChar(CHARS[td.hourOnes + 1]);
  setCursorPos(startRow + 1, endPos.col + PADDING);
  endPos = printChar(CHARS[0]); // :
  setCursorPos(startRow, endPos.col + PADDING);
  endPos = printChar(CHARS[td.minuteTens + 1]);
  setCursorPos(startRow, endPos.col + PADDING);
  endPos = printChar(CHARS[td.minuteOnes + 1]);
  setCursorPos(startRow + 1, endPos.col + PADDING);
  endPos = printChar(CHARS[0]); // :
  setCursorPos(startRow, endPos.col + PADDING);
  endPos = printChar(CHARS[td.secondsTens + 1]);
  setCursorPos(startRow, endPos.col + PADDING);
  endPos = printChar(CHARS[td.secondsOnes + 1]);
}

void handleExit() {
  tcsetattr(STDIN_FILENO, TCSADRAIN, &savedAttrs);
  showCursor();
  printf("\n");
  exit(0);
}

// Get viewport size
struct winsize getWinSize() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w;
}

int main() {
  signal(SIGINT, handleExit);
  tcgetattr(STDIN_FILENO, &savedAttrs);

  struct termios tAttr;
  cfmakeraw(&tAttr);
  tAttr.c_cc[VMIN] = 0;
  tAttr.c_cc[VTIME] = 1 * 10; // 1 second, `read` timeout
  tcsetattr(STDIN_FILENO, TCSANOW, &tAttr);
  hideCursor();

  char cmd;
  while (true) {
    struct winsize w = getWinSize();
    printTimeASCII(w.ws_row, w.ws_col);

    if (read(STDIN_FILENO, &cmd, 1) == 1 && cmd == 'q')
      handleExit();
  }

  return 0;
}