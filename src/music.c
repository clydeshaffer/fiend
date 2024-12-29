#include "music.h"
#include "gametank.h"
#include "dynawave.h"
#include "note_numbers.h"
#include "banking.h"

extern const unsigned char* MainMusic;
extern const unsigned char* SecondMusic;
extern const unsigned char* ThirdMusic;
extern const unsigned char* FourthMusic;
extern const unsigned char* TitleMusic;
extern const unsigned char* DiedMusic;
extern const unsigned char* StairsMusic;
extern const unsigned char* BossMusic;
extern const unsigned char* BossMusic2;
extern const unsigned char* EndMusic;
extern const unsigned char* PickupMusic;
extern const unsigned char* FanfareMusic;
extern const unsigned char* MapItemMusic;
extern const unsigned char* ConsoleJingle;
unsigned char audio_amplitudes[4] = {0, 0, 0, 0};
unsigned char* music_cursor = 0;
unsigned char delay_counter = 0;

unsigned char* repeat_point;
unsigned char* paused_cursor = 0;
unsigned char paused_delay;
unsigned char music_mode = REPEAT_NONE;
unsigned char repeat_resume_pending = 0;

void init_music() {
    music_cursor = 0;
    delay_counter = 0;
}

void play_track(char track, char loop) {
    char *prev_cursor = music_cursor;
    ChangeRomBank(BANK_COMMON);
    switch (track)
    {
    case MUSIC_TRACK_TITLE:
        music_cursor = &TitleMusic;
        break;
    case MUSIC_TRACK_MAIN:
        music_cursor = &MainMusic;
        break;
    case MUSIC_TRACK_AREA2:
        music_cursor = &SecondMusic;
        break;
    case MUSIC_TRACK_AREA3:
        music_cursor = &ThirdMusic;
        break;
    case MUSIC_TRACK_AREA4:
        music_cursor = &FourthMusic;
        break;
    case MUSIC_TRACK_DIED:
        music_cursor = &DiedMusic;
        break;
    case MUSIC_TRACK_STAIRS:
        music_cursor = &StairsMusic;
        break;
    case MUSIC_TRACK_BOSS:
        music_cursor = &BossMusic;
        break;
    case MUSIC_TRACK_BOSS2:
        music_cursor = &BossMusic2;
        break;
    case MUSIC_TRACK_END:
        music_cursor = &EndMusic;
        break;
    case MUSIC_TRACK_PICKUP:
        music_cursor = &PickupMusic;
        break;
    case MUSIC_TRACK_FANFARE:
        music_cursor = &FanfareMusic;
        break;
    case MUSIC_TRACK_MAP_ITEM:
        music_cursor = &MapItemMusic;
        break;
    case MUSIC_TRACK_CONSOLE_JINGLE:
        music_cursor = &ConsoleJingle;
        break;
    default:
        music_cursor = 0;
        break;
    }

    switch(loop) {
        case REPEAT_NONE:
            repeat_point = 0;
            music_mode = REPEAT_NONE;
            repeat_resume_pending = 0;
            break;
        case REPEAT_LOOP:
            repeat_point = music_cursor;
            music_mode = REPEAT_LOOP;
            repeat_resume_pending = 0;
            break;
        case REPEAT_RESUME:
            if(!repeat_resume_pending) {
                paused_cursor = prev_cursor;
                paused_delay = delay_counter;
                repeat_resume_pending = 1;
            }
            break;
    }

    if(music_cursor) {
        delay_counter = *(music_cursor++);
    }
}

void pause_music() {
    paused_cursor = music_cursor;
    paused_delay = delay_counter;
    music_cursor = 0;
}

void unpause_music() {
    music_cursor = paused_cursor;
    delay_counter = paused_delay;
    paused_cursor = 0;
}

#pragma codeseg (push, "CODE2");
void tick_music() {
    unsigned char n, noteMask;
    ChangeRomBank(BANK_COMMON);
    if(audio_amplitudes[0] > 0) {
        audio_amplitudes[0]--;
        push_audio_param(AMPLITUDE, audio_amplitudes[0]);
    }

    if(audio_amplitudes[1] > 0) {
        audio_amplitudes[1]--;
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
                    audio_amplitudes[0] = 64;
                    push_audio_param(AMPLITUDE, 64);
                } else {
                    audio_amplitudes[0] = 0;
                    push_audio_param(AMPLITUDE, 0);
                }
            }
            if(noteMask & 2) {     
                n = *(music_cursor++);
                if(n > 0) {
                    set_note(1, n);
                    audio_amplitudes[1] = 64;
                    push_audio_param(AMPLITUDE+1, 64);
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
                    push_audio_param(AMPLITUDE+2, 64);
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
                    audio_amplitudes[3] = 63;
                    push_audio_param(AMPLITUDE+3, 63);
                } else {
                    audio_amplitudes[3] = 0;
                    push_audio_param(AMPLITUDE+3, 0);
                }
            }
            delay_counter = *(music_cursor++);
            if(delay_counter == 0) {
                if(repeat_resume_pending != 0) {
                    repeat_resume_pending = 0;
                    music_cursor = paused_cursor;
                    delay_counter = paused_delay;
                    paused_cursor = 0;
                } else {
                    music_cursor = repeat_point;
                    if(music_cursor) {
                        delay_counter = *(music_cursor++);
                    }   
                }
            }
        }
    }


    flush_audio_params();
}
#pragma codeseg (pop);

void do_noise_effect(char note, char bend, char duration) {
    set_note(2, note);
    push_audio_param(PITCHBEND+2, bend);
    audio_amplitudes[2] = duration;
    push_audio_param(AMPLITUDE+2, 127);
    flush_audio_params();
}

void stop_music() {
    music_cursor = 0;
    audio_amplitudes[0] = 0;
    audio_amplitudes[1] = 0;
    audio_amplitudes[2] = 0;
    audio_amplitudes[3] = 0;
    push_audio_param(AMPLITUDE+0, 0);
    push_audio_param(AMPLITUDE+1, 0);
    push_audio_param(AMPLITUDE+2, 0);
    push_audio_param(AMPLITUDE+3, 0);
    flush_audio_params();
}