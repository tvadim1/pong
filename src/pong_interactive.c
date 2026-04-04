#include <ncurses.h>

#define W 80
#define H 25
#define WIN_SCORE 21
#define BALL_DELAY 4 // мяч движется каждые ball_delay тиков
#define TICK_MS 80 // миллисекунд на одно событие двига мяча
#define ESC_KEY 27 // код клавиши escape

// инициализация библиотеки ncurses, переводит терминал в управляемый режим
void init_ncurses() {
  initscr(); // захватывает управление терминалом
  noecho(); // не отображать нажатые клавиши на экране
  curs_set(0); // скрыть мигающий курсор
  keypad(stdscr,
         TRUE); // (опциональный п-р) чтобы не выскочил при нажатии F1, стрелок
                // и др (не нужен для ESC)
  nodelay(stdscr, TRUE); // сделать getch() неблокирующим и будет возвращать
                         // ERR, если нет ввода
}

// возвращает символ для клетки (x, y) на основе текущего состояния игры
char get_cell(int bx, int by, int p1y, int p2y, int x, int y) {
  char result = ' '; // пустое место

  if (x == W / 2 && y % 2 == 1)
    result = ':'; // пунктирная центральная линия
  if (x == W - 2 && y >= p2y && y <= p2y + 2)
    result = '#'; // правая ракетка
  if (x == 1 && y >= p1y && y <= p1y + 2)
    result = '#'; // левая ракетка
  if (x == bx && y == by)
    result = 'O'; // мяч
  if (x == 0 || x == W - 1)
    result = '|'; // левая и правая границы
  if (y == 0 || y == H - 1)
    result = '-'; // верхняя и нижняя границы

  return result;
}

// отрисовывает всё игровое поле, счет и подсказку
void render(int bx, int by, int p1y, int p2y, int s1, int s2) {
  int x, y;

  for (y = 0; y < H; y++) {
    for (x = 0; x < W; x++) {
      mvaddch(y, x, get_cell(bx, by, p1y, p2y, x, y));
    }
  }

  mvprintw(H, 0, "p1 [a/z]: %d   p2 [k/m]: %d   esc = exit", s1, s2);

  refresh(); // показывает все нарисованные изменения на экране
}

// выводит сообщение о победителе и ждёт любой клавиши перед выходом
void show_winner(int s1, int s2) {
  clear(); // очищает экран

  if (s1 >= WIN_SCORE) {
    mvprintw(H / 2, W / 2 - 12, "p1 wins %d:%d!", s1, s2);
  } else {
    mvprintw(H / 2, W / 2 - 12, "p2 wins %d:%d!", s2, s1);
  }

  mvprintw(H / 2 + 1, W / 2 - 10, "press any key...");

  nodelay(stdscr, FALSE); // переводит getch() в режим ожидания
  refresh();

  getch(); // ждёт нажатия любой клавиши
}

int main() {
  // все переменные состояния - без структур, массивов, указателей, глобальных
  // переменных
  int bx = W / 2, by = H / 2, bdx = 1, bdy = 1;
  int p1y = H / 2 - 1, p2y = H / 2 - 1;
  int s1 = 0, s2 = 0;

  int tick = 0; // счётчик тиков для замедления мяча
  int running = 1; // флаг работы игры (0 при нажатии esc)

  init_ncurses();

  render(bx, by, p1y, p2y, s1, s2);

  while (running && s1 < WIN_SCORE && s2 < WIN_SCORE) {
    int ch = getch(); // последняя нажатая клавиша - функция ncurses, настроили,
                      // что теерь не блокирует выполнение, возвращает ERR, если
                      // нет ввода

    // обработка ввода (движение ракеток или выход)
    if (ch == ESC_KEY) {
      running = 0;
    } else {
      if (ch == 'a' && p1y > 1)
        p1y--;
      if (ch == 'z' && p1y < H - 4)
        p1y++;
      if (ch == 'k' && p2y > 1)
        p2y--;
      if (ch == 'm' && p2y < H - 4)
        p2y++;
    }

    tick++;
    // мяч двигается только раз в ball_delay тиков
    if (tick >= BALL_DELAY) {
      bx += bdx;
      by += bdy;

      // отскок от верхней и нижней стенок
      if (by <= 1) {
        by = 1;
        bdy = 1;
      }
      if (by >= H - 2) {
        by = H - 2;
        bdy = -1;
      }
      // отскок от левой ракетки
      if (bx <= 2 && by >= p1y && by <= p1y + 2) {
        bx = 3;
        bdx = 1;
      }
      // отскок от правой ракетки
      if (bx >= W - 3 && by >= p2y && by <= p2y + 2) {
        bx = W - 4;
        bdx = -1;
      }
      // проверка голов (мяч вышел за левую или правую границу)
      if (bx <= 0) {
        s2++;
        bx = W / 2;
        by = H / 2;
        bdx = 1;
        bdy = 1;
        tick = 0;
      }
      if (bx >= W - 1) {
        s1++;
        bx = W / 2;
        by = H / 2;
        bdx = 1;
        bdy = 1;
        tick = 0;
      }

      tick = 0; // сброс счётчика тиков
    }

    render(bx, by, p1y, p2y, s1, s2);

    napms(TICK_MS); //  останавливает выполнение программы на N мс, т.е. нужен
                    //  для управления частотой кадров, napms(60) это 1 событие
                    //  в 60 мс (1000 мс / 60 мс = ~16.67 кадров (т.е. событий)
                    //  в секунду), а TICK_MS 80 это 1 событие в 80 мс (1000 мс
                    //  / 80 мс = 12.5 кадров (событий) в секунду)
    // napms(TICK_MS) пишется внутри while, чтобы приостановить цикл
    // int counter = 0;
    // while (counter < 4) {
    //     napms(60);
    //     counter++;
    // }
    // за 240 мс, будет 4 события в теории
  }

  if (running) {
    show_winner(s1, s2);
  }

  endwin(); // возвращает терминал в обычный режим

  return 0;
}