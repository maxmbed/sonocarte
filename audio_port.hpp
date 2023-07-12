#include <cstdint>

class Audio_port_base {
    public:
        enum class au_port_format {
            none = 0,
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

        enum class channel {
            channel_none = 0,
            channel_auto,
            channel_mono   = 1u,
            channel_stereo = 2u,
        };

        std::uint16_t port_channels;
        std::uint32_t port_sample_rate; // in Hz
        std::uint32_t port_buffer_time; // in millisecond
        au_port_format port_format;

        Audio_port_base() : port_channels(0), port_sample_rate(0), port_buffer_time(0), port_format(au_port_format::pcm_s16) { }
        virtual ~Audio_port_base() { };
        virtual int port_open() = 0;
        virtual int port_write(std::uint8_t* frame_ptr, std::uint32_t frame_count) = 0;
        virtual int port_close() = 0;
};
