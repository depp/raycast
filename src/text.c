#include "text.h"
#include "draw.h"
#include <cairo/cairo.h>
#include <pango/pangocairo.h>
#include <string.h>

void text_draw(struct pixbuf *p, const char *text, int x, int y)
{
    cairo_surface_t *cs = cairo_image_surface_create_for_data(
        (void *) p->ptr, CAIRO_FORMAT_ARGB32,
        p->width, p->height, p->row * 4);
    cairo_t *cr = cairo_create(cs);
    cairo_translate(cr, x, y);
    PangoContext *pc = pango_cairo_create_context(cr);
    PangoLayout *pl = pango_layout_new(pc);
    // pango_layout_set_alignment(pl, PANGO_ALIGN_LEFT);
    pango_layout_set_text(pl, text, strlen(text));
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    pango_cairo_show_layout(cr, pl);
    g_object_unref(pl);
    g_object_unref(pc);
    cairo_destroy(cr);
    cairo_surface_destroy(cs);
}
