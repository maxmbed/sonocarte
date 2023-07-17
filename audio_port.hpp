#ifndef AUDIO_PORT_HPP
#define AUDIO_PORT_HPP

#include <cstdint>

class Audio_port_base {
    public:
        enum class au_port_format {
            pcm_none = 0,
            pcm_auto,
            pcm_s8,
            pcm_s16,
            pcm_s24,
            pcm_s32,

            pcm_u8,
            pcm_u16,
            pcm_u24,
            pcm_u32,

            pcm_float,
        };

        enum class au_port_sample_rate {
            rate_none = 0,
            rate_auto,
            rate_8k    = 8000u,
            rate_16k   = 16000u,
            rate_44k1  = 44100u,
            rate_48k   = 48000u,
            rate_88k2  = 88200u,
            rate_96k   = 96000u,
            rate_192k  = 192000u,
        };

        enum class au_port_channel {
            channel_none = 0,
            channel_auto,
            channel_mono   = 1u,
            channel_stereo = 2u,
        };

        au_port_format port_format;
        au_port_channel port_channels;
        au_port_sample_rate port_sample_rate; // in Hz

        std::uint32_t port_buffer_time; // in millisecond
        std::uint32_t port_bufsize;
        std::uint32_t port_chunksize;

        Audio_port_base() : port_channels(au_port_channel::channel_none), port_sample_rate(au_port_sample_rate::rate_none), port_buffer_time(0), port_format(au_port_format::pcm_none) { }
        virtual ~Audio_port_base() { };
        virtual int port_open() = 0;
        virtual int port_config(au_port_format format, au_port_sample_rate sample_rate, au_port_channel channel, std::uint32_t buffer_time) = 0;
        virtual int port_write(std::uint8_t* frame_ptr, std::uint32_t frame_count) = 0;
        virtual int port_close() = 0;
};

#endif
