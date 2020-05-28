STM32F746-Discovery board is used for SD audio playback.
Program reads .wav files (without any problems) and .mp3 files (may stuck on rewind) using Helix decoder. Just export your Rekordbox playlist to microSD card and program will read it.
Playback control is realized via touchscreen and via CDJ-100S keyboard, jog and slider via SPI commands from CDJ-100S MIDI controller (look in another repository).

Brief instructions of how to upgrade your CDJ-100S player can be found in Instructions.pdf file.

The author would appreciate any improvements to this program. Here are some ideas:

-fix bugs in Helix mp3 decoder implementation

-make master tempo and sound effects (this may require additional digital signal processor, I'm afraid)

-add some new display modes

Updates 28.05.2020:

- changed high resolution waveform display process. Now there are 9 scale modes of waveform display. Touch the right side of waveform to expand, touch the left side to compress it, touch the center cursor to return default scale.

- changed hot cues behaviour. Now SCAN+WAH buttons are used to switch between the cue points. The switch now doesn't change the playback state (playing/stopped).

- fixed some minor bugs.
