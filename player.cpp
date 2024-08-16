#include <iostream>
#include "player.hpp"
#include "audio_file.hpp"

void Player::player_thread(Audio_port_base& audio, Audio_file_base& file) {

    std::cout << "player start " << std::this_thread::get_id() << std::endl;
    std::uint32_t chunk;
    std::uint8_t* buffer_ptr = nullptr;

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

end_thread:
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
