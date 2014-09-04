// ---------------------------------------------------------------------------
// toneAC_RTTTl Library - v1.0 - 10/26/2013
//
// AUTHOR/LICENSE:
// Created by Thaddeus Bort, Jr - https://github.com/thaddeusbort
// Copyright 2013 License: GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
//
//  based on toneAC Library v1.2 by Tim Eckel (http://code.google.com/p/arduino-tone-ac/)
//
// LINKS:
// Project home: http://code.google.com/p/arduino-tone-ac/
// Blog: http://arduino.cc/forum/index.php/topic,142097.msg1066968.html
//
// DISCLAIMER:
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// PURPOSE:
// Replacement to the standard tone library with the advantage of nearly twice
// the volume, higher frequencies (even if running at a lower clock speed),
// higher quality (less clicking), less stress on the speaker, and support for
// the RTTTL ringtone format. Disadvantages are that it must use certain pins and
// it uses two pins instead of one. But, if you're flexible with your pin
// choices, this is a great upgrade. It also uses timer 1 instead of timer 2,
// which may free up a conflict you have with the tone library.
//
// USAGE:
// Connection is very similar to a piezo or standard speaker. Except, instead
// of connecting one speaker wire to ground you connect both speaker wires to
// Arduino pins. The pins you connect to are specific, as toneAC lets the
// ATmega microcontroller do all the pin timing and switching. This is
// important due to the high switching speed possible with toneAC and to make
// sure the pins are always perfectly out of phase with each other
// (push/pull). See the below CONNECTION section for which pins to use for
// different Arduinos. Just as usual when connecting a speaker, make sure you
// add an inline 100 ohm resistor between one of the pins and the speaker wire.
//
// CONNECTION:
//   Pins  9 & 10 - ATmega328, ATmega128, ATmega640, ATmega8, Uno, Leonardo, etc.
//   Pins 11 & 12 - ATmega2560/2561, ATmega1280/1281, Mega
//   Pins 12 & 13 - ATmega1284P, ATmega644
//   Pins 14 & 15 - Teensy 2.0
//   Pins 25 & 26 - Teensy++ 2.0
//
// SYNTAX:
//   playNote( frequency [, volume [, length [, background ]]] ) - Play a note.
//     Parameters:
//       * frequency  - Play the specified frequency indefinitely, turn off with playNote().
//       * volume     - [optional] Set a volume level. (default: 10, range: 0 to 10 [0 = off])
//       * length     - [optional] Set the length to play in milliseconds. (default: 0 [forever], range: 0 to 2^32-1)
//       * background - [optional] Play note in background or pause till finished? (default: false, values: true/false)
//   toneAC_RTTTL( rtttlSong [, volume [, background ]] ) - Play an RTTTL formatted song.
//     Parameters:
//       * rtttlSong  - Play a song with the notes and durations specified in the rtttl formatted char[]. Play once and turn off.
//       * volume     - [optional] Set a volume level. (default: 10, range: 0 to 10 [0 = off])
//       * background - [optional] Play song in background or pause application until finished? (default: false, value: true/false)
//   stop()           - Stop playing.
//   isSongPlaying()  - Return a bool for whether or not a song is playing
//   setVolume( volume ) - Sets the volume of the speaker
//     Parameters:
//       * volume     - Set a volume level. (range: 0 to 10 [0 = off])
  char* getSongName();
//
// HISTORY:
// 10/26/2013 toneAC_RTTTL v1.0 - modified library to support the RTTTL format.
// Removed TONEAC_TINY because this version is not meant to be lightweight. If
// you want the small footprint please use Tim Eckel's original library.
//
// 01/27/2013 v1.2 - Fixed a counter error which went "over the top" and caused
// periods of silence (thanks Krodal). For advanced users needing tight code,
// the TONEAC_TINY switch in toneAC.h activates a version of toneAC() that
// saves 110 bytes. With TONEAC_TINY, the syntax is toneAC(frequency, length)
// while playing the note at full volume forever in the background. Added
// support for the ATmega 640, 644, 1281, 1284P and 2561 microcontrollers.
//
// 01/16/2013 v1.1 - Option to play notes in background, returning control back
// to your sketch for processing while note plays (similar to the way the tone
// library works). Volume is now linear and in the range from 0-10. Now uses
// prescaler 256 instead of 64 for frequencies below 122 Hz so it can go down
// to 1 Hz no matter what speed the CPU is clocked at (helpful if using toneAC
// to control a two-pin dual LED).
//
// 01/11/2013 v1.0 - Initial release.
//
// ---------------------------------------------------------------------------

