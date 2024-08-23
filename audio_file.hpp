#ifndef AUDIO_FILE_HPP
#define AUDIO_FILE_HPP

#include <list>
#include <string>
#include "audio_port.hpp"

class Audio_file_base {
    public:
        Audio_file_base() {};
        virtual ~Audio_file_base() {};

        using file_supported = std::list<std::string>;

        virtual int open(std::string path) = 0;
        virtual int close() = 0;
        virtual int read(std::uint8_t* pbuffer, std::uint32_t len) = 0;
        virtual int seek(std::int32_t position) = 0;
        virtual int get_meta() = 0;
        virtual file_supported get_file_supported_list() = 0;

        Audio_port_base::au_port_format format;
        Audio_port_base::au_port_channel channel;
        Audio_port_base::au_port_sample_rate sample_rate;

        struct Meta {
            std::string title;
            std::string artist;
            std::string album;
            std::string track_number;
            std::string genre;
            std::string date;
            std::string comment;
            std::string copyright;
            std::string license;
            std::string software;
        } meta;
};

#endif
