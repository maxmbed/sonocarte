#ifndef ALSA_PORT_HPP
#define ALSA_PORT_HPP

#include <alsa/asoundlib.h>
#include "audio_port.hpp"

class Alsa_port: public Audio_port_base {
    public:
        Alsa_port(const char* pcm_name);
        ~Alsa_port();
        int port_open();
        int port_config(au_port_format format, au_port_sample_rate sample_rate, au_port_channel channel, std::uint32_t buffer_time);
        int port_write(std::uint8_t* frame_ptr, std::uint32_t frame_count);
        int port_close();

    private:
        char* alsa_pcm_name = NULL;
        snd_pcm_t* alsa_pcm_handle = NULL;
        snd_pcm_hw_params_t* alsa_hwparam = NULL;
        snd_pcm_sw_params_t *alsa_swparams = NULL;
};

#endif
