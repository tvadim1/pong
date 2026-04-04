#include <stdio.h>

#define W 80
#define H 25
#define WIN_SCORE 21

int is_valid(int ch) {
  return (ch == ' ' || ch == 'a' || ch == 'z' || ch == 'k' || ch == 'm');
}

// возвращает новую позицию левой ракетки
int move_p1y(int p1y, int ch) {
  int result = p1y;

  if (ch == 'a' && p1y > 1)
    result = p1y - 1;
  if (ch == 'z' && p1y < H - 4)
    result = p1y + 1;

  return result;
}

// возвращает новую позицию правой ракетки
int move_p2y(int p2y, int ch) {
  int result = p2y;

  if (ch == 'k' && p2y > 1)
    result = p2y - 1;
  if (ch == 'm' && p2y < H - 4)
    result = p2y + 1;

  return result;
}

// возвращает новое направление мяча по X с учётом ракеток
int new_bdx(int bx, int by, int bdx, int p1y, int p2y) {
  int result = bdx;

  if (bx <= 2 && by >= p1y && by <= p1y + 2)
    result = 1;
  if (bx >= W - 3 && by >= p2y && by <= p2y + 2)
    result = -1;

  return result;
}

// возвращает новое направление мяча по Y с учётом стенок
int new_bdy(int by, int bdy) {
  int result = bdy;

  if (by <= 1)
    result = 1;
  if (by >= H - 2)
    result = -1;

  return result;
}

// возвращает координату мяча по X с учётом отскока от ракеток
int new_bx(int bx, int by, int bdx, int p1y, int p2y) {
  int result = bx + bdx;

  if (bx <= 2 && by >= p1y && by <= p1y + 2)
    result = 3;
  if (bx >= W - 3 && by >= p2y && by <= p2y + 2)
    result = W - 4;

  return result;
}

// возвращает координату мяча по Y с учётом стенок
int new_by(int by, int bdy) {
  int result = by + bdy;

  if (by + bdy <= 1)
    result = 1;
  if (by + bdy >= H - 2)
    result = H - 2;

  return result;
}

// функция отрисовки - принимает все переменные состояния
// не использует структуры, массивы, указатели, глобальные переменные
void render(int bx, int by, int p1y, int p2y, int s1, int s2) {
  int x;
  int y;
  char c;

  // очистка экрана (40 пустых строк)
  for (y = 0; y < 40; y++)
    printf("\n");

  for (y = 0; y < H; y++) {
    for (x = 0; x < W; x++) {
      if (y == 0 || y == H - 1)
        c = '-';
      else if (x == 0 || x == W - 1)
        c = '|';
      else if (x == bx && y == by)
        c = 'O';
      else if (x == 1 && y >= p1y && y <= p1y + 2)
        c = '#';
      else if (x == W - 2 && y >= p2y && y <= p2y + 2)
        c = '#';
      else if (x == W / 2 && y % 2 == 1)
        c = ':';
      else
        c = ' ';
      printf("%c", c);
    }
    printf("\n");
  }

  printf("P1 [A/Z]: %d    P2 [K/M]: %d    SPACE = пропустить ход\n", s1, s2);
}

int main() {
  int bx = W / 2, by = H / 2, bdx = 1, bdy = 1; // сброс мяча в центр
  int p1y = H / 2 - 1, p2y = H / 2 - 1; // начальная позиция ракеток
  int s1 = 0, s2 = 0;

  render(bx, by, p1y, p2y, s1, s2);

  while (s1 < WIN_SCORE && s2 < WIN_SCORE) {
    int ch = getchar();

    if (ch != '\n' && is_valid(ch)) {
      p1y = move_p1y(p1y, ch);
      p2y = move_p2y(p2y, ch);
    }

    // движение мяча
    bdx = new_bdx(bx, by, bdx, p1y, p2y);
    bdy = new_bdy(by, bdy);
    bx = new_bx(bx, by, bdx, p1y, p2y);
    by = new_by(by, bdy);

    // проверка голов (мяч вышел за левую или правую границу)
    if (bx <= 0) {
      s2++;
      bx = W / 2;
      by = H / 2;
      bdx = 1;
      bdy = 1;
    }
    if (bx >= W - 1) {
      s1++;
      bx = W / 2;
      by = H / 2;
      bdx = 1;
      bdy = 1;
    }

    render(bx, by, p1y, p2y, s1, s2);
  }

  if (s1 >= WIN_SCORE)
    printf("P1 wins %d:%d!\n", s1, s2);
  else
    printf("P2 wins %d:%d!\n", s2, s1);

  return 0;
}