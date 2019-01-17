#include <stdio.h>
#include <stdlib.h>
#include "mp4demux.h"
#include "hap.h"

static void hap_callback
    (HapDecodeWorkFunction function, void *p, unsigned int count, void *info)
{
    fputs("Threading callback is not implemented.", stderr);
    exit(-1);
}

static double calc_time(uint32_t hi, uint32_t lo, uint32_t scale)
{
    return (double)(((uint64_t)hi << 32) + lo) / scale;
}

static const char *str_fourcc(uint32_t code)
{
    static char str[5];
    str[0] = code >> 24;
    str[1] = code >> 16;
    str[2] = code >> 8;
    str[3] = code;
    return str;
}

int main()
{
    FILE *file = fopen("test.mov", "rb");

    MP4D_demux_t demux;
    MP4D__open(&demux, file);

    void *in_buffer = malloc(4 * 1024 * 1024);
    void *out_buffer = malloc(4 * 1024 * 1024);

    for (int track_index = 0; track_index < demux.track_count; track_index++)
    {
        const MP4D_track_t *track = &demux.track[track_index];

        printf("Track %d '%s' %.2fs %d frames",
               track_index,
               str_fourcc(track->handler_type),
               calc_time(track->duration_hi,
                         track->duration_lo,
                         track->timescale),
               track->sample_count);

        if (track->handler_type != MP4_HANDLER_TYPE_VIDE)
        {
            puts("");
            continue;
        }

        printf(" %dx%d\n",
               track->SampleDescription.video.width,
               track->SampleDescription.video.height);

        for (unsigned int sample_index = 0;
             sample_index < track->sample_count;
             sample_index += track->sample_count / 10)
        {
            unsigned int in_size, timestamp, duration;
            mp4d_size_t in_offs = MP4D__frame_offset
                (&demux, 0, sample_index, &in_size, &timestamp, &duration);

            fseek(file, in_offs, SEEK_SET);
            fread(in_buffer, in_size, 1, file);

            unsigned long used;
            unsigned int format;
            unsigned int res = HapDecode(in_buffer, in_size, 0,
                                         hap_callback, NULL,
                                         out_buffer, 1024 * 1024,
                                         &used, &format);

            printf("Frame %d Result:%d Format:%x Size:%ld\n",
                   sample_index, res, format, used);
        }
    }

    free(in_buffer);
    free(out_buffer);
    fclose(file);

    return 0;
}
