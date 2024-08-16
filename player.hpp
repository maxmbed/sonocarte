#ifndef PLAYER_H
#define PLAYER_H

#include <thread>

#include "audio_file.hpp"
#include "audio_port.hpp"

class Player {

    public:

        Player(Audio_port_base& audio_port, Audio_file_base& audio_file) : audio(audio_port), file(audio_file) {
            thread_handler = std::thread(player_thread, std::ref(audio_port), std::ref(audio_file));
            thread_handler.join(); // For the moment, wait player thread plays until the end of audio file
        }

        int play();
        int pause();
        int stop();
        int config();

    private:
        Audio_port_base& audio;
        Audio_file_base& file;
        std::thread thread_handler;

        static void player_thread(Audio_port_base& audio, Audio_file_base& file);
};

#endif
