#ifndef INPUT_H
#define INPUT_H

/* An axis records input for a positional axis.  It combines
   positional data (as from a mouse) and velocity data (as from a
   keyboard or joystick).  Each event has a timestamp, and the axis
   can be queried for its value at a given point in time.  */

struct in_point {
    int time;
    int pos;
    int vel;
};

struct in_axis {
    struct in_point *points;
    unsigned pcount, palloc;
};

void in_axis_init(struct in_axis *a);

void in_axis_destroy(struct in_axis *a);

/* Get the axis position at the given time.  */
int in_axis_get(struct in_axis *a, int time);

/* Set the axis position at the given time.  All events after the
   given time are discarded.  The velocity is not changed.  */
void in_axis_setpos(struct in_axis *a, int time, int pos);

/* Set the axis velocity at the given time.  All events after the
   given time are discarded.  The position is not changed.  */
void in_axis_setvel(struct in_axis *a, int time, int vel);

/* Subtract the given delta from all event timestamps.  Events before
   time zero may be discarded.  */
void in_axis_advance(struct in_axis *a, int time);

#endif
