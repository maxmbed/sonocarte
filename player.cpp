#include <iostream>
#include "player.hpp"

void Player::player_thread(Audio_port_base& audio) {

    while (1) {

        std::cout << "thread " << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
