// keyboard.hpp

#pragma once

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/select.h>
#endif

namespace keyboard {

#ifdef _WIN32

    inline bool kbhit() {
        return _kbhit();
    }

    inline char getch() {
        return _getch();
    }

#else

    // POSIX (Linux/macOS)
    inline void setBufferedInput(bool enable) {
        static bool enabled = true;
        static struct termios old;
        struct termios newt;

        if (enable && !enabled) {
            tcsetattr(STDIN_FILENO, TCSANOW, &old);
            enabled = true;
        } else if (!enable && enabled) {
            tcgetattr(STDIN_FILENO, &newt);
            old = newt;
            newt.c_lflag &= ~(ICANON | ECHO); // no line buffering, no echo
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            enabled = false;
        }
    }

    inline bool kbhit() {
        timeval tv = {0, 0};
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
    }

    inline char getch() {
        char c;
        if (read(STDIN_FILENO, &c, 1) < 0) return 0;
        return c;
    }

#endif

}

