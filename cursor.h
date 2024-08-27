#ifndef CURSOR_H_
#define CURSOR_H_

typedef struct {
  int row;
  int col;
} CursorPos;

CursorPos getCursorPos();

void moveHome(void);
void moveUp(int rows);
void moveDown(int rows);
void moveLeft(int cols);
void moveRight(int cols);

void hideCursor(void);
void showCursor(void);
void setCursorCol(int col);
void setCursorRow(int row);
void setCursorPos(int row, int col);

void eraseScreen(void);

#endif // CURSOR_H_