#ifndef SNDFILE_PORT_HPP
#define SNDFILE_PORT_HPP

#include <sndfile.hh>

#include "audio_file.hpp"

class Sndfile_port : public Audio_file_base {
    public:
        Sndfile_port() {};
        ~Sndfile_port();

        int open(std::string path);
        int close();
        int read(std::uint8_t* pbuffer, std::uint32_t len);
        int seek(std::int32_t position);
        int get_meta();
        file_supported get_file_supported_list();

    private:
        SndfileHandle file;
};

#endif
