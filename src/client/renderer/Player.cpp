#include "Player.h"

using namespace mc::client;

Player::Player(float fovDegree, float aspect, float nearPlane, float farPlane)
    : camera_{fovDegree, aspect, nearPlane, farPlane} {
}

void Player::selectNextSlot() { current_slot_ = (current_slot_ + 1) % hotbar_.size(); }
void Player::selectPrevSlot() { current_slot_ = (current_slot_ + hotbar_.size() - 1) % hotbar_.size(); }
void Player::selectSlot(int index) { if (index >= 0 && index < hotbar_.size()) current_slot_ = index; }
