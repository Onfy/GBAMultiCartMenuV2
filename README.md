# GBAMultiCartMenuV2-batteryless

This is a fork of ArcheyChen's GBAMultiCartMenuV2, intended to modify the program to be better suited toward batteryless carts. It seems stable, I've tested it on my cart and I haven't noticed any problems, but maybe I messed up and it'll mess up your roms or saves. Be sure to make backups.

Rest of the readme that follows hasn't been changed yet.

## How to use?

What you need:
1. A multi game gba cart(better with a battery)
2. this very menu
3. something to flash the cart
   (I use gbabf myself: https://gitlab.com/Fexean/gbabf)
  
And you can get started:

1. Flash this menu to your cart at 0MB offset, and leave 0-16MB untouched.
2. Patch your game to sram save, and flash it to the space other than the first 16MB

Note: 
1.The menu will only reconize the game at` Nx8MB offset ` , that means you can only flash your game at 16MB,24Mb,32MB,40Mb, etc.
2.due to the hardware limit, if your game is 16MB, than you can only flash it to N x 16MB offset, like 16MB,32MB,48MB, if your game is 32MB you can only flash it to N x 32MB offsegt
3.the hardware use sram to change the flash offset, that means that when flash the cart, your save in sram might be corrutped,so remember to save your save to flash before adding new games.(see below)

Then you can start using it:

Hold L or R on boot, then the menu will show up , otherwise it will auto boot the last game you choose.

Everytime you boot into menu, it will ask you if you want to save your sram save to flash.

Everytime you boot a game from menu, it will load the save from flash to sram.



