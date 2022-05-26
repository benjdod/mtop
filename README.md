# mtop

Have you ever wanted to check on your system but couldn't stand to look at the staid and austere interface of `top`? Have you ever wanted to view key system stats but couldn't sift through the bevy of cryptuic numbers and letters that your system utilities have thrown at you? Have you ever wanted to truly feel like a **hacker** by tapping into the heart of the simulation itself and understanding the inner workings of your machine in the manner of one of the greatest cinematic masterpieces of the 20th century? Then if so, `mtop` is for you!

Because why can't your system be fun and functional at the same time?

### Building

When you first clone this repository, run the initialization script with `sh init-cmake.sh`. This will create a build directory with two subdirectories for debug and release. Then run `cmake --build build/[debug|release]` depending on which target you'd like to compile. The executable is called mtop and will be in its respective build folder.

🚨 *This program is still under development and thus is not guaranteed to be stable or even functional. Use at your own risk.*
