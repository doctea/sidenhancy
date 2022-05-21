#ifndef SIDSPEC__INCLUDED
#define SIDSPEC__INCLUDED

#include <Arduino.h>

#define MAX_CUTOFF      4095    // 12 bit cutoff
#define MAX_RESONANCE   15      // 4-bit resonance
#define MAX_VOLUME      15      // 4-bit volume
#define MAX_ATTACK      15
#define MAX_DECAY       15
#define MAX_SUSTAIN     15
#define MAX_RELEASE     15

#define MAX_FREQ sidinote[126]

struct voicemap {
    byte FREQLO;    // voice frequency low byte
    byte FREQHI;    // voice frequency high byte
    byte PWLO;      // pulsewidth low byte
    byte PWHI;      // pulsewidth highi byte
    byte CR;        // waveform types + gate
    byte AD;        // attack + decay
    byte SR;        // sustain + release
};

struct sidmap {
    voicemap voice0;
    voicemap voice1;
    voicemap voice2;
    byte FCLO;      // filter cutoff low byte
    byte FCHI;      // filter cutoff byte
    byte RESFILT;   // resonance and which voices are filtered
    byte MODEVOL;   // filter modes and overall volume
    byte POTX;
    byte POTY;
    byte OSC3;
    byte ENV3;
};


// thanks http://www.sidmusic.org/sid/sidtech2.html
// per-voice envelope + oscillator options
enum ENVOSC {
    gateMask = 0b00000001,  // When this bit is set to one, the envelope generator is gated (triggered) and the ATTACK/DECAY/SUSTAIN cycle is initiated.
    syncMask = 0b00000010,  // synchronizes the fundamental frequency of oscillator 1 with the fundamental frequency of oscillator 3, producing "Hard Sync" effects
    ringMask = 0b00000100,  // replaces the triangle waveform of output of oscillator 1 with a "Ring Modulated" combination of oscillators 1 and 3
    testMask = 0b00001000,  // resets and locks oscillator 1 at zero until the TEST bit is cleared.
    triMask  = 0b00010000,  // The triangle waveform is low in harmonics and has a mellow, flute-like quality.
    sawMask  = 0b00100000,  // The sawtooth waveform is rich in even and odd harmonics and has a bright, brassy quality.
    pulseMask= 0b01000000,  // Pulse waveform, affected by pulse width registers
    noiseMask= 0b10000000,  // This output is a random signal which changes at the frequency of oscillator
};

//Bits 4-7 of this register (RES0-RES3) control the resonance of the filter. There are 16 resonance settings ranging linearly from no resonance (0) to maximum resonance (15 or $F).
//Bits 0-3 determine which signals will be routed through the Filter:
enum RESFILT {
    filt1   =   0b00000001, // set to filter voice 1 
    filt2   =   0b00000010, // set to filter voice 2
    filt3   =   0b00000100, // set to filter voice 3
    filtex  =   0b00001000, // set to filter external input
};

// per-chip filter options
// bits 0-3 select volume (0-15)
// Bits 4-7 of this register select various filter mode and output options:
enum MODEVOL {
    filter_lowpass =   0b00010000,  // the low-pass output of the filter is selected and sent to the audio output 
    filter_bandpass =  0b00100000,  // the band-pass output of the filter is selected and sent to the audio output
    filter_highpass =  0b01000000,  // the high-pass output of the filter is selected and sent to the audio output
    filter_off3 =      0b10000000,  // voice 3 is disconnected from direct audio path, Setting voice 3 to bypass the filter (FILT 3 = 0) and setting 3 OFF to a one prevents voice 3 from reaching the audio output. allowing to use voice 3 for modulation purposes (eg with ringmask)
};


#endif