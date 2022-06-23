#include "gametank.h"
#include "dynawave.h"
#include "note_numbers.h"

#define PATTERN_LEN 16
char patterns[PATTERN_LEN*11] = {
    Dn4, Bf3, Gn3, Dn4, Bf3, Gn3, Dn4, Gn3, Ds4, Bf3, Gn3, Ds4, Bf3, Gn3, Ds4, Df4,
    Dn4, Bf4, Bf4, An4, An4, Af4, Af4, Gn4, Gn4, Gf4, Gf4, Fn4, En4, Gf4, En4, Ef4,
    Gn2, RST, As2, RST, An2, RST, Gs2, RST, Gn2, RST, As2, RST, An2, RST, Gs2, RST,
    En2, En3, En2, En3, En2, En3, En2, En3, An2, An3, An2, An3, An2, An3, An2, An3, 
    Af2, Af3, Af2, Af3, En2, En3, En2, En3, An2, An3, An2, An3, An2, An3, An2, An3,
    Dn2, Dn3, Dn2, Dn3, Dn2, Dn3, Dn2, Dn3, An2, An3, An2, An3, An2, An3, An2, An3,
    En4, RST, RST, RST, Cn4, RST, RST, RST, Dn4, RST, RST, RST, Bn3, RST, RST, RST,
    Cn4, RST, RST, RST, An3, RST, RST, RST, Af3, RST, RST, RST, Bn3, RST, RST, RST,
    Cn4, RST, En4, RST, An4, RST, RST, RST, Af4, RST, RST, RST, RST, RST, RST, RST,
    An2, En3, An2, En3, An2, En3, An2, En3, Af2, En3, Af2, En3, Af2, En3, Af2, En3,
    An2, En3, An2, En3, An2, En3, An2, En3, Af2, RST, En2, RST, Gf2, RST, Af2, RST,
};

char sequence[] = {
    0, 0, 0, 1, 0xFF
};

char sequence2[] = {
    2, 0xFF
};

#define BEAT_FRAMES 16
char metronome = BEAT_FRAMES;
char noteNum = 0;
char pattNum = 0;
char noteNum2 = 0;
char pattNum2 = 0;
char *currentPattern = patterns;
char *currentPattern2 = patterns;
unsigned char audio_amplitudes[4] = {0, 0, 0, 0};

void tick_music() {
    if(audio_amplitudes[0] > 0) {
        if(audio_amplitudes[0] < 17) {
            audio_amplitudes[0] = 0;
        } else {
            audio_amplitudes[0] -= 17;
            if(audio_amplitudes[0] & 1) {
                push_audio_param(PITCHBEND, 2);
            } else {
                push_audio_param(PITCHBEND, -2);
            }
        }
        push_audio_param(AMPLITUDE, audio_amplitudes[0]);
    }
    
    metronome--;
    if(metronome == 0) {
        if(currentPattern[noteNum] != RST) {
            set_note(0, (currentPattern[noteNum]-12) );
            audio_amplitudes[0] = 128;
            push_audio_param(AMPLITUDE, 128);
        }
        noteNum = (noteNum + 1) % PATTERN_LEN;
        if(noteNum == 0) {
            ++pattNum;
            if(sequence[pattNum] == 0xFF)
                pattNum = 0;
            currentPattern = &(patterns[sequence[pattNum]*PATTERN_LEN]);
        }

        if(currentPattern2[noteNum2] != RST) {
            set_note(3, (currentPattern2[noteNum2]) );
            audio_amplitudes[3] = 127;
            push_audio_param(AMPLITUDE+3, 127);
        }
        noteNum2 = (noteNum2 + 1) % PATTERN_LEN;
        if(noteNum2 == 0) {
            ++pattNum2;
            if(sequence2[pattNum2] == 0xFF)
                pattNum2 = 0;
            currentPattern2 = &(patterns[sequence2[pattNum2]*PATTERN_LEN]);
        }
        metronome = BEAT_FRAMES;

    }
    flush_audio_params();
}