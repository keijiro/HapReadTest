#include <stdio.h>
#include <stdlib.h>

#define MINIMP4_IMPLEMENTATION
#include "mp4demux.h"

#include "hap.h"

void hap_callback(HapDecodeWorkFunction function, void *p, unsigned int count, void *info)
{
}

static void print_mp4_info(const MP4D_demux_t * mp4_demux)
{
    unsigned i;

    printf("\nMP4 FILE: %d tracks found. Movie time %.2f sec\n", mp4_demux->track_count, (4294967296.0*mp4_demux->duration_hi + mp4_demux->duration_lo) / mp4_demux->timescale);
    printf("\nNo|type|lng| duration           | bitrate| %-23s| Object type","Stream type");
    for (i = 0; i < mp4_demux->track_count; i++)
    {
        MP4D_track_t * tr = mp4_demux->track + i;

        printf("\n%2d|%c%c%c%c|%c%c%c|%7.2f s %6d frm| %7d|", i,
            (tr->handler_type>>24), (tr->handler_type>>16), (tr->handler_type>>8), (tr->handler_type>>0),
            tr->language[0],tr->language[1],tr->language[2],
            (65536.0* 65536.0*tr->duration_hi + tr->duration_lo) / tr->timescale,
            tr->sample_count,
            tr->avg_bitrate_bps
            );

        printf(" %-23s|", MP4D__stream_type_to_ascii(tr->stream_type));
        printf(" %-23s", MP4D__object_type_to_ascii(tr->object_type_indication));

        if (tr->handler_type == MP4_HANDLER_TYPE_SOUN)
        {
            printf("  -  %d ch %d hz", tr->SampleDescription.audio.channelcount, tr->SampleDescription.audio.samplerate_hz);
        }
        else if (tr->handler_type == MP4_HANDLER_TYPE_VIDE)
        {
            printf("  -  %dx%d", tr->SampleDescription.video.width, tr->SampleDescription.video.height);
        }
    }
    printf("\n");
}

int main()
{
    FILE *file = fopen("hap.mov", "rb");

    MP4D_demux_t demux;
    MP4D__open(&demux, file);
    print_mp4_info(&demux);

    void *in_buffer = malloc(4 * 1024 * 1024);
    void *out_buffer = malloc(4 * 1024 * 1024);

    unsigned int frame_count = demux.track[0].sample_count;
    for (unsigned int i = 0; i < frame_count; i += frame_count / 10)
    {
        unsigned int in_size, timestamp, duration;
        mp4d_size_t in_offs = MP4D__frame_offset(&demux, 0, i, &in_size, &timestamp, &duration);

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
