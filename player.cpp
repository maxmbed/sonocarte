#include <iostream>
#include <format>
#include <thread>
#include "player.hpp"
#include "audio_file.hpp"

void Player::player_thread(Audio_port_base& audio, Audio_file_base& file, std::list<std::string>& file_list) {

    std::uint32_t chunk;
    std::uint8_t* buffer_ptr = nullptr;

    for (auto f : file_list) {
        if (f.empty()) {
            std::cout << "Player cannot read empty file" << std::endl;
            goto end_thread;
        }
        if (file.open(f)) {
            std::cout << std::format("Player fails to open file {}", f) << std::endl;
            goto end_thread;
        }
        std::cout << std::format("Player starts {}", file.meta.title) << std::endl;

        audio.port_config(file.format, file.sample_rate, file.channel, 100);

        buffer_ptr = static_cast<std::uint8_t*>(operator new(audio.port_bufsize, std::nothrow));
        if (buffer_ptr == NULL) {
            std::cout << "error cannot allocate audio buffer" << std::endl;
            goto end_thread;
        }
        do {
            chunk = file.read(buffer_ptr, audio.port_chunksize);
            if (chunk >= 0) {
                audio.port_write((std::uint8_t*)buffer_ptr, chunk);
            }
        } while (chunk == audio.port_chunksize);

        operator delete(static_cast<void*>(buffer_ptr));
    }

end_thread:
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
