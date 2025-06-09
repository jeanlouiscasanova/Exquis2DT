# Exquis2DT
Polyphony &amp; MPE on Digitakt with Intuitive Instruments Exquis and Arduino

## Features
Exquis 2DT is an Arduino program for using the Intuitives Instruments Exquis controller with the Digitakt:
* Exquis must be set for 4-voice MPE polyphony. The first 5 MIDI channels are routed to the last 4 channels of the Digitakt, so: Exquis channel 1 => Digitack channel 16 (channels 1 and 2 are grouped together), Exquis channels 2 to 5 => Digitakt channels 16 to 13.
* Some messages are converted to be managed on the Digitakt: Channel Pressure converted to Aftertouch, Slide (CC #74) converted to Modulation Wheel (CC #1).
* Exquis2DT manages the trigger thresholds and response curves of the Exquis MPE messages.
* Messages handled: Note On, Note Off, Channel Pressure, Slide (CC #74), Pitch Bend.

The [arduino_midi_library](https://github.com/FortySevenEffects/arduino_midi_library) library from [FortySevenEffects](https://github.com/FortySevenEffects) is used.
