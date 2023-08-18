#ifndef SONG_HPP
#define SONG_HPP

#include <string>
#include "audio_file.hpp"

class Song : public Audio_file_base {

    public:

        Song() {}
        int track_number;
        std::string title;
        std::string artist;
        std::string album;
        std::string genre;
        std::string date;
        std::string comment;

        int open(Audio_file_base& file, std::string song_path) {

            return file.open(song_path);
        }

        int close(Audio_file_base& file) {

            return file.close();
        }
};

#endif