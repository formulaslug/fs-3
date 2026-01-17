#ifndef DL_H
#define DL_H

#define DLSWAP_FRAME 0x2UL
#define DLSWAP_LINE  0x1UL

#define ALPHA_FUNC(func, ref)                                                  \
	((0x09UL << 24) + ((func << 8) & 0x700UL) + (ref & 0xFFUL))

#define BEGIN(prim)            ((0x1FUL << 24) + (prim & 0xFUL))

#define BITMAP_EXT_FORMAT(fmt) ((0x2E << 24) + (format & 0xFFFFUL))

#define BITMAP_LAYOUT(fmt, linestride, height)                                 \
	((0x07UL << 24) + ((format << 19) & 0xF80000UL) +                          \
	 ((linestride << 9) & 0x7fE00UL) + (height & 0x1FFUL))

#define BITMAP_LAYOUT_H(linestride, height)                                    \
	((0x28UL << 24) + ((linestride << 2) & 0xCUL) + (height & 0x3UL))

#define BITMAP_SIZE(filter, wrapx, wrapy, width, height)                       \
	((0x08UL << 24) + ((filter << 20) & 0x100000UL) +                          \
	 ((wrapx << 19) & 0x80000UL) + ((wrapy << 18) & 0x40000UL) +               \
	 ((width << 9) & 0x3FE00UL) + (height & 0x1FFUL))

#define BITMAP_SIZE_H(width, height)                                           \
	((0x29UL << 24) + ((width << 2) & 0xCUL) + (height & 0x3UL))

#define BITMAP_SOURCE(addr) ((0x01UL << 24) + (addr & 0xFFFFFFUL))

#define BITMAP_SWIZZLE(r, g, b, a)                                             \
	((0x2fUL << 24) + ((r << 9) & 0xA00UL) + ((g << 6) & 0x180UL) +              \
	 ((b << 3) & 0x38UL) + (a & 0x7UL))

#define BITMAP_TRANSFORM_A(p, v)                                               \
	((0x15UL << 24) + ((p << 17) & 0x20000UL) + (v & 0x1FFFFUL))

#define BITMAP_TRANSFORM_B(p, v)                                               \
	((0x16UL << 24) + ((p << 17) & 0x20000UL) + (v & 0x1FFFFUL))

#define BITMAP_TRANSFORM_C(c) ((0x17UL << 24) + (c & 0xFFFFFFUL))

#define BITMAP_TRANSFORM_D(p, v)                                               \
	((0x18UL << 24) + ((p << 17) & 0x20000UL) + (v & 0x1FFFFUL))

#define BITMAP_TRANSFORM_E(p, v)                                               \
	((0x19UL << 24) + ((p << 17) & 0x20000UL) + (v & 0x1FFFFUL))

#define BITMAP_TRANSFORM_F(f) ((0x1AUL << 24) + (f & 0xFFFFFFUL))

#define BLEND_FUNC(src, dst)  ((0x0BUL << 24) + ((src << 3) & 0x38UL) + (dst & 0x7UL))

#define CALL(dst)             ((0x1DUL << 24) + (dst & 0xFFFFUL))

#define CELL(cell)            ((0x06UL << 24) + (cell & 0x7FUL))

#define CLEAR(c, s, t)                                                         \
	((0x26UL << 24) + ((c << 2) & 0x4UL) + ((s << 1) & 0x2UL) + (t & 0x1UL))

#define CLEAR_COLOR_A(alpha) ((0x0FUL << 24) + (alpha & 0xFFUL))

#define CLEAR_COLOR_RGB(r, g, b)                                               \
	((0x02UL << 24) + ((r << 16) & 0xFF0000UL) + ((g << 8) & 0xFF00UL) +         \
	 (b & 0xFFUL))

#define CLEAR_STENCIL(s) ((0x11UL << 24) + (s & 0xFFUL))

#define CLEAR_TAG(t)     ((0x12UL << 24) + (t & 0xFFUL))

#define COLOR_A(alpha)   ((0x10UL << 24) + (alpha & 0xFFUL))

#define COLOR_MASK(r, g, b, a)                                                 \
	((0x20UL << 24) + ((r << 3) & 0x8UL) + ((g << 2) & 0x4UL) +                  \
	 ((b << 1) & 0x2UL) + (a & 0x1UL))

#define COLOR_RGB(red, green, blue)                                            \
	((0x04UL << 24) + ((red << 16) & 0xFF0000UL) + ((green<< 8) & 0xFF00UL) +    \
	 (blue & 0xFFUL))

#define DISPLAY()            (0x00000000UL)

#define END()                (0x21UL << 24)

#define JUMP(dest)           ((0x1EUL << 24) + (dest & 0xFFFFUL))
#define LINE_WIDTH(width)    ((0x0EUL << 24) + (width & 0xFFFUL))

#define MACRO(m)             ((0x25UL << 24) + (m & 0x1UL))

#define NOP()                (0x2DUL << 24)

#define PALETTE_SOURCE(addr) ((0x2AUL << 24) + (addr & 0x3FFFFFUL))

