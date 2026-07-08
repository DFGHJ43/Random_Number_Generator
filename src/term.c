#include "term.h"

#include <stdio.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

/* ══════════════════════════════════════════════════════════
 * Terminal control (ANSI escape codes)
 * ══════════════════════════════════════════════════════════ */

void term_clear(void) {
    printf("\033[2J\033[H");
}

void term_goto(int row, int col) {
    printf("\033[%d;%dH", row + 1, col + 1);
}

void term_hide_cursor(void) {
    printf("\033[?25l");
}

void term_show_cursor(void) {
    printf("\033[?25h");
}

void term_flush(void) {
    fflush(stdout);
}

void term_sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

#ifdef _WIN32
void term_enable_vt(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

/* ══════════════════════════════════════════════════════════
 * Keyboard input
 * ══════════════════════════════════════════════════════════ */

#ifdef _WIN32
int kb_hit(void) { return _kbhit(); }
int kb_get(void) { return _getch(); }
#else
int kb_hit(void) {
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= (unsigned int)~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) { ungetc(ch, stdin); return 1; }
    return 0;
}

int kb_get(void) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= (unsigned int)~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

int read_key(void) {
    int c = kb_get();
    if (c == 0 || c == 224) {  /* extended key prefix (Windows) */
        int c2 = kb_get();
        switch (c2) {
        case 72: return KEY_UP;
        case 80: return KEY_DOWN;
        case 75: return KEY_LEFT;
        case 77: return KEY_RIGHT;
        default: return c2;
        }
    }
    if (c == 27) {  /* ESC or ANSI escape sequence */
        if (kb_hit()) {
            int c2 = kb_get();
            if (c2 == '[') {
                int c3 = kb_get();
                switch (c3) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
                default:  return c3;
                }
            }
        }
        return KEY_ESC;
    }
    if (c == '\r') c = '\n';
    return c;
}
