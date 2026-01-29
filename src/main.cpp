#include <bn_core.h>
#include <bn_display.h>
#include <bn_log.h>
#include <bn_keypad.h>
#include <bn_random.h>
#include <bn_rect.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_size.h>
#include <bn_string.h>
#include <bn_backdrop.h>
#include <bn_color.h>
#include <bn_math.h>

#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"
#include "bn_sprite_items_enemy.h"
#include "bn_sprite_items_megadot.h"
#include "bn_sprite_items_enemydot.h"

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 2;
static constexpr bn::fixed TREASURE_SPEED = 1;

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};
static constexpr bn::size MEGA_TREASURE_SIZE = {16, 16};
static constexpr bn::size ENEMYBOX_SIZE = {32, 32};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

int main()
{
    bn::core::init();

    bn::backdrop::set_color(bn::color(21, 15, 15));
    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;
    static constexpr int xCord = 70;
    static constexpr int yCord = 10;

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(xCord, yCord);
    bn::sprite_ptr treasure = bn::sprite_items::dot.create_sprite(0, 0);
    bn::sprite_ptr enemybox = bn::sprite_items::enemydot.create_sprite(0, 0);

    int boostDuration = 60;  // How long the boost will last in frames(?)
    int boostTime = 0;       // Decreases while boosting
    int boostCount = 3;      // How many boosts remain
    int boostMultiplier = 2; // How much faster the sphere moves

    int currentSpeedMultiplier = 1; // The Current multiplier for speed, gets changed to 2 when boosting.

    // The CURRENT width and height of the treasure ( this could be done better )
    int treasureSizeX = TREASURE_SIZE.width();
    int treasureSizeY = TREASURE_SIZE.height();
    int treasureScareMultiplier = 3; // This multiplied with the treasure's size is the distance it will run from the player at.

    int enemyDirectionX = 1; // Used to determine movement logic for enemybox on x axis
    int enemyDirectionY = 1; // Used to determine movement logic for enemybox on y axis
    int enemySpeedX = 1;
    int enemySpeedY = 1;

    while (true)
    {

        // Speed boost
        if (bn::keypad::a_pressed() && boostCount > 0)
        {
            if (boostCount > 0)
            {
                boostCount--;
                boostTime = boostDuration;
                currentSpeedMultiplier = boostMultiplier;
            }
        }
        if (boostTime > 0)
        {
            boostTime--;
        }
        else
        {
            currentSpeedMultiplier = 1;
        }
        // Loop around border
        if (player.x() >= MAX_X)
        {
            player.set_x(MIN_X + 1);
        }
        if (player.x() <= MIN_X)
        {
            player.set_x(MAX_X - 1);
        }
        if (player.y() >= MAX_Y)
        {
            player.set_y(MIN_Y + 1);
        }
        if (player.y() <= MIN_Y)
        {
            player.set_y(MAX_Y - 1);
        }
        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - SPEED * currentSpeedMultiplier);
        }
        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + SPEED * currentSpeedMultiplier);
        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - SPEED * currentSpeedMultiplier);
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + SPEED * currentSpeedMultiplier);
        }

        // Reset Button
        if (bn::keypad::start_pressed())
        {
            player.set_x(xCord);
            player.set_y(yCord);

            treasure.set_x(0);
            treasure.set_y(0);

            score = 0;
        }

        // The bounding boxes of the player and treasure, snapped to integer pixels
        bn::rect player_rect = bn::rect(player.x().round_integer(),
                                        player.y().round_integer(),
                                        PLAYER_SIZE.width(),
                                        PLAYER_SIZE.height());
        bn::rect treasure_rect = bn::rect(treasure.x().round_integer(),
                                          treasure.y().round_integer(),
                                          treasureSizeX,
                                          treasureSizeY);
        bn::rect enemybox_rect = bn::rect(enemybox.x().round_integer(),
                                          enemybox.y().round_integer(),
                                          ENEMYBOX_SIZE.width(),
                                          ENEMYBOX_SIZE.height());

        // If the bounding boxes overlap, set the treasure to a new location an increase score
        if (player_rect.intersects(treasure_rect))
        {
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            score++;

            // If score > 10, treasure sprite becomes mega - Seadrah
            if (score == 10)
            {
                treasure = bn::sprite_items::megadot.create_sprite(0, 0);

                // This rect only exists for the frame it was created
                treasure_rect = bn::rect(treasure.x().round_integer(),
                                         treasure.y().round_integer(),
                                         MEGA_TREASURE_SIZE.width(),
                                         MEGA_TREASURE_SIZE.height());
                treasureSizeX = MEGA_TREASURE_SIZE.width();
                treasureSizeY = MEGA_TREASURE_SIZE.height();
            }
        }

        // Move treasure away from player
        // Get player direction to treasure -> treasure(x,y)-player(x,y)
        bn::fixed dx = bn::clamp<bn::fixed>(treasure.x() - player.x(), -1, 1);
        bn::fixed dy = bn::clamp<bn::fixed>(treasure.y() - player.y(), -1, 1);
        // Get "Unit" (not actually unit but just 1 in either direction)
        // Multiply by treasure_speed
        dx = dx * TREASURE_SPEED;
        dy = dy * TREASURE_SPEED;
        // Move to new spot
        treasure.set_x(treasure.x() + dx);
        treasure.set_y(treasure.y() + dy);

        // Loop treasure around border ONLY when the player is close enough
        if (bn::abs(treasure.x() - player.x()) < treasureSizeX * treasureScareMultiplier && bn::abs(treasure.y() - player.y()) < treasureSizeY * treasureScareMultiplier)
        {
            if (treasure.x() >= MAX_X)
            {
                treasure.set_x(MIN_X + 10);
            }
            if (treasure.x() <= MIN_X)
            {
                treasure.set_x(MAX_X - 10);
            }
            if (treasure.y() >= MAX_Y)
            {
                treasure.set_y(MIN_Y + 10);
            }
            if (treasure.y() <= MIN_Y)
            {
                treasure.set_y(MAX_Y - 10);
            }
        }
        else
        {
            // Otherwise just bonk.
            if (treasure.x() >= MAX_X - treasureSizeX)
            {
                treasure.set_x(MAX_X - treasureSizeX);
            }
            if (treasure.x() <= MIN_X + treasureSizeX)
            {
                treasure.set_x(MIN_X + treasureSizeX);
            }
            if (treasure.y() >= MAX_Y - treasureSizeY)
            {
                treasure.set_y(MAX_Y - treasureSizeY);
            }
            if (treasure.y() <= MIN_Y + treasureSizeY)
            {
                treasure.set_y(MIN_Y + treasureSizeY);
            }
        }
        // Update score display
        bn::string<MAX_SCORE_CHARS>
            score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        // enemy logic

        // sets ememy dircetions

        // moves on x axis when the score is a even number
        if (enemybox.x() >= MAX_X)
        {
            enemyDirectionX = -1;
            enemySpeedX = rng.get_int(1, 2);
        }
        if (enemybox.x() <= MIN_X)
        {
            enemyDirectionX = 1;
            enemySpeedX = rng.get_int(1, 2);
        }
        if (enemybox.y() >= MAX_Y)
        {
            enemyDirectionY = -1;
            enemySpeedY = rng.get_int(1, 2);
        }
        if (enemybox.y() <= MIN_Y)
        {
            enemyDirectionY = 1;
            enemySpeedY = rng.get_int(1, 2);
        }

        enemybox.set_x(enemybox.x() + enemySpeedX * enemyDirectionX);
        enemybox.set_y(enemybox.y() + enemySpeedY * enemyDirectionY);

        // detects if player and enemy hit
        if (player_rect.intersects(enemybox_rect))
        {
            if (score > 0)
            {
                score--;
            }
            player.set_x(10);
            player.set_y(10);
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            enemybox.set_x(new_x);
            enemybox.set_y(new_y);
        }
        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        bn::core::update();
    }
}