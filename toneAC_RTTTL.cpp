// ---------------------------------------------------------------------------
// Created by Thaddeus Bort, Jr - https://github.com/thaddeusbort
// Copyright 2013 License: GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
//
//  based on work by Tim Eckel (http://code.google.com/p/arduino-tone-ac/)
//
// See "toneAC_RTTTL.h" for purpose, syntax, version history, links, and more.
// ---------------------------------------------------------------------------

#include "toneAC_RTTTL.h"

unsigned long _t_time; // Used to track end note with timer when playing note in the background.
const uint8_t _t_volume[] = { 200, 100, 67, 50, 40, 33, 29, 22, 11, 2 }; // Duty for linear volume control.
const uint16_t _notes[] = { 0,
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};

long _song_frequencies[100];
long _song_lengths[100];
uint8_t _song_volume;
uint8_t _note_index;
bool _song_is_playing = false;
char _song_name[20];

#define isdigit(n) (n >= '0' && n <= '9')

void toneAC_RTTTL(char *p, uint8_t volume, uint8_t background, uint8_t octave_offset) {
  if (volume == 0) { toneAC_stop(); return; } // If volume is 0, turn off sound and return.
  if (volume > 10) volume = 10;                              // Make sure volume is in range (1 to 10).
  _song_volume = volume;
  _song_is_playing = true;

  // parse the rtttl string
  uint8_t default_dur = 4;
  uint8_t default_oct = 6;
  uint8_t _bpm = 63;
  uint16_t _wholenote = 0;
  uint16_t duration = 0;
  int num;
  byte note;
  byte scale;
  uint8_t song_index = 0;

  // clear _song_name
  while(song_index < 19) { // only go to 19, the 20th character is the null termination
    _song_name[song_index++] = ' ';
  }
  song_index = 0;
  while(*p != ':') {
    if(song_index < 19) { // only go to 19, the 20th character is the null termination
      _song_name[song_index++] = *p++;
    } else {
      p++; // only saving the first 19 characters of the name, skip the rest
    }
  }
  p++;                     // skip ':'

  // get default duration
  if(*p == 'd') {
    p++; p++;              // skip "d="
    num = 0;
    while(isdigit(*p)) {
      num = (num * 10) + (*p++ - '0');
    }
    if(num > 0) default_dur = num;
    p++;                   // skip comma
  }

  // get default octave
  if(*p == 'o') {
    p++; p++;              // skip "o="
    num = *p++ - '0';
    if(num >= 3 && num <=7) default_oct = num;
    p++;                   // skip comma
  }

  // get BPM
  if(*p == 'b') {
    p++; p++;              // skip "b="
    num = 0;
    while(isdigit(*p)) {
      num = (num * 10) + (*p++ - '0');
    }
    _bpm = num;
    p++;                   // skip colon
  }

  // BPM usually expresses the number of quarter notes per minute
  _wholenote = (60 * 1000L / _bpm) * 4;  // this is the time for whole note (in milliseconds)
 
  // parse notes and durations into arrays
  // now begin note loop
  int noteIndex = 0;
  while(*p) {
    // first, get note duration, if available
    num = 0;
    while(isdigit(*p)) {
      num = (num * 10) + (*p++ - '0');
    }
    if(num) duration = _wholenote / num;
    else duration = _wholenote / default_dur;  // we will need to check if we are a dotted note after

    // now get the note
    note = 0;
    switch(*p) {
      case 'c':
        note = 1;
        break;
      case 'd':
        note = 3;
        break;
      case 'e':
        note = 5;
        break;
      case 'f':
        note = 6;
        break;
      case 'g':
        note = 8;
        break;
      case 'a':
        note = 10;
        break;
      case 'b':
        note = 12;
        break;
      case 'p':
      default:
        note = 0;
    }
    p++;

    // now, get optional '#' sharp
    if(*p == '#') {
      note++;
      p++;
    }

    // now, get optional '.' dotted note
    if(*p == '.') {
      duration += duration/2;
      p++;
    }
  
    // now, get scale
    if(isdigit(*p)) {
      scale = *p - '0';
      p++;
    } else {
      scale = default_oct;
    }
    scale += octave_offset;

    if(*p == ',')
      p++;       // skip comma for next note (or we may be at the end)

    // add note to arrays
    _song_frequencies[noteIndex] = note ? _notes[(scale-4)*12 + note] : 0;
    _song_lengths[noteIndex] = duration;
    noteIndex++;
  }

  _note_index = 0;
  toneAC_playNote(_song_frequencies[0], _song_volume, _song_lengths[0], true);
}

