#ifndef AUDIO_FILE_HPP
#define AUDIO_FILE_HPP

#include <string>
#include "audio_port.hpp"

class Audio_file_base {
    public:
        Audio_file_base() {};
        virtual ~Audio_file_base() {};

        virtual int open(std::string path) = 0;
        virtual int close() = 0;
        virtual int read(std::uint8_t* pbuffer, std::uint32_t len) = 0;
        virtual int seek(std::int32_t position) = 0;

        Audio_port_base::au_port_format format;
        Audio_port_base::au_port_channel channel;
        Audio_port_base::au_port_sample_rate sample_rate;
};

#endif