#ifndef toneAC_RTTTL_h
  #define toneAC_RTTTL_h

  #if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif

  #if defined (__AVR_ATmega32U4__) || defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
    #define PWMT1AMASK DDB5
    #define PWMT1BMASK DDB6
    #define PWMT1DREG DDRB
    #define PWMT1PORT PORTB
  #elif defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__)
    #define PWMT1AMASK DDD4
    #define PWMT1BMASK DDD5
    #define PWMT1DREG DDRD
    #define PWMT1PORT PORTD
  #else
    #define PWMT1AMASK DDB1
    #define PWMT1BMASK DDB2
    #define PWMT1DREG DDRB
    #define PWMT1PORT PORTB
  #endif

  #if defined(__AVR_ATmega8__) || defined(__AVR_ATmega128__)
    #define TIMSK1 TIMSK
  #endif

  void playNote(unsigned long frequency = 0, uint8_t volume = 10, unsigned long length = 0, uint8_t background = false);
  void toneAC_RTTTL(char *p, uint8_t volume = 10, uint8_t background = false, uint8_t octave_offset = 0);
  void noToneAC_RTTTL();
  bool isSongPlaying();
  void stopSong();
  void setVolume(uint8_t volume);
  char* getSongName();

  // define notes
  #define NOTE_B0  31
  #define NOTE_C1  33
  #define NOTE_CS1 35
  #define NOTE_D1  37
  #define NOTE_DS1 39
  #define NOTE_E1  41
  #define NOTE_F1  44
  #define NOTE_FS1 46
  #define NOTE_G1  49
  #define NOTE_GS1 52
  #define NOTE_A1  55
  #define NOTE_AS1 58
  #define NOTE_B1  62
  #define NOTE_C2  65
  #define NOTE_CS2 69
  #define NOTE_D2  73
  #define NOTE_DS2 78
  #define NOTE_E2  82
  #define NOTE_F2  87
  #define NOTE_FS2 93
  #define NOTE_G2  98
  #define NOTE_GS2 104
  #define NOTE_A2  110
  #define NOTE_AS2 117
  #define NOTE_B2  123
  #define NOTE_C3  131
  #define NOTE_CS3 139
  #define NOTE_D3  147
  #define NOTE_DS3 156
  #define NOTE_E3  165
  #define NOTE_F3  175
  #define NOTE_FS3 185
  #define NOTE_G3  196
  #define NOTE_GS3 208
  #define NOTE_A3  220
  #define NOTE_AS3 233
  #define NOTE_B3  247
  #define NOTE_C4  262
  #define NOTE_CS4 277
  #define NOTE_D4  294
  #define NOTE_DS4 311
  #define NOTE_E4  330
  #define NOTE_F4  349
  #define NOTE_FS4 370
  #define NOTE_G4  392
  #define NOTE_GS4 415
  #define NOTE_A4  440
  #define NOTE_AS4 466
  #define NOTE_B4  494
  #define NOTE_C5  523
  #define NOTE_CS5 554
  #define NOTE_D5  587
  #define NOTE_DS5 622
  #define NOTE_E5  659
  #define NOTE_F5  698
  #define NOTE_FS5 740
  #define NOTE_G5  784
  #define NOTE_GS5 831
  #define NOTE_A5  880
  #define NOTE_AS5 932
  #define NOTE_B5  988
  #define NOTE_C6  1047
  #define NOTE_CS6 1109
  #define NOTE_D6  1175
  #define NOTE_DS6 1245
  #define NOTE_E6  1319
  #define NOTE_F6  1397
  #define NOTE_FS6 1480
  #define NOTE_G6  1568
  #define NOTE_GS6 1661
  #define NOTE_A6  1760
  #define NOTE_AS6 1865
  #define NOTE_B6  1976
  #define NOTE_C7  2093
  #define NOTE_CS7 2217
  #define NOTE_D7  2349
  #define NOTE_DS7 2489
  #define NOTE_E7  2637
  #define NOTE_F7  2794
  #define NOTE_FS7 2960
  #define NOTE_G7  3136
  #define NOTE_GS7 3322
  #define NOTE_A7  3520
  #define NOTE_AS7 3729
  #define NOTE_B7  3951
  #define NOTE_C8  4186
  #define NOTE_CS8 4435
  #define NOTE_D8  4699
  #define NOTE_DS8 4978

  #endif