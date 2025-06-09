//=============================================================================
// Exquis+_003.ino
//=============================================================================

// Inclure la bibliothèque de gestion des interruptions
#include "PinChangeInterrupt.h"
// Inclure la bibliothèque MIDI et créer une instance MIDI
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//- Constantes ----------------------------------------------------------------
#define LED_PIN 10
#define BTN_PIN 11

//- Variables -----------------------------------------------------------------
// Drapeau de l'interruption de bouton
int8_t buttonFlag = 0;
// Mode de gestion des messages MIDI entrants (0 = Exquis, 1 = Digitakt)
int8_t inputMode = 0;
// Gestion des seuils et courbes de l'Exquis
float aftertouchThreshold = 0.5;
int8_t aftertouchSlope = 2;
float slideThreshold = 0.5;
int8_t slideSlope = 2;
// Gestion Digitakt
byte inputChannel = 16;
byte channelCount = 4;

//- Setup ---------------------------------------------------------------------
void setup() {
  // Configurer l'interruption bouton
  attachPCINT(digitalPinToPCINT(BTN_PIN), BTN_ISR, FALLING);
  // Configurer les interruptions MIDI
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleCC);
  MIDI.setHandleAfterTouchChannel(handleAftertouch);
  MIDI.setHandlePitchBend(HandlePitchBend);
  // Initialiser MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  // Configuer les entrées/sorties
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  // inputMode à 0
  inputMode = 1;
  digitalWrite(LED_PIN, LOW);
}

//- Fonction interruption bouton ----------------------------------------------
void BTN_ISR(void) {
  buttonFlag = 1;
}

//- Fonctions callback MIDI ---------------------------------------------------
void handleNoteOn(byte channel, byte pitch, byte velocity) {
  if (inputMode == 0 && channel <= 5) {
    MIDI.sendNoteOn(pitch, velocity, channel == 1 ? 16 : 18 - channel);
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  if (inputMode == 0 && channel <= 5) {
    MIDI.sendNoteOff(pitch, 0, channel == 1 ? 16 : 18 - channel);
  }
}

void handleCC(byte channel, byte number, byte value) {
  // Gestion des CC venant de l'Exquis
  if (inputMode == 0 && channel <= 5 && number == 74) {  // SLide
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
  // Gestion des CC venant du Digitakt, channel 13
  if (inputMode == 1 && channel == inputChannel) {
    int8_t flag = 0;
    // Track Volume & Pattern Mute
    if (number == 95 || number == 110) {
      flag = 1;
    }
    // Trig parameters
    if ((number >= 3 && number <= 5) || (number == 9) || (number >= 13 && number <= 14) || (number == 65)) {
      flag = 1;
    }
    // Source parameters
    if ((number >= 16 && number <= 17) || (number >= 20 && number <= 23)) {
      flag = 1;
    }
    // Filter parameters
    if ((number >= 26 && number <= 27) || (number >= 70 && number <= 77) || (number >= 91 && number <= 92) || (number == 111)) {
      flag = 1;
    }
    // Amp parameters
    if ((number >= 79 && number <= 83) || (number >= 87 && number <= 90)) {
      flag = 1;
    }
    // FX parameters
    if ((number == 12) || (number >= 54 && number <= 57) || (number >= 84 && number <= 85)) {
      flag = 1;
    }
    // Mod 1 parameters
    if (number >= 102 && number <= 109) {
      flag = 1;
    }
    // Mod 2 parameters
    if (number >= 112 && number <= 119) {
      flag = 1;
    }
    // Mod 3 parameters
    if ((number >= 28 && number <= 31) || (number >= 52 && number <= 53) || (number == 78) || (number == 86)) {
      flag = 1;
    }
    // Envoyer le CC
    if (flag == 1) {
      for (int i = 1; i <= channelCount - 1; i++) {
        MIDI.sendControlChange(number, value, inputChannel - i);
      }
    }
  }
}

void handleAftertouch(byte channel, byte value) {
  if (inputMode == 0 && channel <= 5) {
    float x, y = 0;
    x = value / 127.0;
    if (x < aftertouchThreshold) {
      y = 0;
    } else {
      y = pow((x - aftertouchThreshold) / (1.0 - aftertouchThreshold), aftertouchSlope) * 127.0;
    }
    MIDI.sendAfterTouch(y, channel == 1 ? 16 : 18 - channel);
  }
}

void HandlePitchBend(byte channel, int bend) {
  if (inputMode == 0 && channel <= 5) {
    float x, y = 0;
    x = bend / 16384.0;
    y = 4 * pow(x - 0.5, 3) - 0.5;
    MIDI.sendPitchBend(bend, channel == 1 ? 16 : 18 - channel);
  }
}

//- Loop -----------------------------------------------------------------------
void loop() {
  MIDI.read();
  if (buttonFlag) {
    delay(10);
    if (digitalRead(BTN_PIN) == LOW) {
      if (inputMode) {
        digitalWrite(LED_PIN, HIGH);
        inputMode = 0;
      } else {
        digitalWrite(LED_PIN, LOW);
        inputMode = 1;
      }
    }
    buttonFlag = 0;
  }
}
