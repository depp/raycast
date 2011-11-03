#include "imath.h"
#include "texture.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>

#define MAX_SIZE 1024

void texture_load(struct texture **tex, const char *path)
{
    FILE *f;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    unsigned w, h, rb, i, flags = TEXTURE_COLUMN;
    unsigned char *jdata = NULL, *jptr[1];

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    f = fopen(path, "rb");
    if (!f)
        goto fail_errno;
    jpeg_stdio_src(&cinfo, f);
    jpeg_read_header(&cinfo, TRUE);

    w = cinfo.image_width;
    h = cinfo.image_height;
    if (w > MAX_SIZE || h > MAX_SIZE) {
        fprintf(stderr, "%s: too big\n", path);
        exit(1);
    }

    switch (cinfo.out_color_space) {
    case JCS_GRAYSCALE:
        rb = align(w);
        // rb = w;
        break;

    case JCS_RGB:
        flags |= TEXTURE_COLOR;
        rb = align(3 * w);
        // rb = 3 * w;
        break;

    default:
        fprintf(stderr, "%s: unknown color space\n", path);
        exit(1);
    }

    jpeg_start_decompress(&cinfo);

    jdata = malloc(rb * h);
    if (!jdata)
        abort();
    while ((unsigned) cinfo.output_scanline < h) {
        for (i = 0; i < 1; ++i)
            jptr[i] = jdata + cinfo.output_scanline * (rb + i);
        jpeg_read_scanlines(&cinfo, jptr, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(f);

    texture_make(tex, jdata, w, h, rb, flags);
    free(jdata);
    return;

fail_errno:
    fprintf(stderr, "%s: %s\n", path, strerror(errno));
    exit(1);
}
