# Nest

This and other 15-466 base code is built around a collection of useful libraries and functions which we will collectively call "nest".
This name captures the goal of having the various parts and functions relatively easy to reconfigure (and remove), while still forming a good support for your game.

## What Is Included + New

Here is a quick overview of what is included. For further information, ☺read the code☺ !
- Base code (files you will certainly edit):
    - ```main.cpp``` creates the game window and contains the main loop. Set your window title, size, and initial Mode here.
    - **New:** ```MenuMode.*pp``` declaration+definition for a sprite-based menu. You'll probably extend this to draw text.
    - ```Jamfile``` responsible for telling FTJam how to build the project. Change this when you add additional .cpp files and to change your runtime executable's name.
    - ```.gitignore``` ignores generated files. You will need to change it if your executable name changes. (If you find yourself changing it to ignore, e.g., your editor's swap files you should probably, instead be investigating making this change in the global git configuration.)
- Useful code (files you should investigate, but probably won't change):
    - **New:** ```data_path.*pp``` get paths relative to the game's directory.
    - **New:** ```Load.*pp``` deferred resource loading.
    - **New:** ```read_write_chunk.hpp``` simple helper to load/save data arrays.
    - **New:** ```Sprite.*pp``` runtime component of a sprite asset pipeline.
    - ```Mode.hpp``` base class for modes (things that recieve events and draw).
    - ```ColorTextureProgram.hpp``` example OpenGL shader program, wrapped in a helper class. **New:** `Load<>`'d at launch.
    - ```gl_compile_program.hpp``` helper function to compiles OpenGL shader programs.
    - ```load_save_png.hpp``` helper functions to load and save PNG images.
    - ```GL.hpp``` includes OpenGL 3.3 prototypes without the namespace pollution of (e.g.) SDL's OpenGL header; on Windows, deals with some function pointer wrangling.
    - ```gl_errors.hpp``` provides a ```GL_ERRORS()``` macro.
	- **New:** ```pack-sprites.cpp```, ```sprites/extract-sprites.py``` utilities used in the sprite asset pipeline. See [the README](sprites/README.md).
- Here be dragons (files you probably don't need to look at):
    - ```make-GL.py``` does what it says on the tin. Included in case you are curious. You won't need to run it.
	- ```glcorearb.h``` used by ```make-GL.py``` to produce ```GL.*pp```


## Build Instructions

This code is set up to build across Linux, Windows, and MacOS.

Building will be done from the command prompt, using FTJam, the freetype project's fork of Perforce's excellent "Jam" build tool ([Documentation](https://www.perforce.com/documentation/jam-documentation) page at Perforce, for the curious).

To add or remove files from the build, read and edit the [Jamfile](Jamfile).

### Setup

Setup for your development environment should be relatively simple:

 0. (Optional) Make sure your system is set up to make it easy to use your favorite code editor and git from the command prompt. So much nicer than using a GUI.
 1. Install one of our target C++ compilers:
 	- Linux: g++
	- MacOS: clang++ (from XCode)
	- Windows: Visual Studio Community 2019
 2. Install ftjam from your OS's package manager:
    - Linux: e.g. `sudo apt-get install ftjam`
	- MacOS: e.g. `brew install ftjam`
	- Windows: (skip this step; jam for windows is included in the nest-libs package)
 3. Extract an appropriate release of [nest-libs](https://github.com/15-466/nest-libs) to a sibling of this folder:
    - Linux: https://github.com/15-466/nest-libs/releases/download/v0.0/nest-libs-linux-v0.0.tar.gz
	- MacOS: https://github.com/15-466/nest-libs/releases/download/v0.0/nest-libs-macos-v0.0.tar.gz
	- Windows: https://github.com/15-466/nest-libs/releases/download/v0.0/nest-libs-windows-v0.0.zip

Once you are finished, your directory tree should looks something like this:

```
game-programming/ #can be called anything
	nest-libs/    #nest-libs from that repository's releases page
		windows/  #subdirectory name varies depending on platform
		...
	my-game/      #fork of this code; can be called anything
		...
	...           #eventually, you will have further subdirectories for other games
```

### Building

Once you have your development environment set up, building is as simple as opening a command prompt (see Windows Note below), changing to the game directory, and running `jam`.

Here are a few worthwhile variations:

```
# Note: Always build from the game's root folder:
  $ cd game-programming
  $ cd my-game

# Simplest build command:
  $ jam

# Variation: multiple (six, in this case) build jobs in parallel:
  $ jam -j6

# Variation: quit if any build job fails:
  $ jam -q

# Variation: run game if build succeeds:
  $ jam && dist/pong

# Variation: show commands being run (useful for debugging the Jamfile):
  $ jam -dx

# Useful: show summary of jam flags:
  $ jam -h

# Variation: this is what I generally use:
  $ jam -q -j6 && dist/pong

# Useful: delete all built files:
  $ jam clean
```


*Windows Note:* a pre-compiled `jam.exe` and a .bat file + .lnk to launch a VS2019 command prompt with jam in the `%PATH%` are included in the `nest-libs/windows/tools/` directory. The README.md in that folder explains how to use them.
