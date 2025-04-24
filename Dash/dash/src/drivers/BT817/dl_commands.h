#ifndef DL_H
#define DL_H

#define ALPHA_FUNC(func, ref)                                                  \
    ((0x09 << 24) + ((func << 8) & 0x700) + (ref & 0xFF))

#define BEGIN(prim) ((0x1F << 24) + (prim & 0xF))

#define BITMAP_EXT_FORMAT(fmt) ((0x2E << 24) + (format & 0xFFFF))

#define BITMAP_LAYOUT(fmt, linestride, height)                                 \
    ((0x07 << 24) + ((format << 19) & 0xF80000) +                              \
     ((linestride << 9) & 0x7fE00) + (height & 0x1FF))

#define BITMAP_LAYOUT_H(linestride, height)                                    \
    ((0x28 << 24) + ((linestride << 2) & 0xC) + (height & 0x3))

#define BITMAP_SIZE(filter, wrapx, wrapy, width, height)                       \
    ((0x08 << 24) + ((filter << 20) & 0x100000) + ((wrapx << 19) & 0x80000) +  \
     ((wrapy << 18) & 0x40000) + ((width << 9) & 0x3FE00) + (height & 0x1FF))

#define BITMAP_SIZE_H(width, height)                                           \
    ((0x29 << 24) + ((width << 2) & 0xC) + (height & 0x3))

#define BITMAP_SOURCE(addr) ((0x01 << 24) + (addr & 0xFFFFFF))

#define BITMAP_SWIZZLE(r, g, b, a)                                             \
    ((0x2f << 24) + ((r << 9) & 0xA00) + ((g << 6) & 0x180) +                  \
     ((b << 3) & 0x38) + (a & 0x7))

#define BITMAP_TRANSFORM_A(p, v)                                               \
    ((0x15 << 24) + ((p << 17) & 0x20000) + (v & 0x1FFFF))

#define BITMAP_TRANSFORM_B(p, v)                                               \
    ((0x16 << 24) + ((p << 17) & 0x20000) + (v & 0x1FFFF))

#define BITMAP_TRANSFORM_C(c) ((0x17 << 24) + (c & 0xFFFFFF))

#define BITMAP_TRANSFORM_D(p, v)                                               \
    ((0x18 << 24) + ((p << 17) & 0x20000) + (v & 0x1FFFF))

#define BITMAP_TRANSFORM_E(p, v)                                               \
    ((0x19 << 24) + ((p << 17) & 0x20000) + (v & 0x1FFFF))

#define BITMAP_TRANSFORM_F(f) ((0x1A << 24) + (f & 0xFFFFFF))

#define BLEND_FUNC(src, dst) ((0x0B << 24) + ((src << 3) & 0x38) + (dst & 0x7))

#define CALL(dst) ((0x1D << 24) + (dst & 0xFFFF))

#define CELL(cell) ((0x06 << 24) + (cell & 0x7F))

#define CLEAR(c, s, t)                                                         \
    ((0x26 << 24) + ((c << 2) & 0x4) + ((s << 1) & 0x2) + (t & 0x1))

#define CLEAR_COLOR_A(alpha) ((0x0F << 24) + (alpha & 0xFF))

#define CLEAR_COLOR_RGB(r, g, b)                                               \
    ((0x02 << 24) + ((r << 16) & 0xFF0000) + ((g << 8) & 0xFF00) + (b & 0xFF))

#define CLEAR_STENCIL(s) ((0x11 << 24) + (s & 0xFF))

#define CLEAR_TAG(t) ((0x12 << 24) + (t & 0xFF))

#define COLOR_A(alpha) ((0x10 << 24) + (alpha & 0xFF))

#define COLOR_MASK(r, g, b, a)                                                 \
    ((0x20 << 24) + ((r << 3) & 0x8) + ((g << 2) & 0x4) + ((b << 1) & 0x2) +   \
     (a & 0x1))

#define COLOR_RGB(red, blue, green)                                            \
    ((0x04 << 24) + ((red << 16) & 0xFF0000) + ((blue << 8) & 0xFF00) +        \
     (green & 0xFF))

#define DISPLAY() (0x00000000)

#define END() (0x21 << 24)

#define JUMP(dest)        ((0x1E << 24) + (dest & 0xFFFF))
#define LINE_WIDTH(width) ((0x0E << 24) + (width & FFF))

#define MACRO(m) ((0x25 << 24) + (m & 0x1))

#define NOP() (0x2D << 24)

#define PALETTE_SOURCE(addr) ((0x2A << 24) + (addr & 0x3FFFFF))

