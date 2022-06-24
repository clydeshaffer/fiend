#include "music.h"
#include "gametank.h"
#include "dynawave.h"
#include "note_numbers.h"

extern const unsigned char* MainMusic;
extern const unsigned char* TitleMusic;
extern const unsigned char* DiedMusic;
unsigned char audio_amplitudes[4] = {0, 0, 0, 0};
unsigned char* music_cursor = 0;
unsigned char delay_counter = 0;
unsigned char* repeat_point;

void init_music() {
    music_cursor = 0;
    delay_counter = 0;
}

void play_track(char track, char loop) {
    switch (track)
    {
    case MUSIC_TRACK_TITLE:
        music_cursor = &TitleMusic;
        break;
    case MUSIC_TRACK_MAIN:
        music_cursor = &MainMusic;
        break;
    case MUSIC_TRACK_DIED:
        music_cursor = &DiedMusic;
        break;
    default:
        music_cursor = 0;
        break;
    }
    if(loop) {
        repeat_point = music_cursor;
    } else {
        repeat_point = 0;
    }
    delay_counter = *(music_cursor++);
}

void tick_music() {
    unsigned char n, noteMask;
    if(audio_amplitudes[0] > 0) {
        if(audio_amplitudes[0] < 4) {
            audio_amplitudes[0] = 0;
        } else {
            audio_amplitudes[0] -= 4;
        }
        push_audio_param(AMPLITUDE, audio_amplitudes[0]);
    }

    if(audio_amplitudes[1] > 0) {
        if(audio_amplitudes[1] < 4) {
            audio_amplitudes[1] = 0;
        } else {
            audio_amplitudes[1] -= 4;
        }
        push_audio_param(AMPLITUDE+1, audio_amplitudes[1]);
    }

    if(audio_amplitudes[2] > 0) {
        audio_amplitudes[2] --;
        if(audio_amplitudes[2] == 0) {
            push_audio_param(AMPLITUDE+2, 0);
        } else {
            push_audio_param(AMPLITUDE+2, 127);
        }
    }

    if(audio_amplitudes[3] > 0) {
        audio_amplitudes[3] --;
        if(audio_amplitudes[3] == 0) {
            push_audio_param(AMPLITUDE+3, 0);
        } else {
            push_audio_param(AMPLITUDE+3, 127);
        }
    }
    
    if(music_cursor) {
        if(delay_counter > 0) {
            delay_counter--;
        } else {
            noteMask = *(music_cursor++);
            if(noteMask & 1) {     
                n = *(music_cursor++);
                if(n > 0) {
                    set_note(0, n);
                    audio_amplitudes[0] = 96;
                    push_audio_param(AMPLITUDE, 96);
                } else {
                    audio_amplitudes[0] = 0;
                    push_audio_param(AMPLITUDE, 0);
                }
            }
            if(noteMask & 2) {     
                n = *(music_cursor++);
                if(n > 0) {
                    set_note(1, n);
                    audio_amplitudes[1] = 96;
                    push_audio_param(AMPLITUDE+1, 96);
                } else {
                    audio_amplitudes[1] = 0;
                    push_audio_param(AMPLITUDE+1, 0);
                }
            }
            if(noteMask & 4) {     
                n = *(music_cursor++);
                if(n > 0) {
                    set_note(2, n);
                    audio_amplitudes[2] = 4;
                    push_audio_param(AMPLITUDE+2, 128);
                    push_audio_param(PITCHBEND+2, -16);
                } else {
                    audio_amplitudes[2] = 0;
                    push_audio_param(AMPLITUDE+2, 0);
                }
            }
            if(noteMask & 8) {     
                n = *(music_cursor++);
                if(n > 0) {
                    set_note(3, n);
                    audio_amplitudes[3] = 127;
                    push_audio_param(AMPLITUDE+3, 127);
                    push_audio_param(PITCHBEND+3, -64);
                } else {
                    audio_amplitudes[3] = 0;
                    push_audio_param(AMPLITUDE+3, 0);
                }
            }
            delay_counter = *(music_cursor++);
            if(delay_counter == 0) {
                music_cursor = repeat_point;
                if(music_cursor) {
                    delay_counter = *(music_cursor++);
                }
            }
        }
    }


    flush_audio_params();
}

void do_noise_effect(char note, char bend, char duration) {
    set_note(2, note);
    push_audio_param(PITCHBEND+2, bend);
    audio_amplitudes[2] = duration;
    push_audio_param(AMPLITUDE+2, 127);
    flush_audio_params();
}