#define POINT_SIZE(size)     ((0x0DUL << 24) + (size & 0x1FFFUL))

#define RESTORE_CONTEXT()    (0x23UL << 24)

#define RETURN()             (0x24UL << 24)

#define SAVE_CONTEXT()       (0x22UL << 24)

#define SCISSOR_SIZE(width, height)                                            \
	((0x1CUL << 24) + ((width << 12) & 0xFFF000UL) + (height & 0xFFFUL))

#define SCISSOR_XY(x, y) ((0x1BUL << 24) + ((x << 11) & 0x3FF800UL) + (y & 0x7FFUL))

#define STENCIL_FUNC(func, ref, mask)                                          \
	((0x0AUL << 24) + ((func << 16) & 0xF0000UL) + ((ref << 8) & 0xFF00UL) +     \
	 (mask & 0xFFUL))

#define STENCIL_MASK(mask) ((0x13UL << 24) + (mask & 0xFFUL))

#define STENCIL_OP(sfail, spass)                                               \
	((0x0CUL << 24) + ((sfail << 3) & 0x38UL) + (spass & 0x7UL))

#define TAG(s)         ((0x03UL << 24) + (s & 0xFFUL))

#define TAG_MASK(mask) ((0x14UL << 24) + (mask & 0x1UL))

#define VERTEX2II(x, y, handle, cell)                                          \
	((0x2UL << 30) + ((x << 21) & 0x3FE00000UL) + ((y << 12) & 0x1FF000UL) +     \
	 ((handle << 7) & 0xF80) + (cell & 0x7F))

#define VERTEX2F(x, y)                 ((0x1UL << 30) + ((x << 15) & 0x3FFF8000UL) + (y & 0x7FFFUL))

#define VERTEX_FORMAT(frac)            ((0x27UL << 24) + (frac & 0x7UL))

#define VERTEX_TRANSLATE_X(x)          ((0x2BUL << 24) + (x & 0x1FFFFUL))

#define VERTEX_TRANSLATE_Y(y)          ((0x2CUL << 24) + (y & 0x1FFFFUL))

#define ZERO                           0UL
#define ONE                            1UL
#define RED                            2UL
#define GREEN                          3UL
#define BLUE                           4UL
#define ALPHA                          5UL
#define SRC_ALPHA                      2UL
#define DST_ALPHA                      3UL
#define ONE_MINUS_SRC_ALPHA            4UL
#define ONE_MINUS_DST_ALPHA            5UL
#define ARGB1555                       0UL
#define L1                             1UL
#define L4                             2UL
#define L8                             3UL
#define RGB332                         4UL
#define ARGB2                          5UL
#define ARGB4                          6UL
#define RGB565                         7UL
#define TEXT8X8                        9UL
#define TEXTVGA                        10UL
#define BARGRAPH                       11UL
#define PALETTED565                    14UL
#define PALETTED4444                   15UL
#define PALETTED8                      16UL
#define L2                             17UL
#define GLFORMAT                       31UL
#define COMPRESSED_RGBA_ASTC_4x4_KHR   37808UL
#define COMPRESSED_RGBA_ASTC_5x4_KHR   37809UL
#define COMPRESSED_RGBA_ASTC_5x5_KHR   37810UL
#define COMPRESSED_RGBA_ASTC_6x5_KHR   37811UL
#define COMPRESSED_RGBA_ASTC_6x6_KHR   37812UL
#define COMPRESSED_RGBA_ASTC_8x5_KHR   37813UL
#define COMPRESSED_RGBA_ASTC_8x6_KHR   37814UL
#define COMPRESSED_RGBA_ASTC_8x8_KHR   37815UL
#define COMPRESSED_RGBA_ASTC_10x5_KHR  37816UL
#define COMPRESSED_RGBA_ASTC_10x6_KHR  37817UL
#define COMPRESSED_RGBA_ASTC_10x8_KHR  37818UL
#define COMPRESSED_RGBA_ASTC_10x10_KHR 37819UL
#define COMPRESSED_RGBA_ASTC_12x10_KHR 37820UL
#define COMPRESSED_RGBA_ASTC_12x12_KHR 37821UL
#define KEEP                           1UL
#define REPLACE                        2UL
#define INCR                           3UL
#define DECR                           4UL
#define INVERT                         5UL
#define BITMAPS                        0x1UL
#define POINTS                         0x2UL
#define LINES                          0x3UL
#define LINE_STRIP                     0x4UL
#define EDGE_STRIP_R                   0x4UL
#define EDGE_STRIP_L                   0x6UL
#define EDGE_STRIP_A                   0x7UL
#define EDGE_STRIP_B                   0x8UL
#define RECTS                          0x9UL
#define NEVER                          0UL
#define LESS                           1UL
#define LEQUAL                         2UL
#define GREATER                        3UL
#define GEQUAL                         4UL
#define EQUAL                          5UL
#define NOTEQUAL                       6UL
#define ALWAYS                         7UL

#endif
