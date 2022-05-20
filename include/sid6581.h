#include <Arduino.h>

#include "myfreqs.h"

#define CHECK_BIT(var,pos) ((var) & (pos))


// --------------------------------------------------------------------------
// SID6581 CONTROL REGISTERS 
// --------------------------------------------------------------------------
/*#define SID6581_REG_F1LO 0
#define SID6581_REG_F1HI 1
#define SID6581_REG_P1LO 2
#define SID6581_REG_P1HI 3
#define SID6581_REG_CTL1 4
#define SID6581_REG_AD1  5
#define SID6581_REG_SR1  6
#define SID6581_REG_F2LO 7
#define SID6581_REG_F2HI 8
#define SID6581_REG_P2LO 9
#define SID6581_REG_P2HI 10
#define SID6581_REG_CTL2 11
#define SID6581_REG_AD2  12
#define SID6581_REG_SR2  13
#define SID6581_REG_F3LO 14
#define SID6581_REG_F3HI 15
#define SID6581_REG_P3LO 16 
#define SID6581_REG_P3HI 17
#define SID6581_REG_CTL3 18
#define SID6581_REG_AD3  19
#define SID6581_REG_SR3  20
#define SID6581_REG_FCLO 21
#define SID6581_REG_FCHI 22
#define SID6581_REG_RFLT 23
#define SID6581_REG_MVOL 24
#define SID6581_REG_POTX 25
#define SID6581_REG_POTY 26
#define SID6581_REG_OSC3 27
#define SID6581_REG_ENV3 28*/

// --------------------------------------------------------------------------
// SID6581 VOICE CONTROL REGISTER BITMASK
// --------------------------------------------------------------------------
#define SID6581_MASK_OFF 0
#define SID6581_MASK_GATE 1
#define SID6581_MASK_SYNC 2
#define SID6581_MASK_RING 4
#define SID6581_MASK_TEST 8
#define SID6581_MASK_TRIANGLE 16
#define SID6581_MASK_SAWTOOTH 32
#define SID6581_MASK_SQUARE 64
#define SID6581_MASK_NOISE 128

#define SID6581_MASK_FLT_V1 1
#define SID6581_MASK_FLT_V2 2
#define SID6581_MASK_FLT_V3 4
#define SID6581_MASK_FLT_EXT 8

#define SID6581_MASK_FLT_MUTEV3 128
#define SID6581_MASK_FLT_HP 64
#define SID6581_MASK_FLT_BP 32
#define SID6581_MASK_FLT_LP 16


// implement stuff for actually reading/writing to the chip - so can "easily" rewire it
class Socket {
    public:

    bool debug = false;

    // use PORTA for data bus
    #define SID6581_PIN_D0  PIN_PA7
    #define SID6581_PIN_D1  PIN_PA6
    #define SID6581_PIN_D2  PIN_PA5
    #define SID6581_PIN_D3  PIN_PA4
    #define SID6581_PIN_D4  PIN_PA3
    #define SID6581_PIN_D5  PIN_PA2
    #define SID6581_PIN_D6  PIN_PA1
    #define SID6581_PIN_D7  PIN_PA0

    // use sort of PORTD and one pin of PORTC for address bus
    // skip PD4 + PD5 so that they can be use as oscillator pinouts
    #define SID6581_PIN_A0  PIN_PD2
    #define SID6581_PIN_A1  PIN_PD3
    #define SID6581_PIN_A2  PIN_PD6
    #define SID6581_PIN_A3  PIN_PD7
    #define SID6581_PIN_A4  PIN_PC7

    // indicator LED
    #define PIN_LED     PIN_PC4

    // control pins
    #define SID6581_PIN_RESET   PIN_PB3
    #define SID6581_PIN_RW      PIN_PB0
    #define SID6581_PIN_SEL     PIN_PB1

    #define SID6581_PIN_CLOCK   PIN_PD5


