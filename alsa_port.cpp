#include <iostream>
#include <format>
#include "alsa_port.hpp"


static void _list_avail_dev(const char *devname) {
    char** hints;
    int    err;
    char** n;
    char*  name;
    char*  desc;
    char*  ioid;
    constexpr int showallcards = -1;

    std::cout << "List of available " << devname << std::endl;
    /* Enumerate sound devices */
    err = snd_device_name_hint(showallcards, devname, (void***)&hints);
    if (err != 0) {
      std::cout << "Cannot get device names" << std::endl;
        exit(1);
    }
    n = hints;
    while (*n != NULL) {
        name = snd_device_name_get_hint(*n, "NAME");
        desc = snd_device_name_get_hint(*n, "DESC");
        ioid = snd_device_name_get_hint(*n, "IOID");

        std::cout << std::format("\tDevices: {}\n", name != NULL ? name: "none");
        std::cout << std::format("\tDescription: {}\n", desc != NULL ? desc : "none");
        std::cout << std::format("\tI/O type: {}\n", ioid != NULL ? ioid : "none");
        std::cout << std::endl;

        if (name && strcmp("null", name)) free(name);
        if (desc && strcmp("null", desc)) free(desc);
        if (ioid && strcmp("null", ioid)) free(ioid);
        n++;
    }
    //Free hint buffer too
    snd_device_name_free_hint((void**)hints);
}

Alsa_port::Alsa_port(const char* pcm_name) {
    this->alsa_pcm_name = (char*)pcm_name;
}

Alsa_port::~Alsa_port() {

    this->port_close();
}

int Alsa_port::port_open() {
    int ret;

    ret = snd_pcm_open(&this->alsa_pcm_handle, this->alsa_pcm_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
    if(ret < 0) {

        std::cout << "open " << this->alsa_pcm_name << " " << snd_strerror(ret) << std::endl;
        _list_avail_dev("pcm");
        return ret;
    }
    return 0;
}
int Alsa_port::port_config(au_port_format format, au_port_sample_rate sample_rate, au_port_channel channel, std::uint32_t buffer_time) {
    int ret;

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

    snd_pcm_format_t alsa_format;
    switch (format) {

        case Alsa_port::au_port_format::pcm_s16: alsa_format = SND_PCM_FORMAT_S16; break;
        case Alsa_port::au_port_format::pcm_s24: alsa_format = SND_PCM_FORMAT_S24; break;
        default:
            std::cout << "port format not supported" << std::endl;
            return -1;
    }

    ret = snd_pcm_hw_params_set_format(this->alsa_pcm_handle, this->alsa_hwparam, alsa_format);
    if (ret < 0) {

        std::cout << "set format: " << snd_strerror(ret) << std::endl;
        return ret;
    }
    else {
        this->port_format = format;
    }


    unsigned int alsa_channel;
    switch (channel) {
        case Alsa_port::au_port_channel::channel_mono:   alsa_channel = 1U; break;
        case Alsa_port::au_port_channel::channel_stereo: alsa_channel = 2U; break;
        default:
            std::cout << "port channel not supported" << std::endl;
            return -1;
    }
    ret = snd_pcm_hw_params_set_channels(this->alsa_pcm_handle, this->alsa_hwparam, alsa_channel);
    if (ret < 0) {

        std::cout << "set channels number: " << snd_strerror(ret) << std::endl;
        return ret;
    }
    else {
        this->port_channels = channel;
    }

    unsigned int alsa_sample_rate, alsa_sample_rate_near;
    switch (sample_rate) {
        case Audio_port_base::au_port_sample_rate::rate_8k:   alsa_sample_rate = 8000U;  break;
        case Audio_port_base::au_port_sample_rate::rate_16k:  alsa_sample_rate = 16000U; break;
        case Audio_port_base::au_port_sample_rate::rate_44k1: alsa_sample_rate = 44100U; break;
        case Audio_port_base::au_port_sample_rate::rate_48k:  alsa_sample_rate = 48000U; break;
        case Audio_port_base::au_port_sample_rate::rate_88k2: alsa_sample_rate = 88200U; break;
        case Audio_port_base::au_port_sample_rate::rate_96k:  alsa_sample_rate = 96000U; break;
        case Audio_port_base::au_port_sample_rate::rate_192k: alsa_sample_rate = 192000U; break;
        default:
            std::cout << "port sample rate not supported" << std::endl;
            return -1;
    }

    alsa_sample_rate_near = alsa_sample_rate;
    ret = snd_pcm_hw_params_set_rate_near(this->alsa_pcm_handle, this->alsa_hwparam, &alsa_sample_rate_near, 0);
    if (ret < 0) {

        std::cout << "set rate: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    if (alsa_sample_rate != alsa_sample_rate_near) {

        std::cout << "cannot set sample rate " << alsa_sample_rate << "Hz" << std::endl;
        std::cout << "use nearest rate " << alsa_sample_rate_near << "Hz" << std::endl;
    }
    else {
        this->port_sample_rate = sample_rate;
    }


    unsigned int buffer_time_calc = buffer_time * 1000; // Alsa buffer time must be microsecond
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

    unsigned int period_time_calc = (buffer_time*1000)/4; // Rule of thumb from ALSA example were the period should be 4 time less than the buffer time
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
    else {
        this->port_buffer_time = buffer_time;
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

    snd_pcm_uframes_t chunk_size = 0;
    snd_pcm_uframes_t buffer_size = 0;

    ret = snd_pcm_hw_params_get_period_size(this->alsa_hwparam, &chunk_size, 0);
    if (ret != 0) {
        std::cout << "unable to get period size: " << snd_strerror(ret) << std::endl;
        return ret;
    }
    snd_pcm_hw_params_get_buffer_size(this->alsa_hwparam, &buffer_size);
    if (ret != 0) {
        std::cout << "unable to get buffer size: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    if (chunk_size == buffer_size) {
        std::cout << "can't use period equal to buffer size" << std::endl;
        return -1;
    }
    else {
        this->port_bufsize = buffer_size;
        this->port_chunksize = chunk_size;
    }

    ret = snd_pcm_sw_params_set_avail_min(this->alsa_pcm_handle, this->alsa_swparams, chunk_size);
    if (ret != 0) {
        std::cout << "unable to set sw min chunk: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    ret = snd_pcm_sw_params(this->alsa_pcm_handle, this->alsa_swparams);
    if (ret < 0) {
        std::cout << "set software parameter: " << snd_strerror(ret) << std::endl;
        return ret;
    }

    snd_output_t* log;

    snd_output_stdio_attach(&log, stderr, 0);
    snd_pcm_dump(this->alsa_pcm_handle, log);
    snd_output_close(log);

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

        case SND_PCM_STATE_SETUP:
            snd_pcm_prepare(this->alsa_pcm_handle);
            break;

        default:
            std::cout << "pcm state " << snd_pcm_state(this->alsa_pcm_handle) << std::endl;
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

int Alsa_port::port_idle() {
    snd_pcm_drain(this->alsa_pcm_handle);
    return 0;
}
