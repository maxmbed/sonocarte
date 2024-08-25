#ifndef PLAYER_H
#define PLAYER_H

#include <thread>
#include <list>
#include "audio_file.hpp"
#include "audio_port.hpp"

class Player {

    public:
        Player(Audio_port_base& audio_port, Audio_file_base& audio_file) : audio(audio_port), file(audio_file) {}
        Player operator =(Player);

        int play(std::list<std::string>& file_list) {
            thread_handler = std::thread(player_thread, std::ref(audio), std::ref(file), std::ref(file_list));
            thread_handler.join(); // For the moment, wait player thread plays until the end of audio file
            return 0;
        }
        int pause();
        int stop();
        int config();

    private:
        Audio_port_base& audio;
        Audio_file_base& file;
        std::thread thread_handler;

        static void player_thread(Audio_port_base& audio, Audio_file_base& file, std::list<std::string>& file_list);
};

#endif
