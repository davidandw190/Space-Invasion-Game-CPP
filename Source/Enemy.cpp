//
// Created by Gabriel on 5/24/2023.
//

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <random>
#include <chrono>

#include "Headers/Enemy.hpp"
#include "Headers/Global.hpp"

Enemy::Enemy(unsigned char type, unsigned short x, unsigned short y)
    :Entity(),
     direction(0 == (y / BASE_SIZE) % 2 ? -1 : 1),
     health(1 + type),
     hit_timer(0),
     hit_points(0),
     enemy_type(type)
{
    this->x = x;
    this->y = y;
}

unsigned char Enemy::get_hit_points() const {
    return hit_points;
}

unsigned char Enemy::get_hit_timer() const {
    return hit_timer;
}

unsigned short Enemy::get_enemy_type() const {
    return enemy_type;
}


void Enemy::hit() {
    hit_timer = ENEMY_HIT_TIMER_DURATION;
}

void Enemy::update() {
        if (hit_timer == 1)
            hit_points = std::max(0, hit_points - 1);
        hit_timer--;
}

void Enemy::move() {
    if (0 != direction)
    {
        if ((1 == direction && x == SCREEN_WIDTH - 2 * BASE_SIZE) || (-1 == direction && x == BASE_SIZE))
        {
            //Moving downwards after reaching the edge
            direction = 0;

            y += ENEMY_MOVE_SPEED;
        }
        else
            //Moving left and right
            x = std::clamp<short>(x + ENEMY_MOVE_SPEED * direction, BASE_SIZE, SCREEN_WIDTH - 2 * BASE_SIZE);
    }
    else
    {
        y = std::min<short>(y + ENEMY_MOVE_SPEED, BASE_SIZE * ceil(y / static_cast<float>(BASE_SIZE)));

        if (y == BASE_SIZE * ceil(y / static_cast<float>(BASE_SIZE)))
            //Checking which direction the enemy should move in
            direction = (y / BASE_SIZE) % 2 == 0 ? -1 : 1;
    }
}

sf::IntRect Enemy::get_hitbox() const {
    return sf::IntRect(x + 0.25f * BASE_SIZE, y + 0.25f * BASE_SIZE, 0.5f * BASE_SIZE, 0.5f * BASE_SIZE);
}

void Enemy::shoot(std::vector<Bullet>& enemy_bullets) {
    enemy_bullets.push_back(Bullet(0, ENEMY_BULLET_SPEED, x, y));
}
