
#include <iostream>
#include "alsa_port.hpp"

Alsa_port::Alsa_port(const char* pcm_name) {
    this->alsa_pcm_name = (char*)pcm_name;
}

Alsa_port::Alsa_port(const char* pcm_name, std::uint16_t channels, std::uint32_t sample_rate, std::uint32_t buffer_time, au_port_format format) {

    this->alsa_pcm_name = (char*)pcm_name;
    this->port_channels = channels;
    this->port_sample_rate = sample_rate;
    this->port_buffer_time = buffer_time;
    this->port_format = format;
}

Alsa_port::~Alsa_port() {

    this->port_close();
}

int Alsa_port::port_open() {
    int ret;

    ret = snd_pcm_open(&this->alsa_pcm_handle, this->alsa_pcm_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
    if(ret < 0) {

        std::cout << "open " << this->alsa_pcm_name << " " << snd_strerror(ret) << std::endl;
        return ret;
    }

    snd_pcm_sw_params_malloc(&this->alsa_swparams);
    if (this->alsa_swparams == NULL) {

        std::cout << "error to allocate pcm sw param " << std::endl;
        return -1;
    }

    snd_pcm_hw_params_malloc(&this->alsa_hwparam);
    if (this->alsa_hwparam == NULL) {

        std::cout << "error to allocate pcm hw param " << std::endl;
        return -1;
    }

    ret = snd_pcm_hw_params_any(this->alsa_pcm_handle, this->alsa_hwparam);
    if (ret < 0) {

        std::cout << "no configurations available: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    ret = snd_pcm_hw_params_set_access(this->alsa_pcm_handle, this->alsa_hwparam, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (ret < 0) {

        std::cout << "set interleaved mode: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    switch (this->port_format) {

        case Alsa_port::au_port_format::pcm_s16:
            this->alsa_format = SND_PCM_FORMAT_S16;
            break;

        case Alsa_port::au_port_format::pcm_s24:
            this->alsa_format = SND_PCM_FORMAT_S24;
            break;

        default:
            std::cout << "port format not supported" << std::endl;
            return -1;
    }

    ret = snd_pcm_hw_params_set_format(this->alsa_pcm_handle, this->alsa_hwparam, this->alsa_format);
    if (ret < 0) {

        std::cout << "set format: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    ret = snd_pcm_hw_params_set_channels(this->alsa_pcm_handle, this->alsa_hwparam, this->port_channels);
    if (ret < 0) {

        std::cout << "set channels number: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    unsigned int sample_rate = this->port_sample_rate;
    ret = snd_pcm_hw_params_set_rate_near(this->alsa_pcm_handle, this->alsa_hwparam, &sample_rate, 0);
    if (ret < 0) {

        std::cout << "set rate: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    if (sample_rate != this->port_sample_rate) {

        std::cout << "cannot set sample rate " << this->port_sample_rate << "Hz" << std::endl;
        std::cout << "use nearest rate " << sample_rate << "Hz" << std::endl;
    }


    unsigned int buffer_time_calc = this->port_buffer_time * 1000; // Alsa buffer time must be microsecond
    unsigned int buffer_time_near = buffer_time_calc;

    ret = snd_pcm_hw_params_set_buffer_time_near(this->alsa_pcm_handle, this->alsa_hwparam, &buffer_time_near, 0);
    if (ret < 0) {

        std::cout << "set buffer time: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    if (buffer_time_near != buffer_time_calc) {

        if (buffer_time_near == 0) {
            std::cout << "error null buffer time" << std::endl;
            return -1;
        }
        std::cout << "cannot set buffer time " << buffer_time_calc << std::endl;
        std::cout << "use nearest time " << buffer_time_near << std::endl;
    }

    unsigned int period_time_calc = (this->port_buffer_time*1000)/4;
    unsigned int period_time_near = period_time_calc;

    ret = snd_pcm_hw_params_set_period_time_near(this->alsa_pcm_handle, this->alsa_hwparam, &period_time_near, 0);
    if (ret < 0) {

        std::cout << "set period time: " << snd_strerror(ret) << std::endl;
        return ret;
    }
    if (period_time_near != period_time_calc) {

        if (period_time_near == 0) {
            std::cout << "error null period time" << std::endl;
            return -1;
        }
        std::cout << "cannot set period time " << period_time_calc << std::endl;
        std::cout << "use nearest time " << period_time_near << std::endl;
    }

    ret = snd_pcm_hw_params(this->alsa_pcm_handle, this->alsa_hwparam);
    if (ret < 0) {

        std::cout << " set hardware parameter: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    ret = snd_pcm_sw_params_current(this->alsa_pcm_handle, this->alsa_swparams);
    if (ret < 0) {
        std::cout << "unable to get current swp param: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    snd_pcm_hw_params_get_period_size(this->alsa_hwparam, &this->chunk_size, 0);
    snd_pcm_hw_params_get_buffer_size(this->alsa_hwparam, &this->buffer_size);

    if (this->chunk_size == this->buffer_size) {
        std::cout << "Can't use period equal to buffer size" << std::endl;
        return -1;
    }

    snd_pcm_sw_params_set_avail_min(this->alsa_pcm_handle, this->alsa_swparams, this->chunk_size);

    ret = snd_pcm_sw_params(this->alsa_pcm_handle, this->alsa_swparams);
    if (ret < 0) {
        std::cout << "set software parameter: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    snd_output_t* log;

    snd_output_stdio_attach(&log, stderr, 0);
    snd_pcm_dump(this->alsa_pcm_handle, log);
    snd_output_close(log);

    this->port_bufsize = this->buffer_size;
    this->port_chunksize = this->chunk_size;

    return 0;
}

int Alsa_port::port_close() {

    if (this->alsa_hwparam != NULL) {
        snd_pcm_hw_params_free(this->alsa_hwparam);
        this->alsa_hwparam = NULL;
    }

    if (this->alsa_swparams != NULL) {
        snd_pcm_sw_params_free(this->alsa_swparams);
        this->alsa_swparams = NULL;
    }

    if (this->alsa_pcm_handle != NULL) {

        snd_pcm_drain(this->alsa_pcm_handle);
        snd_pcm_close(this->alsa_pcm_handle);
    }
    return 0;
}

int Alsa_port::port_write(std::uint8_t* frame_ptr, std::uint32_t frame_count) {
    int ret = 0;
    snd_pcm_state_t pcm_state;

    if (this->alsa_pcm_handle == NULL) {

        std::cout <<"pcm device handle null" << std::endl;
        return -1;
    }

    pcm_state = snd_pcm_state(this->alsa_pcm_handle);

    switch (pcm_state) {

        case SND_PCM_STATE_RUNNING:
            ret = snd_pcm_wait(this->alsa_pcm_handle, -1);
            if (ret < 0) {
                std::cout << "wait pcm device: " << snd_strerror(ret) << std::endl;
                return -1;
            }
            break;

        case SND_PCM_STATE_XRUN:
            ret = snd_pcm_drop(this->alsa_pcm_handle);
            if (ret < 0) {
                std::cout << "drop pcm pending samples: " << snd_strerror(ret) << std::endl;
            }
            snd_pcm_prepare(this->alsa_pcm_handle);
            break;
    }

    ret = snd_pcm_writei(this->alsa_pcm_handle, reinterpret_cast<const void*>(frame_ptr), static_cast<snd_pcm_uframes_t>(frame_count));
    if (ret == -EPIPE) {

        std::cout << "write pcm device: over/under run" << std::endl;
    }
    else if (ret < 0) {

        std::cout << "write pcm device: " << snd_strerror(ret) << std::endl;
        snd_pcm_recover(this->alsa_pcm_handle, ret, 0);
    }

    return 0;
}
