#ifndef MUSIC_H
#define MUSIC_H

#define MUSIC_TRACK_TITLE 0
#define MUSIC_TRACK_DIED 1
#define MUSIC_TRACK_STAIRS 2
#define MUSIC_TRACK_MAIN 3
#define MUSIC_TRACK_AREA2 4
#define MUSIC_TRACK_AREA3 5
#define MUSIC_TRACK_AREA4 6


void init_music();

void play_track(char track, char loop);

void tick_music();

void do_noise_effect(char note, char bend, char duration);

void stop_music();

#endif