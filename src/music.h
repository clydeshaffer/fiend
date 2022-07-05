#ifndef MUSIC_H
#define MUSIC_H

#define MUSIC_TRACK_NONE 0
#define MUSIC_TRACK_TITLE 1
#define MUSIC_TRACK_DIED 2
#define MUSIC_TRACK_STAIRS 3
#define MUSIC_TRACK_MAIN 4
#define MUSIC_TRACK_AREA2 5
#define MUSIC_TRACK_AREA3 6
#define MUSIC_TRACK_AREA4 7
#define MUSIC_TRACK_BOSS 8
#define MUSIC_TRACK_BOSS2 9
#define MUSIC_TRACK_END 10

void init_music();

void play_track(char track, char loop);

void tick_music();

void do_noise_effect(char note, char bend, char duration);

void stop_music();

#endif