#define POINT_SIZE(size) ((0x0D << 24) + (size & 0x1FFF))

#define RESTORE_CONTEXT() (0x23 << 24)

#define RETURN() (0x24 << 24)

#define SAVE_CONTEXT() (0x22 << 24)

#define SCISSOR_SIZE(width, height)                                            \
    ((0x1C << 24) + ((width << 12) & 0xFFF000) + (height & 0xFFF))

#define SCISSOR_XY(x, y) ((0x1B << 24) + ((x << 11) & 0x3FF800) + (y & 0x7FF))

#define STENCIL_FUNC(func, ref, mask)                                          \
    ((0x0A << 24) + ((func << 16) & 0xF0000) + ((ref << 8) & 0xFF00) +         \
     (mask & 0xFF))

#define STENCIL_MASK(mask) ((0x13 << 24) + (mask & 0xFF))

#define STENCIL_OP(sfail, spass)                                               \
    ((0x0C << 24) + ((sfail << 3) & 0x38) + (spass & 0x7))

#define TAG(s) ((0x03 << 24) + (s & 0xFF))

#define TAG_MASK(mask) ((0x14 << 24) + (mask & 0x1))

#define VERTEX2II(x, y, handle, cell)                                          \
    ((0x2 << 30) + ((x << 21) & 0x3FE00000) + ((y << 12) & 0x1FF000) +         \
     ((handle << 7) & 0xF80) + (cell & 0x7F))

#define VERTEX2F(x, y) ((0x1 << 30) + ((x << 15) & 0x3FFF8000) + (y & 0x7FFF))

#define VERTEX_FORMAT(frac) ((0x27 << 24) + (frac & 0x7))

#define VERTEX_TRANSLATE_X(x) ((0x2B << 24) + (x & 0x1FFFF))

#define VERTEX_TRANSLATE_Y(y) ((0x2C << 24) + (y & 0x1FFFF))

#define ZERO                           0
#define ONE                            1
#define RED                            2
#define GREEN                          3
#define BLUE                           4
#define ALPHA                          5
#define SRC_ALPHA                      2
#define DST_ALPHA                      3
#define ONE_MINUS_SRC_ALPHA            4
#define ONE_MINUS_DST_ALPHA            5
#define ARGB1555                       0
#define L1                             1
#define L4                             2
#define L8                             3
#define RGB332                         4
#define ARGB2                          5
#define ARGB4                          6
#define RGB565                         7
#define TEXT8X8                        9
#define TEXTVGA                        10
#define BARGRAPH                       11
#define PALETTED565                    14
#define PALETTED4444                   15
#define PALETTED8                      16
#define L2                             17
#define GLFORMAT                       31
#define COMPRESSED_RGBA_ASTC_4x4_KHR   37808
#define COMPRESSED_RGBA_ASTC_5x4_KHR   37809
#define COMPRESSED_RGBA_ASTC_5x5_KHR   37810
#define COMPRESSED_RGBA_ASTC_6x5_KHR   37811
#define COMPRESSED_RGBA_ASTC_6x6_KHR   37812
#define COMPRESSED_RGBA_ASTC_8x5_KHR   37813
#define COMPRESSED_RGBA_ASTC_8x6_KHR   37814
#define COMPRESSED_RGBA_ASTC_8x8_KHR   37815
#define COMPRESSED_RGBA_ASTC_10x5_KHR  37816
#define COMPRESSED_RGBA_ASTC_10x6_KHR  37817
#define COMPRESSED_RGBA_ASTC_10x8_KHR  37818
#define COMPRESSED_RGBA_ASTC_10x10_KHR 37819
#define COMPRESSED_RGBA_ASTC_12x10_KHR 37820
#define COMPRESSED_RGBA_ASTC_12x12_KHR 37821
#define KEEP                           1
#define REPLACE                        2
#define INCR                           3
#define DECR                           4
#define INVERT                         5
#define BITMAPS                        0x1
#define POINTS                         0x2
#define LINES                          0x3
#define LINE_STRIP                     0x4
#define EDGE_STRIP_R                   0x4
#define EDGE_STRIP_L                   0x6
#define EDGE_STRIP_A                   0x7
#define EDGE_STRIP_B                   0x8
#define RECTS                          0x9
#define NEVER                          0
#define LESS                           1
#define LEQUAL                         2
#define GREATER                        3
#define GEQUAL                         4
#define EQUAL                          5
#define NOTEQUAL                       6
#define ALWAYS                         7

#endif
