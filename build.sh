gcc -Wall -O2 -DHAVE_BUILTIN_CTZ \
    -Wno-sign-compare -Wno-unused-result -Wno-unused-function \
    -I mp4 -I hap -I snappy \
    test.c \
    mp4/mp4demux.c \
    hap/hap.c \
    snappy/snappy-c.cc \
    snappy/snappy-sinksource.cc \
    snappy/snappy-stubs-internal.cc \
    snappy/snappy.cc \
    -lstdc++