    void setup() {
        pinMode( SID6581_PIN_D0, OUTPUT );
        pinMode( SID6581_PIN_D1, OUTPUT );
        pinMode( SID6581_PIN_D2, OUTPUT );
        pinMode( SID6581_PIN_D3, OUTPUT );
        pinMode( SID6581_PIN_D4, OUTPUT );
        pinMode( SID6581_PIN_D5, OUTPUT );
        pinMode( SID6581_PIN_D6, OUTPUT );
        pinMode( SID6581_PIN_D7, OUTPUT );
        
        pinMode( SID6581_PIN_A0, OUTPUT );
        pinMode( SID6581_PIN_A1, OUTPUT );
        pinMode( SID6581_PIN_A2, OUTPUT );
        pinMode( SID6581_PIN_A3, OUTPUT );
        pinMode( SID6581_PIN_A4, OUTPUT );
        
        pinMode( SID6581_PIN_RESET, OUTPUT );
        pinMode( SID6581_PIN_RW, OUTPUT );
        pinMode( SID6581_PIN_SEL, OUTPUT );

        pinMode( SID6581_PIN_CLOCK, OUTPUT );

        setupClock();

        resetChip();
    }

    void setupClock() {
        pinMode( SID6581_PIN_CLOCK, OUTPUT );
        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1 = 0;
        OCR1A = 7;   // toggle after counting to 8
        TCCR1A |= (1 << COM1A0);   // Toggle OC1A on Compare Match.
        TCCR1B |= (1 << WGM12);    // CTC mode
        TCCR1B |= (1 << CS10);     // clock on, no pre-scaler

        delayMicroseconds(1);
    }


    void waitCycle(void) {
        /*uint8_t foo = digitalRead( SID6581_PIN_TIMER );

        while( foo != HIGH ) {
            Serial.println("Delay1");
            foo = digitalRead( SID6581_PIN_TIMER );
        }
        
        foo = digitalRead( SID6581_PIN_TIMER );
        while( foo == HIGH ) {
            Serial.println("Delay2");    
            foo = digitalRead( SID6581_PIN_TIMER );
        }*/
        delayMicroseconds(2);
    }

    void setAddress(byte what) {
        digitalWrite( SID6581_PIN_A0, CHECK_BIT(what,1)?HIGH:LOW );
        digitalWrite( SID6581_PIN_A1, CHECK_BIT(what,2)?HIGH:LOW );
        digitalWrite( SID6581_PIN_A2, CHECK_BIT(what,4)?HIGH:LOW );
        digitalWrite( SID6581_PIN_A3, CHECK_BIT(what,8)?HIGH:LOW );
        digitalWrite( SID6581_PIN_A4, CHECK_BIT(what,16)?HIGH:LOW );
    }

    void setData(byte what) {
        digitalWrite( SID6581_PIN_D0, CHECK_BIT(what,1)?HIGH:LOW );
        digitalWrite( SID6581_PIN_D1, CHECK_BIT(what,2)?HIGH:LOW );
        digitalWrite( SID6581_PIN_D2, CHECK_BIT(what,4)?HIGH:LOW );
        digitalWrite( SID6581_PIN_D3, CHECK_BIT(what,8)?HIGH:LOW );
        digitalWrite( SID6581_PIN_D4, CHECK_BIT(what,16)?HIGH:LOW );
        digitalWrite( SID6581_PIN_D5, CHECK_BIT(what,32)?HIGH:LOW );
        digitalWrite( SID6581_PIN_D6, CHECK_BIT(what,64)?HIGH:LOW );
        digitalWrite( SID6581_PIN_D7, CHECK_BIT(what,128)?HIGH:LOW );
    }

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

    void write(byte addr, byte data) {
        if (debug) Serial.printf("Writing to %i: "BYTE_TO_BINARY_PATTERN"\n", addr, BYTE_TO_BINARY(data));
        setAddress(addr);
        setData(data);
        send();
        //delayMicroseconds(2);
    }

    private:

        void send() {
            waitCycle();
            
            // Ensure chip is in WRITE mode
            digitalWrite( SID6581_PIN_RW, LOW );
            
            // Select chip
            digitalWrite( SID6581_PIN_SEL, LOW );
            
            // Wait for a high to come and go
            waitCycle();
            
            // Ensure chip is in READ mode to avoid accidental writes ? // doctea
            digitalWrite( SID6581_PIN_RW, HIGH );
            
            // Deselect chip
            digitalWrite( SID6581_PIN_SEL, HIGH );
        }

