#ifndef PLAYER_H
#define PLAYER_H

#include <thread>

#include "audio_port.hpp"
#include "song.hpp"

class Player {

    public:

        Player(Audio_port_base& audio_port) : audio(audio_port), thread_handler(player_thread, std::ref(audio_port)) { }

        int play(Song& song);
        int pause(Song& song);
        int stop(Song& song);
        int config(Song& song);

    private:
        Audio_port_base& audio;
        std::thread thread_handler;

        static void player_thread(Audio_port_base& audio);
};

#endif
