A place to write your findings and plans

## Understanding
The player moves when the keypad buttons are held based on a set Speed. The bounding boxes of the player/treasure are created through a bn::rect namespace. The collision box is positioned at the sprite's x and y, the size is total, i.e. half in either direction. The player_rect and treasure_rect are defined in update, meaning they will be lost the next frame and disappear. I'm confused by this as I'd assume this would cause a memory leak?
When the player's bounding box intersects with the treasure's, the score increases and the treasure moves to a random position.
A string sprite, made through a sprite_text_generator, displays the current score. This needs to be removed and remade each frame. - Jamison

The first part of the code is defining variables such as speed, character and treasure size, and window dimensions, as well as the location of the score and maximum characters. In the while loop, it defines controls, intersection logic, and score updates. - Seadrah

Game does stuff!

## Planning required changes
Change the speed of the player - Seadrah

Change the backdrop color - Jamison DONE

Change the starting position of the player and dot, making new static constexpr for starting X and Y of each - Aaron

Make it so when the player hits start, the game restarts (the player and treasure are sent back to their initial positions and the score is reset to zero) - Seadrah

Make it so that the player loops around the screen (if they go off the left of the screen, they show up on the right, if they go off the bottom of the screen they show up at the top, etc.) - Aaron

Make a speed boost. When the player presses 'A', their speed is increased for a short amount of time. They can only use the speed boost 3 times. They get all speed boosts back when the game is restarted by pressing start. - Jamison DONE

 Make it so when player hits start, game restarts -- can be implemented with an "if" check for the start button. I can also change the speed of the character. - Seadrah

## Brainstorming game ideas
should add a sprite wall that kills user
Multiple treasure pieces
Treasure pieces automatically move after a few seconds
Could make user painfully slow, change background color to something more fun? - Seadrah

## Plan for implementing game
Aaron shouldnt do anything cause he is awesome

Adjust speed, adjust background colors - Seadrah


### Treasure Pieces run away
* Get the Vector between the treasure and the player.
* Find the unit of that vector -- magnitude of 1.
* Invert the Vector.
* Multiply the Vector by an ENEMY_SPEED variable.
* Move the treasure to the new position indicated by the Vector.

### Introduce megadot treasure - Seadrah
* Made treasure get super mega big after 10 dots are captured