    void resetChip(void) {
        int x = 10;
        
        // Bring reset high
        digitalWrite( SID6581_PIN_RESET, HIGH );
        
        // HIGH for 2 cycles
        x = 2;
        while( x > 0 ) {
            waitCycle();
            x--;
        }
        
        digitalWrite( SID6581_PIN_RESET, LOW );
        
        // Bring reset LOW for 10 clock cycles
        x = 10;
        while( x > 0 ) {
            waitCycle();
            x--;
        }
        
        digitalWrite( SID6581_PIN_RESET, HIGH );
    }

};

struct voicemap {
    byte FREQLO;
    byte FREQHI;
    byte PWLO;
    byte PWHI;
    byte CR;
    byte AD;
    byte SR;
};

struct sidmap {
    voicemap voice0;
    voicemap voice1;
    voicemap voice2;
    byte FCLO;
    byte FCHI;
    byte RESFILT;
    byte MODEVOL;
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
    lowpass =   0b00010000,  // the low-pass output of the filter is selected and sent to the audio output 
    bandpass =  0b00100000,  // the band-pass output of the filter is selected and sent to the audio output
    highpass =  0b01000000,  // the high-pass output of the filter is selected and sent to the audio output
    off3 =      0b10000000,  // voice 3 is disconnected from direct audio path, Setting voice 3 to bypass the filter (FILT 3 = 0) and setting 3 OFF to a one prevents voice 3 from reaching the audio output. allowing to use voice 3 for modulation purposes (eg with ringmask)
};

// get current voice register offset
#define VREG(X)     (get_base_register_address() + offsetof(voicemap,X))
#define VREG2(X,Y)  (get_base_register_address() + offsetof(sidmap[X],Y))
// get chip register offset
#define CREG(X)     (get_base_register_address() + offsetof(sidmap,X))

class Voice {
    bool debug = false;

    Socket *hw;
    byte voice_number = 0;
    byte offset = 0;

    bool playing = false;
    byte curNote = 0;

    byte control = 16;
    byte attack, decay, sustain, release;
    double lastFrequency, curFrequency = 0;
    uint16_t pulseWidth;

    public:
    Voice(Socket *hw, byte voice_number) {
        this->voice_number = voice_number;
        this->hw = hw;
        //this->offset = sizeof(voicemap) * voice_number;
    }

    byte get_base_register_address() {
        return sizeof(voicemap) * this->voice_number;
    }

    // playing notes / frequency
    void playNote(byte note) {
        double freq = getFrequencyForNote(note);
        Serial.printf("note %i: changing freq to %i (gap of %i)\n", note, freq, freq-lastFrequency);
        lastFrequency = freq;
        curNote = note;
        setFrequency(freq, true);
        //updateVoiceFrequency();
        gateOn(true);
    }

    void stopNote() {
        gateOff();
        curNote = 0;
    }

    // oscillator options
    void syncOn(bool immediate = true) {
        control |= ENVOSC::syncMask;
        if (immediate) updateControl();
    }
    void syncOff(bool immediate = true) {
        control &= ~ENVOSC::syncMask;
        if (immediate) updateControl();
    }

    void ringOn(bool immediate = true) {
        control |= ENVOSC::ringMask;
        if (immediate) updateControl();
    }
    void ringOff(bool immediate = true) {
        control &= ~ENVOSC::ringMask;
        if (immediate) updateControl();
    }

    void testOn(bool immediate = true) {
        control |= ENVOSC::testMask;
        if (immediate) updateControl();
    }
    void testOff(bool immediate = true) {
        control &= ~ENVOSC::testMask;
        if (immediate) updateControl();
    }

    void triOn(bool immediate = true) {
        control |= ENVOSC::triMask;
        if (immediate) updateControl();
    }
    void triOff(bool immediate = true) {
        control &= ~ENVOSC::triMask;
        if (immediate) updateControl();
    }

    void sawOn(bool immediate = true) {
        control |= ENVOSC::sawMask;
        if (immediate) updateControl();
    }
    void sawOff(bool immediate = true) {
        control &= ~ENVOSC::sawMask;
        if (immediate) updateControl();
    }

