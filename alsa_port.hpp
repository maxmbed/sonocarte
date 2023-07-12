#include <alsa/asoundlib.h>
#include "audio_port.hpp"

class Alsa_port: public Audio_port_base {
    public:
        Alsa_port(const char* pcm_name);
        Alsa_port(const char* pcm_name, std::uint16_t channels, std::uint32_t sample_rate, std::uint32_t buffer_time, au_port_format format);
        ~Alsa_port();
        int port_open();
        int port_write(std::uint8_t* frame_ptr, std::uint32_t frame_count);
        int port_close();

    private:
        char* alsa_pcm_name = NULL;
        snd_pcm_t* alsa_pcm_handle = NULL;
        snd_pcm_hw_params_t* alsa_hwparam = NULL;
        snd_pcm_sw_params_t *alsa_swparams = NULL;
        snd_pcm_format_t alsa_format = SND_PCM_FORMAT_UNKNOWN;

        snd_pcm_uframes_t chunk_size = 0;
        snd_pcm_uframes_t buffer_size = 0;
};
