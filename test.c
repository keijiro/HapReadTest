#define MINIMP4_IMPLEMENTATION
#include "minimp4.h"

#include "hap.h"

void hap_callback(HapDecodeWorkFunction function, void *p, unsigned int count, void *info)
{
}

int main()
{
    FILE* file = fopen("hap.mov", "rb");

    MP4D_demux_t demux;
    MP4D__open(&demux, file);

    unsigned int length, timestamp, duration;
    MP4D_file_offset_t offs = MP4D__frame_offset(&demux, 0, 0, &length, &timestamp, &duration);

    void* in_buffer = malloc(length);
    void* out_buffer = malloc(1024 * 1024);

    fseek(file, offs, SEEK_SET);
    fread(in_buffer, length, 1, file);

    unsigned long used;
    unsigned int format;
    unsigned int res = HapDecode(in_buffer, length, 0, hap_callback, NULL, out_buffer, 1024 * 1024, &used, &format);

    printf("res:%d format:%x size:%ld\n", res, format, used);

    free(in_buffer);
    free(out_buffer);
    fclose(file);
    return 0;
}
