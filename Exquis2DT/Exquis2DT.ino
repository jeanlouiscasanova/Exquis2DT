//=============================================================================
// Exquis2DT_005_001.ino
//=============================================================================

// Inclure la bibliothèque de gestion des interruptions
#include "PinChangeInterrupt.h"
// Inclure la bibliothèque MIDI et créer une instance MIDI
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//- Variables -----------------------------------------------------------------
// Gestion des seuils et courbes de l'Exquis
float aftertouchThreshold = 0.5;
int8_t aftertouchSlope = 2;
float slideThreshold = 0.5;
int8_t slideSlope = 2;

//- Setup ---------------------------------------------------------------------
void setup() {
  // Configurer les interruptions MIDI
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleCC);
  MIDI.setHandleAfterTouchChannel(handleAftertouch);
  MIDI.setHandlePitchBend(HandlePitchBend);
  // Initialiser MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
}

//- Fonctions callback MIDI ---------------------------------------------------
void handleNoteOn(byte channel, byte pitch, byte velocity) {
  MIDI.sendNoteOn(pitch, velocity, channel == 1 ? 16 : 18 - channel);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  MIDI.sendNoteOff(pitch, 0, channel == 1 ? 16 : 18 - channel);
}

void handleCC(byte channel, byte number, byte value) {
  if (number == 74) {  // SLide
    float x, y = 0;
    x = value / 127.0;
    if (x == 0) {
      y = 127;
    } else if (x < slideThreshold) {
      y = pow((slideThreshold - x) / (1.0 - slideThreshold), slideSlope) * 127.0;
    } else {
      y = pow((x - slideThreshold) / (1.0 - slideThreshold), slideSlope) * 127.0;
    }
    MIDI.sendControlChange(1, y, channel == 1 ? 16 : 18 - channel);  // => Modulation Wheel
  }
}

void handleAftertouch(byte channel, byte value) {
  float x, y = 0;
  x = value / 127.0;
  if (x < aftertouchThreshold) {
    y = 0;
  } else {
    y = pow((x - aftertouchThreshold) / (1.0 - aftertouchThreshold), aftertouchSlope) * 127.0;
  }
  MIDI.sendAfterTouch(y, channel == 1 ? 16 : 18 - channel);
}

void HandlePitchBend(byte channel, int bend) {
  float x, y = 0;
  x = bend / 16384.0;
  y = 4 * pow(x - 0.5, 3) - 0.5;
  MIDI.sendPitchBend(bend, channel == 1 ? 16 : 18 - channel);
}

//- Loop -----------------------------------------------------------------------
void loop() {
  MIDI.read();
}
