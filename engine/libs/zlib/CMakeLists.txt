# zlib 1.2.8 (http://zlib.net/)

add_library(
    z

    STATIC

    adler32.c
    compress.c
    crc32.c
    deflate.c
    infback.c
    inffast.c
    inflate.c
    inftrees.c
    trees.c
    uncompr.c
    zutil.c
)
