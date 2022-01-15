#include "dcolor.h"
#include "xutil.h"

#define DCOLOR_USECOLOR 1
#define DCOLOR_USETRUECOLOR 0

size_t dcolor_write(dcolor_t color, char* buf, size_t n) {

    // buf must be at least 20 to accomodate "\e[38;2;255;255;255m" + '\0';
    if (n < 20 || !(DCOLOR_USECOLOR | DCOLOR_USETRUECOLOR)) return 0;  

    if (color.nature == DCOLOR_RESET) {
        x_strncpy(buf, "\e[0m", 4);
        return 4;
    } else if (DCOLOR_USETRUECOLOR) {
        return (size_t) snprintf(buf, 19, "\e[38;2;%d;%d;%dm", color.rgb.r, color.rgb.g, color.rgb.b);
    } else {
        char colorcode = color.hue + color.nature + color.stage;
        return (size_t) snprintf(buf, 19, "\e[%dm", colorcode);
    }
}