    void pulseOn(bool immediate = true) {
        control |= ENVOSC::pulseMask;
        if (immediate) updateControl();
    }
    void pulseOff(bool immediate = true) {
        control &= ~ENVOSC::pulseMask;
        if (immediate) updateControl();
    }

    void noiseOn(bool immediate = true) {
        control |= ENVOSC::noiseMask;
        if (immediate) updateControl();
    }
    void noiseOff(bool immediate = true) {
        control &= ~ENVOSC::noiseMask;
        if (immediate) updateControl();
    }

    void updateControl() {
        // Set control register & data
        if (debug) Serial.printf("Updating control at %i with %02X\n", VREG(CR), control);
        hw->write(VREG(CR), control);
    }

    void setPulseWidth(uint16_t pw, bool immediate = true) {
        this->pulseWidth = pw & 0b0000111111111111;
        if (immediate) updatePulseWidth(); 
    }
    void updatePulseWidth() {
        hw->write(VREG(PWLO), (uint8_t) (pulseWidth & 0b0000000011111111));
        hw->write(VREG(PWHI), (uint8_t) (pulseWidth >> 8));
    }

    void updateVoiceFrequency() {
        uint16_t sid_frequency = getSIDFrequencyForFrequency(curFrequency);
        hw->write(VREG(FREQLO), (uint8_t) (0b0000000011111111 & sid_frequency));
        hw->write(VREG(FREQHI), (uint8_t) (0b0000000011111111 & (sid_frequency >> 8)));
    }
    
    // frequency calculation, either directly from passed-in double (eg from CV) or from a MIDI note
    double get_frequency_for_pitch(byte pitch) {
        return sidinote[pitch+1];   // hmmm needs to be +1, why?
    }
    uint16_t getFrequencyForNote(byte note) {
        return get_frequency_for_pitch(note);
    }
    uint16_t getSIDFrequencyForFrequency(double frequency) {
        return frequency / 0.059604645; // MAGIC from http://www.sidmusic.org/sid/sidtech2.html
    }
    void setFrequency(double frequency, bool immediate = true) {
        curFrequency = frequency;
        if (immediate) updateVoiceFrequency();
    }

    // envelopes
    void gateOn(bool immediate = true) {
        if (debug) Serial.printf("voice %i: gateOn starting with "BYTE_TO_BINARY_PATTERN, voice_number, BYTE_TO_BINARY(control));
        control |= gateMask;
        if (debug) Serial.printf(" and changed to "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(control));
        if (immediate) updateControl();
    }
    void gateOff(bool immediate = true) {
        control &= ~ENVOSC::gateMask;
        if (immediate) updateControl();
    }

    // attack and decay are packed into same byte
    void setAttack(byte attack, bool immediate = true) {
        this->attack = attack & B00001111;
        if (immediate) updateAttackDecay();
    }
    void setDecay(byte decay, bool immediate = true) {
        this->decay = decay & B00001111;
        if (immediate) updateAttackDecay();
    }
    void updateAttackDecay() {
        hw->write(VREG(AD), this->attack << 4 | this->decay);
    }
    
    // sustain and release are packed into same byte
    void setSustain(byte sustain, bool immediate = true) {
        this->sustain = sustain & B00001111;
        if (immediate) updateSustainRelease();
    }
    void setRelease(byte release, bool immediate = true) {
        this->release = release & B00001111;
        if (immediate) updateSustainRelease();
    }
    void updateSustainRelease() {
        hw->write(VREG(SR), sustain << 4 | release);
    }

    // helper function to set all ADSR params
    void setADSR(byte attack, byte decay, byte sustain, byte release) {
        setAttack(attack,false);
        setDecay(decay,false);
        setSustain(sustain,false);
        setRelease(release,false);
        updateAttackDecay();
        updateSustainRelease();
    }


