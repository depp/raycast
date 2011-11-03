/* Each "pixels[n]" pointer points to a 32-bit texture with dimensions
   (w >> n) by (h >> n), with minimum width and height 1.  The
   textures are stored as columns starting with the top left
   pixel.  */
struct texture {
    unsigned wbits, hbits;
    unsigned count;
    void *pixels[];
};

enum {
    TEXTURE_COLUMN = 1 << 0,
    TEXTURE_COLOR = 1 << 1
};

void texture_load(struct texture **tex, const char *path);

void texture_make(struct texture **tex, unsigned char *ptr,
                  unsigned w, unsigned h, unsigned rb, unsigned flags);
