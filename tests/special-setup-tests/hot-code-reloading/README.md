# About

This example is the same as the basic shapes example but it adds hot code reloading.
This is achieved by separating the game and platform layer. 

The platform layer gets compiled into an executable while the game layer gets compiled into a dll.

If you press the `R` key while the game is running, the program will reload the dll.
You can make certain kinds of changes to `game.c` (such as changing the color on line `40` from `rf_clear_background(rf_raywhite)` to `rf_clear_background(rf_red)`),
recompile the code and then press the `R` key to reload it and see the changes appear live without restarting the program. 

Additionally the example will print on the screen the number of hot-code reloads.

# How to use

You can run `build.bat` to compile all examples including this one.
After that you can make changes to `game.c` and run the script again, then press the `R` key to
hot-reload the code. 

From CLion you can build the `hot_code_reloading_dynamic` target and then run the `hot_code_reloading` target.
After that you can make changes to `game.c` and build the`hot_code_reloading_dynamic` target again and then press the `R` key to hot-reload the code.

![](https://i.gyazo.com/884f3d322d9656145c52cb49e1993127.gif)