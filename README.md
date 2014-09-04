toneAC_RTTTL
============

ToneAC_RTTTL is an arduino tone library that supports the RTTTL ringtone format and is based on the amazing toneAC Library v1.2 by Tim Eckel (http://code.google.com/p/arduino-tone-ac/).


Check out the RTTTL_examples file for some RTTTL ringtones.


How to use
````
char song_buffer[500];
prog_char bubble[] PROGMEM = "Bubble Full:d=4,o=6,b=125:8a#6,8a6,8g.6,16f6,8a6,8g6,8f6,8d#6,8g6,8f6,16d#6,8d.6,f.6,16d6,16c6,8a#5,8c6,8d6,8d#6,8c6,16d6,8d#.6,8f6,8f6,8g6,16a6,8g.6,8f6,8f6,8g6,8a6,8a#6,8a6,8g.6,16f6,8a6,8g6,8f6,8d#6,8g6,8f6,16d#6,8d.6,f.6,16d6,16c6,8a#5,8c6,8d6,8d#6,8c6,16d6,8d#.6,8f6,8f6,8g6,16a6,8f.6,8a#6";
strcpy_P(song_buffer, (char*)pgm_read_word(&bubble));
toneAC_RTTTL(song_buffer, volume, true);
````