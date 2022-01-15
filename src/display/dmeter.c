#include "dmeter.h"

size_t dmeter_draw(dmeter_t dm, char* buf, size_t n) {
    if (n == 0) return 0;
    else if (n == 1) {
        buf[0] = '|';
        return 1;
    }

    size_t app = 0;

    buf[app++] = '[';

    double normalized = ((double) (dm.value - dm.min)) / (dm.max - dm.min);
    for (size_t i = 0; i < dm.width - 2; i++) {
        double ni = ((double) i) / (dm.width - 2);
        buf[app++] = (ni < normalized) ? '|' : ' ';
    }

    buf[app++] = ']';

    return app;
}