void toneAC_playNote(unsigned long frequency, uint8_t volume, unsigned long length, uint8_t background) {
  if(frequency == 0 || volume == 0) {
    if(background) {
      _t_time = millis() + length; // Set when the note should end.
      TIMSK1 |= _BV(OCIE1A);         // Activate the timer interrupt.

      TCCR1B  = _BV(CS11);        // Default clock prescaler of 8.
      TCCR1A  = _BV(WGM10);       // Set to defaults so PWM can work like normal (PWM, phase corrected, 8bit).
    }
    return;
  }
  
  PWMT1DREG |= _BV(PWMT1AMASK) | _BV(PWMT1BMASK); // Set timer 1 PWM pins to OUTPUT (because analogWrite does it too).
  
  uint8_t prescaler = _BV(CS10);                 // Try using prescaler 1 first.
  unsigned long top = F_CPU / frequency / 2 - 1; // Calculate the top.
  if (top > 65535) {                             // If not in the range for prescaler 1, use prescaler 256 (122 Hz and lower @ 16 MHz).
    prescaler = _BV(CS12);                       // Set the 256 prescaler bit.
    top = top / 256 - 1;                         // Calculate the top using prescaler 256.
  }
  unsigned int duty = top / _t_volume[volume - 1]; // Calculate the duty cycle (volume).

  if (length > 0 && background) {  // Background tone playing, returns control to your sketch.
    _t_time = millis() + length; // Set when the note should end.
    TIMSK1 |= _BV(OCIE1A);         // Activate the timer interrupt.
  }

  ICR1   = top;                         // Set the top.
  if (TCNT1 > top) TCNT1 = top;         // Counter over the top, put within range.
  TCCR1B = _BV(WGM13)  | prescaler;     // Set PWM, phase and frequency corrected (top=ICR1) and prescaler.
  OCR1A  = OCR1B = duty;                // Set the duty cycle (volume).
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1B0); // Inverted/non-inverted mode (AC).

  if (length > 0 && !background) { delay(length); toneAC_stop(); } // Just a simple delay, doesn't return control till finished.
}

void toneAC_stop() {
  TIMSK1 &= ~_BV(OCIE1A);     // Remove the timer interrupt.
  TCCR1B  = _BV(CS11);        // Default clock prescaler of 8.
  TCCR1A  = _BV(WGM10);       // Set to defaults so PWM can work like normal (PWM, phase corrected, 8bit).
  PWMT1PORT &= ~_BV(PWMT1AMASK); // Set timer 1 PWM pins to LOW.
  PWMT1PORT &= ~_BV(PWMT1BMASK); // Other timer 1 PWM pin also to LOW.
  
  // reset arrays
  if(_t_time > 0) {
    _t_time = 0;
    for(int i=0; i<100; i++) {
      _song_frequencies[i] = 0;
      _song_lengths[i] = 0;
    }
  }
  _song_is_playing = false;
}
void toneAC_togglePause() {
  if(_song_is_playing) {
    TIMSK1 &= ~_BV(OCIE1A);     // Remove the timer interrupt.
    TCCR1B  = _BV(CS11);        // Default clock prescaler of 8.
    TCCR1A  = _BV(WGM10);       // Set to defaults so PWM can work like normal (PWM, phase corrected, 8bit).
    PWMT1PORT &= ~_BV(PWMT1AMASK); // Set timer 1 PWM pins to LOW.
    PWMT1PORT &= ~_BV(PWMT1BMASK); // Other timer 1 PWM pin also to LOW.
  } else {
    toneAC_playNote(_song_frequencies[_note_index], _song_volume, _song_lengths[_note_index], true);
  }
  _song_is_playing = !_song_is_playing;
}

bool toneAC_isSongPlaying() {
  return _song_is_playing;
}
void toneAC_setVolume(uint8_t volume) {
  _song_volume = volume > 10 ? 10 : volume;
}
char *toneAC_getSongName() {
  if(toneAC_isSongPlaying())
    return _song_name;
  return "";
}


ISR(TIMER1_COMPA_vect) { // Timer interrupt vector.
  // Check to see if it's time for the note to end.
  if (millis() >= _t_time) {
    _note_index++;

    // if there are more notes left to play in the song, play the next note
    if(_song_lengths[_note_index] > 0) {
      toneAC_playNote(_song_frequencies[_note_index], _song_volume, _song_lengths[_note_index], true);
    } else {
      // the song is over, stop playing
      toneAC_stop();
    }
  }
}