#include "cursor.h"
#include <stdio.h>

CursorPos getCursorPos() {
  CursorPos pos;

  printf("\033[6n");
  scanf("\033[%d;%dR", &pos.row, &pos.col);

  return pos;
}

void moveHome() { printf("\033[H"); }

void moveUp(int rows) { printf("\033[%dA", rows); }

void moveDown(int rows) { printf("\033[%dB", rows); }

void moveRight(int cols) { printf("\033[%dC", cols); }

void moveLeft(int cols) { printf("\033[%dD", cols); }

void setCursorCol(int col) { printf("\033[%dG", col); }

void setCursorRow(int row) { printf("\033[%dH", row); }

void setCursorPos(int rows, int cols) {
  printf("\033[%d;%dH", rows, cols);
  printf("\033[%d;%df", rows, cols);
}

void hideCursor() { printf("\033[?25l"); };

void showCursor() { printf("\033[?25h"); };

void eraseScreen() { printf("\033[2J"); }