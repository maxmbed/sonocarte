#include <iostream>
#include <string>
#include <sndfile.hh>

#include "sndfile_port.hpp"
#include "alsa_port.hpp"
#include "player.hpp"

class Sonocarte {
    public:

        enum class command_playback {
            undef = 0,
            new_song,
            play,
            pause,
            stop,
            next,
            previous,
            forward,
            backward,
        };
        enum class command_system {
            undef = 0,
            reset,
            shutdown,
        };
        enum class error_type {
            success = 0,
            error_undef,
        };

        Sonocarte(Audio_port_base& audio_port, Audio_file_base & audio_file) : audio(audio_port), file(audio_file) {}

        int audio_open(void) {

            return audio.port_open();
        }

        int audio_config() {
            audio.port_config(file.format, file.sample_rate, file.channel, 100);
            return 0;
        }

        int audio_close(void) {
            return audio.port_close();
        }

        int audio_write(uint8_t *frame_ptr, uint32_t frame_count) {
            return audio.port_write(frame_ptr, frame_count);
        }

        int song_open(std::string song_path) {
            file.open(song_path);
            return 0;
        }

        int song_close() {
            file.close();
            return 0;
        }

        int song_play() {
            Player player(audio, file);
            return 0;
        }

    private:
        Audio_port_base& audio;
        Audio_file_base& file;
};

int main(int argc, char* argv[]) {

    std::cout << "Sonocarte demo" << std::endl;

    if (argc < 2) {
        std::cout <<  "error not enough argument" << std::endl;
        std::exit(static_cast<int>(std::errc::invalid_argument));
    }


    Alsa_port alsa_port("pipewire");
    Sndfile_port file_port;

    Sonocarte sonocarte(alsa_port, file_port);

    std::string song(argv[1]);

    if (sonocarte.song_open(song) != 0) {
        return -1;
    }

    if (sonocarte.audio_open() != 0) {
        std::cout << "error open audio" << std::endl;
        return -1;
    }

    if (sonocarte.audio_config() != 0) {
        std::cout << "error config audio" << std::endl;
        return -1;
    }

    sonocarte.song_play();
    sonocarte.song_close();
    std::cout << "Sonocarte demo end" << std::endl;

    return 0;
}
