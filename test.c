#define MINIMP4_IMPLEMENTATION
#include "minimp4.h"

#include "hap.h"

void hap_callback(HapDecodeWorkFunction function, void *p, unsigned int count, void *info)
{
}

int main()
{
    FILE *file = fopen("hap.mov", "rb");

    MP4D_demux_t demux;
    MP4D__open(&demux, file);

    void *in_buffer = malloc(4 * 1024 * 1024);
    void *out_buffer = malloc(4 * 1024 * 1024);

    printf("width:%d height:%d\n",
        demux.track[0].SampleDescription.video.width,
        demux.track[0].SampleDescription.video.height);

    for (unsigned int i = 0; i < demux.track[0].sample_count; i++)
    {
        unsigned int in_size, timestamp, duration;
        MP4D_file_offset_t in_offs = MP4D__frame_offset(&demux, 0, i, &in_size, &timestamp, &duration);

        fseek(file, in_offs, SEEK_SET);
        fread(in_buffer, in_size, 1, file);

        unsigned long used;
        unsigned int format;
        unsigned int res = HapDecode(in_buffer, in_size, 0, hap_callback, NULL, out_buffer, 1024 * 1024, &used, &format);

        printf("frm:%d res:%d format:%x size:%ld\n", i, res, format, used);
    }

    free(in_buffer);
    free(out_buffer);
    fclose(file);

    return 0;
}
