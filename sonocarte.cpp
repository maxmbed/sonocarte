#include <iostream>
#include <string>
#include <sndfile.hh>

#include "alsa_port.hpp"


class Sonocarte {
    public:
        SndfileHandle song;

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

        Sonocarte(Audio_port_base& audio_port) : audio(audio_port) {}

        int audio_open(void) {
            audio.port_buffer_time = 100;
            audio.port_channels = song.channels();
            audio.port_sample_rate = song.samplerate();
            audio.port_format = Audio_port_base::au_port_format::pcm_s16;

            return audio.port_open();
        }

        int audio_close(void) {
            return audio.port_close();
        }

        int audio_write(uint8_t *frame_ptr, uint32_t frame_count) {
            return audio.port_write(frame_ptr, frame_count);
        }

        int song_open(std::string song_path) {

            song = SndfileHandle(song_path);

            if (song.error() == 0) {

                std::cout << song << std::endl;
                std::cout << " channels: " << song.channels() << std::endl;
                std::cout << " frames: "  << song.frames() << std::endl;
                std::cout << " format: " << song.format() << std::endl;
                std::cout << " sample rate:" << song.samplerate() << std::endl;
            }
            else {
                std::cout << "error: " << song.strError() << std::endl;
                return -1;
            }

            return 0;
        }

        int song_close() {

           ~SndfileHandle();
           return 0;
        }

        int song_play() {
            std::uint32_t chunk;

            buffer_ptr = static_cast<std::int8_t*>(operator new(audio.port_bufsize, std::nothrow));
            if (buffer_ptr == NULL) {

                std::cout << "error cannot allocate audio buffer" << std::endl; 
                return -1;
            }

            do {
                chunk = song.readf((short*)buffer_ptr, audio.port_chunksize);

                if (chunk >= 0) {
                    audio_write((std::uint8_t*)buffer_ptr, chunk);
                }

            } while (chunk == audio.port_chunksize);

            operator delete(static_cast<void*>(buffer_ptr));

            return 0;
        }

    private:
        Audio_port_base& audio;
        std::int8_t* buffer_ptr;

};

int main(int argc, char* argv[]) {

    std::cout << "Sonocarte demo" << std::endl;

    if (argc < 2) {
        std::cout <<  "error not enough argument" << std::endl;
        std::exit(static_cast<int>(std::errc::invalid_argument));
    }

    Alsa_port alsa_port_pa("pulse");
    Sonocarte sonocarte(alsa_port_pa);

    std::string song(argv[1]);

    if (sonocarte.song_open(song) != 0) {
        return -1;
    }

    if (sonocarte.audio_open() != 0) {
        std::cout << "error open audio" << std::endl;
        return -1;
    }

    sonocarte.song_play();

    return 0;
}