    /*
    // untested realtime calculate of frequency for pitch
    double calculate_frequency_for_pitch(byte pitch) {
        int BASEFREQ = 440; //This is the Hz for the standard A note.
        int NOTE = pitch;	//This is the note relative to the standard A. 0 = standard A itself, -1 = G# etc. 
        int STEPS_PER_OCTAVE = 12;	//Normally we use 12 notes per octave.

        float freq_hz = BASEFREQ * 2^(NOTE/STEPS_PER_OCTAVE);

        #define PAL_PHI 985248
        #define CONSTANT (float)((float)(256^(3)) / (float)PAL_PHI)
        
        Serial.printf("for pitch %i, got freq_hz %i\n", pitch, freq_hz);
        Serial.printf("constant is %i\n", CONSTANT);
        Serial.printf("resulting in value %i\n", (uint16_t) ((float)CONSTANT * (float)freq_hz));

        return ((float)CONSTANT * (float)freq_hz);
    }*/

};


class SID6581 {
    bool debug = false;
    Socket hw = Socket();

    public:
    Voice voice[3] = {
        Voice(&hw,0),
        Voice(&hw,1),
        Voice(&hw,2)
    };

    SID6581() {};

    byte get_base_register_address() {
        return 0;
    }

    void waitCycle(void) {
        /*uint8_t foo = digitalRead( SID6581_PIN_TIMER );

        while( foo != HIGH ) {
            Serial.println("Delay1");
            foo = digitalRead( SID6581_PIN_TIMER );
        }
        
        foo = digitalRead( SID6581_PIN_TIMER );
        while( foo == HIGH ) {
            Serial.println("Delay2");    
            foo = digitalRead( SID6581_PIN_TIMER );
        }*/
        delayMicroseconds(2);
    }

    void resetChip(void) {
        int x = 10;
        
        // Bring reset high
        digitalWrite( SID6581_PIN_RESET, HIGH );
        
        // HIGH for 2 cycles
        x = 2;
        while( x > 0 ) {
            waitCycle();
            x--;
        }
        
        digitalWrite( SID6581_PIN_RESET, LOW );
        
        // Bring reset LOW for 10 clock cycles
        x = 10;
        while( x > 0 ) {
            waitCycle();
            x--;
        }
        
        digitalWrite( SID6581_PIN_RESET, HIGH );
    }

    void setup() {
        hw.setup();

        resetChip();

        /*hw.write(0,255);
        hw.write(1,255);
        hw.write(6,240);
        hw.write(6+14,240);*/

        /*voice[0].setFrequency(255<<8 + 255, true);
        voice[0].setSustain(15);
        voice[0].setRelease(7, true);

        voice[2].setSustain(15);
        voice[2].setRelease(15, true);*/

        hw.write(23,0xF7); //1ch filter
        hw.write(24,0x0F); //volume

        hw.write(CREG(RESFILT),0xF0);
        hw.write(CREG(MODEVOL),0x0F);

        voice[0].triOn(true);
        voice[1].sawOn(true);
        voice[1].sawOn(true);
        voice[2].sawOn(true);
        //voice[2].pulseOn(true);

        voice[0].setADSR(1,8,15,10);
        voice[1].setADSR(1,8,15,10);
        voice[2].setADSR(1,8,15,10);

        /*voice[0].setADSR(1,1,15,2);
        voice[1].setADSR(4,4,8,2);
        voice[2].setADSR(8,8,4,2);*/

        voice[0].gateOn(true);
        delay(1000);
        voice[1].gateOn(true);
        delay(1000);
        voice[2].gateOn(true);

        delay(1000);
        voice[0].gateOff(true);
        delay(1000);
        voice[1].gateOff(true);
        delay(1000);
        voice[2].gateOff(true);
        delay(1000);

        //test_tones();            
    }

    void tone(unsigned short i) {
        byte voice3_base = 14;
        #define FREQLO 0
        #define FREQHI 1
        #define CR 4
        hw.write(0+voice3_base,i);
        hw.write(FREQHI+voice3_base,i>>8);
        hw.write(CR+voice3_base,17);
        delay(200);
        hw.write(CR+voice3_base,16);
        delay(300);
    }

    void test_tones() {
        hw.write(23,0xF1); //1ch filter
        hw.write(24,0x0F); //volume
    
        #define T1khz  16777U
        #define T2khz (16777U*2U)
        #define T3khz (16777U*3U)
        tone(T2khz);
        tone(T1khz);
        tone(T1khz);
        tone(T3khz);
    }

    void playNote(byte vn, byte note) {
        voice[vn].playNote(note);
    }
    void stopNote(byte vn) {
        voice[vn].stopNote();
    }

};