STM32F746-Discovery board is used for SD audio playback.
Program reads .wav files (without any problems) and .mp3 files (may stuck on rewind) using Helix decoder. Just export your Rekordbox playlist to microSD card and program will read it.
Playback control is realized via touchscreen and via CDJ-100S keyboard, jog and slider via SPI commands from CDJ-100S MIDI controller (look in another repository).

Brief instructions of how to upgrade your CDJ-100S player can be found in Instructions.pdf file.

Ideas for program:

-fix bugs in Helix mp3 decoder implementation

-make master tempo and sound effects (this may require additional digital signal processor, I'm afraid)

-add some new display modes
