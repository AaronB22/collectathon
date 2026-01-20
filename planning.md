A place to write your findings and plans

## Understanding
The player moves when the keypad buttons are held based on a set Speed. The bounding boxes of the player/treasure are created through a bn::rect namespace. The collision box is positioned at the sprite's x and y, the size is total, i.e. half in either direction. The player_rect and treasure_rect are defined in update, meaning they will be lost the next frame and disappear. I'm confused by this as I'd assume this would cause a memory leak?
When the player's bounding box intersects with the treasure's, the score increases and the treasure moves to a random position.
A string sprite, made through a sprite_text_generator, displays the current score. This needs to be removed and remade each frame.

## Planning required changes

## Brainstorming game ideas

## Plan for implementing game

