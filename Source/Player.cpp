//
// Created by David on 5/20/2023.
//

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <random>

#include "Headers/Player.hpp"
#include "Headers/Global.hpp"
#include "Headers/Bullet.hpp"
#include "Headers/Enemy.hpp"


Player::Player() : Entity(), explosion(EXPLOSION_ANIMATION_SPEED, BASE_SIZE,
                                       R"(C:\Users\40787\Desktop\PP-SPACE-INVASION\Source\Resources\Explosion.png)") {
    reset();    // for when we transition to a new level etc. , because we will call again the constructor

    current_power = 0;
    reload_timer = 0;

    texture.loadFromFile(R"(C:\Users\40787\Desktop\PP-SPACE-INVASION\Source\Resources\Test-Player.png)");
    bullet_texture.loadFromFile(R"(C:\Users\40787\Desktop\PP-SPACE-INVASION\Source\Resources\Player-Bullet.png)");

    sprite.setTexture(texture);
    bullet_sprite.setTexture(bullet_texture);

}

void Player::reset() {

    this->current_power = 0;
    this->reload_timer = 0;
    this->dead_animation_over = false;

    this->dead = false;
    this->x = 0.5f * (SCREEN_WIDTH - BASE_SIZE);  // x and y position the player
    this->y = SCREEN_HEIGHT - 2 * BASE_SIZE;

    this->player_bullets.clear();

    this->explosion.reset();

}


void Player::die() {
    this->dead = true;
}

void Player::draw(sf::RenderWindow& window) {
    if (!dead) {
        // here we basically iterate over the player texture according to `the current_power` offset
        sprite.setTextureRect(
                sf::IntRect(BASE_SIZE * current_power, 0, BASE_SIZE, BASE_SIZE));

        // I set the position of the player's sprite as a vector since it does not accept actulat ints
        sprite.setPosition(sf::Vector2f(x, y));

        for (const Bullet& bullet : player_bullets) {
            bullet_sprite.setPosition(bullet.x, bullet.y);

            window.draw(bullet_sprite);
        }

        window.draw(sprite);

        if (!shield_animation_over) {
            // the sheild beign destroyed.. we'll show a blue explosion.
            explosion.draw(x, y, window, sf::Color(0, 109, 255));
        }


    } else if (!dead_animation_over) {

        explosion.draw(x, y, window, sf::Color(255, 36, 0));
    }
}


sf::IntRect Player::get_hitbox() const {
    return sf::IntRect(
            x + 0.125f * BASE_SIZE,
            y + 0.125f * BASE_SIZE,
            0.75f * BASE_SIZE,
            0.75f * BASE_SIZE
            );

}

void Player::update(std::mt19937_64& random_engine, std::vector<Bullet>& enemy_bullets, std::vector<Enemy>& enemies) {
    if (!dead) {

        unsigned char powerup_type;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            current_power = 0;

        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            current_power = 1;

        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            current_power = 2;

        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            current_power = 3;
        }

        // move left
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {

            if (current_power == 1) {
                // we mirror  the controls
                x = std::min<int>(PLAYER_MOVE_SPEED + x, SCREEN_WIDTH - 2 * BASE_SIZE);
            } else {
                x = std::max<int>(x - PLAYER_MOVE_SPEED, BASE_SIZE);
            }

        }

        // move right
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {

            if (current_power == 1) {
                // we ALSO mirror the controls
                x = std::max<int>(x - PLAYER_MOVE_SPEED, BASE_SIZE);
            } else {
                x = std::min<int>(PLAYER_MOVE_SPEED + x, SCREEN_WIDTH - 2 * BASE_SIZE);
            }
        }

        // player shoot
        if (reload_timer == 0) {

            if ((current_power != 1) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))) {

                if (current_power == 2) {
                    reload_timer = FAST_RELOAD_DURATION;

                } else {
                    reload_timer = RELOAD_DURATION;
                }

                player_bullets.push_back(Bullet(0, -PLAYER_BULLET_SPEED, x, y));

                if (current_power == 3) {

                    player_bullets.push_back(Bullet(0, -PLAYER_BULLET_SPEED, x - 0.375f * BASE_SIZE, y));
                    player_bullets.push_back(Bullet(0, -PLAYER_BULLET_SPEED, x + 0.375f * BASE_SIZE, y));
                }

            } else if ((current_power == 1) && (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) ) {

                reload_timer = RELOAD_DURATION;
                player_bullets.push_back(Bullet(0, -PLAYER_BULLET_SPEED, x, y));

            }


        } else {

            reload_timer--;
        }

        for (Bullet& bullet : enemy_bullets) {

            if (this->get_hitbox().intersects(bullet.get_hitbox())) {



                if (current_power == 4) {
                    current_power = 0;
                    shield_animation_over = false;

                } else {
                    dead = true;
                }

                bullet.dead = true; // we have to also kill the bullet haha

                break;


            }

        }

        powerup_type = 2;

        if (powerup_type > 0) {
            current_power = powerup_type;

            power_timer = POWERUP_DURATION;

        }
        if (powerup_type == 0){
            current_power = 0;

        } else {
            power_timer--;
        }

        if (!shield_animation_over) {
            shield_animation_over = explosion.update();
        }


    }

    for (Bullet& bullet : player_bullets) {
        bullet.update();

    }

    for (Enemy& enemy : enemies) {
        for (Bullet& bullet : player_bullets) {
            if ((!bullet.dead) && (0 < enemy.get_health())
            && enemy.get_hitbox().intersects(bullet.get_hitbox())) {

                bullet.dead = true;

                enemy.hit();

                break;
            }
        }
    }

    player_bullets.erase(
            remove_if(player_bullets.begin(),player_bullets.end(), [](const Bullet& i_bullet) {
        return 1 == i_bullet.dead;
    }), player_bullets.end());

}

bool Player::get_dead_animation_over() const {
    return dead_animation_over;
}

unsigned short Player::get_power_timer() const {
    return power_timer;
}