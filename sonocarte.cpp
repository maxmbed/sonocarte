#include <iostream>
#include <list>
#include <string>
#include <filesystem>
#include <sndfile.hh>

#include "audio_file.hpp"
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

        Sonocarte(Audio_port_base& audio_port, Audio_file_base & audio_file) : audio(audio_port), file(audio_file) { }

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
            if (std::filesystem::exists(song_path) == false) {
                std::cout << "Sonocarte song path does not exist" << std::endl;
                return -1;
            }
            if (std::filesystem::is_directory(song_path)) {
                if (std::filesystem::is_empty(song_path)) {
                    std::cout << "song path directory empty" << std::endl;
                    return -1;
                }
                std::cout << "gather all audio file from directory" << std::endl;
                for (auto const& dir_entry : std::filesystem::directory_iterator{song_path}) {
                    std::string tmp_str = dir_entry.path();
                    if (tmp_str.find("wav") != std::string::npos) {
                        song_list.push_back(dir_entry.path());
                    }
                }
            }
            else if (std::filesystem::is_regular_file(song_path)) {
                song_list.push_back(song_path);
            }
            else {
                std::cout << "song path not a file or directory" << std::endl;
                return -1;
            }
            return 0;
        }

        int song_close() {
            file.close();
            return 0;
        }

        int song_play() {
            Player player(audio, file);
            player.play(song_list);
            return 0;
        }

    private:
        Audio_port_base& audio;
        Audio_file_base& file;
        std::list<std::string> song_list;
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

    sonocarte.song_play();
    sonocarte.song_close();
    std::cout << "Sonocarte demo end" << std::endl;

    return 0;
}
