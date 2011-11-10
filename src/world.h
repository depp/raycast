#ifndef WORLD_H
#define WORLD_H

struct obj {
    int x0, y0;
    int x1, y1;
    int angle;
    int speed, strafe;
};

struct world {
    struct obj player;
};

struct world *world_new(void);

void world_delete(struct world *w);

void world_update(struct world *w);

#endif
