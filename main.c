#include "characters.h"
#include "cursor.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static const int START_ROW = 5;
static const int START_COL = 10;
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

void printCursorPos() {
  CursorPos p = getCursorPos();
  printf("Row: %d Col: %d\n", p.row, p.col);
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

void printTimeASCII() {
  time_t time_t = time(NULL);
  struct tm timeInfo = *localtime(&time_t);
  TimeDigits td = getTimeDigits(timeInfo);
  CursorPos startPos, endPos;

  eraseScreen();
  setCursorPos(START_ROW, START_COL);
  startPos = getCursorPos();

  endPos = printChar(CHARS[td.hourTens + 1]);
  setCursorPos(startPos.row, endPos.col + PADDING);
  endPos = printChar(CHARS[td.hourOnes + 1]);
  setCursorPos(startPos.row + 1, endPos.col + PADDING);
  endPos = printChar(CHARS[0]); // :
  setCursorPos(startPos.row, endPos.col + PADDING);
  endPos = printChar(CHARS[td.minuteTens + 1]);
  setCursorPos(startPos.row, endPos.col + PADDING);
  endPos = printChar(CHARS[td.minuteOnes + 1]);
  setCursorPos(startPos.row + 1, endPos.col + PADDING);
  endPos = printChar(CHARS[0]); // :
  setCursorPos(startPos.row, endPos.col + PADDING);
  endPos = printChar(CHARS[td.secondsTens + 1]);
  setCursorPos(startPos.row, endPos.col + PADDING);
  endPos = printChar(CHARS[td.secondsOnes + 1]);
}

void handleExit() {
  tcsetattr(STDIN_FILENO, TCSADRAIN, &savedAttrs);
  showCursor();
  printf("\n");
  exit(0);
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
    printTimeASCII();
    if (read(STDIN_FILENO, &cmd, 1) == 1 && cmd == 'q')
      handleExit();
  }

  return 0;
}