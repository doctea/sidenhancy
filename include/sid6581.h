#ifndef SID6581__INCLUDED
#define SID6581__INCLUDED

#include <Arduino.h>

#include "hardware.h"

#include "myfreqs.h"

#include "sidspec.h"

//#include "ads.h"    // for get_voltage_for_frequency

// get current voice register offset
#define VREG(X)     (get_base_register_address() + offsetof(voicemap,X))
#define VREG2(X,Y)  (get_base_register_address() + offsetof(sidmap[X],Y))
// get chip register offset
#define CREG(X)     (get_base_register_address() + offsetof(sidmap,X))

extern bool debug_sid;

class Voice {

    Socket *hw;
    byte voice_number = 0;
    byte offset = 0;

    bool playing = false;
    byte curNote = 0;

    byte control = 16;
    byte attack, decay, sustain, release;
    double lastFrequency, curFrequency = 0;

    public:

    uint16_t pulseWidth;

    Voice(Socket *hw, byte voice_number) {
        this->voice_number = voice_number;
        this->hw = hw;
        //this->offset = sizeof(voicemap) * voice_number;
    }

    void printStatus() {
        Serial.printf(F("Voice %i :: "), voice_number);
        Serial.printf(F("Att=%2i,Dec=%2i,Sus=%2i,Rel=%2i :: "), attack, decay, sustain, release);
        Serial.printf(F("PW=%2X :: "), pulseWidth);
        Serial.printf(F("Freq=")); //%5u :: "), curFrequency);
        Serial.print(curFrequency);
        Serial.print(" :: ");
        Serial.printf(F("Control=[" BYTE_TO_BINARY_PATTERN "]"), BYTE_TO_BINARY(control));
        Serial.println();
    }

    byte get_base_register_address() {
        return sizeof(voicemap) * this->voice_number;
    }

    void updateAll() {
        updateAttackDecay();
        updateSustainRelease();
        updatePulseWidth();
        updateVoiceFrequency();
        updateControl();
    }

    // playing notes / frequency
    void playNote(byte note) {
        double freq = getFrequencyForNote(note);
        Serial.printf(F("voice %i :: note %i: changing freq to %i (gap of %i)\n"), voice_number, note, (uint16_t) freq, (uint16_t) freq-lastFrequency);
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
    void setOscMask(byte mask, bool immediate = true) {
        control |= mask;
        if (immediate) updateControl();
    }
    void unsetOscMask(byte mask, bool immediate = true) {
        control &= ~mask;
        if (immediate) updateControl();
    }

    bool isWaveform(byte mask) {
        return (mask & control) == mask;
    }

    void setOsc(double oscMode) {
        byte o = oscMode*15;
        setOscMask(o<<4);
    }

    void updateControl() {
        // Set control register & data
        if (debug_sid) Serial.printf(F("Updating control at %i with 0x%02X\n"), VREG(CR), control);
        hw->write(VREG(CR), control, (char*)"CR");
    }

    float pulsewidth_modulate = 0.0f;
    void setPulseWidth(uint16_t pw, bool immediate = true) {
        this->pulseWidth = pw & 0b0000111111111111;
        if (immediate) updatePulseWidth(); 
    }
    void updatePulseWidth() {
        int16_t pw = pulseWidth & 0b0000111111111111;   // 12 bit
        
        int16_t modulation = pulsewidth_modulate * (4095/2);
        pw += modulation;

        pw = constrain(pw, 0, 4095);

        hw->write(VREG(PWHI), (uint8_t) ((pw & 0b0000111111111111) >> 8), (char*)"PWHI");
        hw->write(VREG(PWLO), (uint8_t) (pw & 0b0000000011111111), (char*)"PWLO");
    }
    void modulatePulseWidth(float normal) {
        pulsewidth_modulate = normal;
        //if (debug) Serial.printf("Modulating pulsewidths by %i\n", (uint16_t)(pulsewidth_modulate*(4095/2)));
        updatePulseWidth();
    }

    float pitch_modulate = 0.0f;
    void modulatePitch(float normal) {
        this->pitch_modulate = normal;
        updateVoiceFrequency();
    }

    double get_modulated_frequency(double frequency, double modulation_normal) {
        if (modulation_normal == 0.0)
            return frequency;

        if (modulation_normal>0.0) {
            // get modulation vs the note up from frequency
            // To get the frequency a semitone up from A4 we multiply 440 Hz by the twelfth root of two
            // n'th root of x is given by pow(x,1.0/n)
            /*To do this on a (scientific) calculator, all you need to remember is that there are 1200 cents to an octave. The formula is:
                a*2^(c/1200) = b.

                For example, using the information from Dave's table, we can convert 19.56 cents to Hz for a 440.
                Here a = 440, c = 19.56 and b is what we want to know.

                440*2^(19.56/1200) = 444.9994 Hz.*/
            //double frequency_target = (pow(frequency, 1.0/12.0));
            double frequency_target = frequency * (pow(2.0,(modulation_normal*100.0)/1200.0));
            //Serial.printf("For frequency %i, found what should be frequency one semitone above, ==%i\n", (uint32_t) frequency, (uint32_t) frequency_target);
            return (frequency_target - frequency) * modulation_normal;
        } else {
            // get modulation vs the note down from frequency
            //modulation_normal = 1.0 - (modulation_normal*-1.0);
            // almost certainly not the correct maths
            //double frequency_target = frequency_target - (pow(frequency, 1.0/12.0));
            //return frequency - ((frequency - frequency_target) * modulation_normal);
            //TODO: almost certainly wrong
            double frequency_target = frequency + (frequency - frequency * (pow(2.0,(modulation_normal*100.0)/1200.0)));
            return (frequency_target - frequency) * modulation_normal;
        }
    }

    void updateVoiceFrequency() {
        // get the frequency range of a semitone at the current frequency...
        uint16_t effective_freq = curFrequency + get_modulated_frequency(curFrequency, pitch_modulate);
        uint16_t sid_frequency = getSIDFrequencyForFrequency(effective_freq);
        hw->write(VREG(FREQLO), (uint8_t) (0b0000000011111111 & sid_frequency), (char*)"FREQLO");
        hw->write(VREG(FREQHI), (uint8_t) (0b0000000011111111 & (sid_frequency >> 8)), (char*)"FREQHI");
    }
    
    // frequency calculation, either directly from passed-in double (eg from CV) or from a MIDI note
    double get_frequency_for_pitch(byte pitch) {
        return sidinote[pitch+1];   // hmmm needs to be +1, why?
    }
    double getFrequencyForNote(byte note) {
        return get_frequency_for_pitch(note);
    }
    uint16_t getSIDFrequencyForFrequency(double frequency) {
        return frequency / 0.059604645; // MAGIC from http://www.sidmusic.org/sid/sidtech2.html
    }
    void setFrequency(double frequency) {
        this->setFrequency(frequency, true);
    }
    void setFrequency(double frequency, bool immediate = true) {
        curFrequency = frequency;
        if (immediate) updateVoiceFrequency();
    }

    // envelopes
    void gateOn(bool immediate = true) {
        if (debug_sid) Serial.printf(F("voice %i: gateOn starting with " BYTE_TO_BINARY_PATTERN), voice_number, BYTE_TO_BINARY(control));
        control |= gateMask;
        if (debug_sid) Serial.printf(F(" and changed to " BYTE_TO_BINARY_PATTERN "\n"), BYTE_TO_BINARY(control));
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
        hw->write(VREG(AD), attack << 4 | decay, (char*)"AD");
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
        hw->write(VREG(SR), sustain << 4 | release, (char*)"SR");
    }

    // helper function to set all ADSR params
    void setADSR(byte attack, byte decay, byte sustain, byte release, bool immediate = true) {
        setAttack(attack,false);
        setDecay(decay,false);
        setSustain(sustain,false);
        setRelease(release,false);
        if (immediate) {
            updateAttackDecay();
            updateSustainRelease();
        }
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

    void printStatus() {
        Serial.println("-----status");
        Serial.printf(F("Vol=%2i :: "), volume);
        Serial.printf(F("Cutoff=%4i :: "), cutoff);
        Serial.printf(F("Res=%2i :: "), resonance);
        Serial.printf(F("filter_type=[" BYTE_TO_BINARY_PATTERN "] :: "), BYTE_TO_BINARY(filter_type_mask));
        Serial.printf(F("filter_voices=[" BYTE_TO_BINARY_PATTERN "] :: "), BYTE_TO_BINARY(filter_voices));
        if (filter_lowpass  & filter_type_mask) Serial.print(F("LP "));
        if (filter_bandpass & filter_type_mask) Serial.print(F("BP "));
        if (filter_highpass & filter_type_mask) Serial.print(F("HP "));
        Serial.println();
        for (int i = 0; i < 3 ; i++) {
            voice[i].printStatus();
        }
        Serial.println(F("-status-----"));
    }

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
        Serial.println(F("------resetChip------"));
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

        setVolume(15,false);
        unsetFilterVoice(0,false);
        unsetFilterVoice(1,false);
        unsetFilterVoice(2,false);
        unsetFilterVoice(3);

        updateAll();
        Serial.println(F("------end resetChip------"));
    }

    byte resonance = MAX_RESONANCE / 2;
    byte filter_voices;

    void setResonanceD(double resonance) {
        this->setResonance((byte)(resonance * 15.0));
    }

    // update RESONANCE and filter voice ON/OFF
    void setResonance(byte value, bool immediate = true) {
        resonance = 0b00001111 & value;
        if (immediate) updateFilterVoice();
    }
    void setFilterVoice(byte number, bool immediate = true) {
        filter_voices |= 1 << number;
        Serial.printf(F("setFilterVoice(%i)\n"), number);
        if (immediate) updateFilterVoice();
    }
    void unsetFilterVoice(byte number, bool immediate = true) {
        filter_voices &= ~(1 << number);
        Serial.printf(F("unsetFilterVoice(%i)\n"), number);
        if (immediate) updateFilterVoice();
    }
    void updateFilterVoice() {
        hw.write(CREG(RESFILT), ((resonance<<4) | filter_voices), (char*)"RESFILT");
    }

    // update filter TYPE mask and VOLUME
    byte filter_type_mask;
    byte volume = MAX_VOLUME;
    void setFilterType(byte mask, bool immediate = true) {
        filter_type_mask |= mask;
        if (immediate) updateFilterType();
    }
    void unsetFilterType(byte mask, bool immediate = true) {
        filter_type_mask &= ~mask;
        if (immediate) updateFilterType();
    }
    void setVolume(byte volume, bool immediate = true) {
        Serial.printf(F("setVolume(%i) called\n"), volume);
        this->volume = 0b00001111 & volume;
        if (immediate) updateFilterType();
    }
    void updateFilterType() {
        Serial.printf(F("updateFilterVoice with filter_type_mask [%02x] and volume [%02x]\n"), filter_type_mask, volume);
        hw.write(CREG(MODEVOL), filter_type_mask | volume, (char*)"MODEVOL");
    }

    int cutoff = MAX_CUTOFF/2;
    float cutoff_modulation = 0.0f;
    void setCutoff(uint16_t cutoff, bool immediate = true) {
        //Serial.printf("Setting cutoff to %i", (cutoff));
        this->cutoff = cutoff & 0b0000111111111111;
        if (immediate) updateCutoff();
    }
    uint16_t getCutoff() {
        return this->cutoff & 0b0000111111111111;
    }
    void updateCutoff() {
        int16_t co = getCutoff();
        int16_t int_modulation = cutoff_modulation * (4096/2);
        co += int_modulation;
        hw.write(CREG(FCHI), co >> 8, (char*)"FCHI");
        hw.write(CREG(FCLO), 0b0000000000001111 & co, (char*)"FCLO");
    }
    void modulateCutoff(float normal_modulation) {  // -1.0 to 1.0
        cutoff_modulation = normal_modulation;
    }
    void setCutoff(double cutoff) {
        this->setCutoff((uint16_t)((double)cutoff * MAX_CUTOFF));
    }

    float getAllPulseWidths() {
        return voice[0].pulseWidth / 4095.0f;
    }
    void setAllPulseWidths(float normal) {
        int pw_value = normal*4095.0f;
        //Serial.printf("setAllPulseWidths() setting to %i\n", pw_value);
        for (int i = 0 ; i < 3 ; i++) {
            voice[i].setPulseWidth(pw_value);
        }
    }
    float pulsewidth_modulation = 0.0f;
    void modulateAllPulseWidths(double normal) {
        /*Serial.print("sid#modulateAllPulseWidths(");
        Serial.print(normal);
        Serial.println(")");*/
        pulsewidth_modulation = normal;
        for (int i = 0 ; i < 3 ; i++) {
            voice[i].modulatePulseWidth(normal);
        }
    }

    float pitch_modulation = 0.0f;
    void modulateAllPitches(double normal) {
        /*Serial.print("sid#modulateAllPulseWidths(");
        Serial.print(normal);
        Serial.println(")");*/
        pitch_modulation = normal;
        for (int i = 0 ; i < 3 ; i++) {
            voice[i].modulatePitch(normal);
        }
    }

    void updateAll() {
        updateFilterVoice();
        updateFilterType();
        updateCutoff();
        voice[0].updateAll();
        voice[1].updateAll();
        voice[2].updateAll();
    }

    void setAllFrequency(double frequency) {
        /*Serial.print("sid#setAllFrequency(");
        Serial.print(frequency);
        Serial.println(")");*/
        for (int i = 0 ; i < 3 ; i++) 
            voice[i].setFrequency(frequency,true);
    }
    void allGateOn() {
        for (int i = 0 ; i < 3 ; i++) 
            voice[i].gateOn();
    }
    void allGateOff() {
        for (int i = 0 ; i < 3 ; i++) 
            voice[i].gateOff();
    }

    void setup() {
        Serial.println(F("\n=================sid setup================="));
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

        //hw.write(23,0xF7); //1ch filter
        //hw.write(24,0x0F); //volume
        //hw.write(CREG)

        //hw.write(CREG(RESFILT),0xF7, "SETUP RESFILT");
        setVolume(MAX_VOLUME,false);
        //setResonance(MAX_RESONANCE,false);
        unsetFilterVoice(0,false);
        unsetFilterVoice(1,false);
        unsetFilterVoice(2,false);
        unsetFilterVoice(3,false);
        setVolume(MAX_VOLUME);
        //hw.write(CREG(MODEVOL),0x0F, "SETUP MODEVOL");
        voice[0].setADSR(1,8,15,10,false);
        voice[1].setADSR(1,8,15,10,false);
        voice[2].setADSR(1,8,15,10,false);
        //voice[0].setFrequency(MAX_FREQ/2,false);
        //voice[1].setFrequency(MAX_FREQ/3,false);
        //voice[2].setFrequency(MAX_FREQ/4,false);
        setAllFrequency(880);
        //voice[0].sawOn(true);
        //voice[0].triOn(true);
        voice[0].pulseOn(true);
        //voice[1].sawOn(true);
        //voice[1].triOn(true);
        voice[1].pulseOn(true);
        //voice[2].sawOn(true);
        voice[2].pulseOn(true);
        updateAll();

        allGateOn();
/*
        setFilterType(filter_lowpass,false);
        unsetFilterVoice(0,false);
        unsetFilterVoice(1,false);
        unsetFilterVoice(2,false);
        setResonance(MAX_RESONANCE/2,false);
        setCutoff(MAX_CUTOFF/2,false);

        voice[0].triOn(false);
        voice[1].sawOn(false);
        voice[1].sawOn(false);
        voice[2].sawOn(false);
        //voice[2].pulseOn(true);
        
        voice[0].setADSR(1,8,15,10,false);
        voice[1].setADSR(1,8,15,10,false);
        voice[2].setADSR(1,8,15,10,false);

        voice[0].setFrequency(MAX_FREQ/2);
        voice[1].setFrequency(MAX_FREQ/3);
        voice[2].setFrequency(MAX_FREQ/4);

        updateAll();
*/

        voice[0].gateOn(true);
        voice[1].gateOn(true);
        voice[2].gateOn(true);

        delay(100);
        voice[0].gateOff(true);
        delay(100);
        voice[1].gateOff(true);
        delay(100);
        voice[2].gateOff(true);
        delay(100);

        //test_tones();            

        Serial.println(F("=================end sid setup================="));
        //while(1) {};
    }

    void tone(unsigned short i) {
        byte voice3_base = 14;
        #define FREQLO 0
        #define FREQHI 1
        #define CR 4
        hw.write(0+voice3_base,i,"tone:voice3_base");
        hw.write(FREQHI+voice3_base,i>>8,"tone:voice3_freqhi");
        hw.write(CR+voice3_base,17,"tone:cr");
        delay(200);
        hw.write(CR+voice3_base,16,"tone:cr");

        /*voice[3].setFrequency(i);
        voice[3].triOn();

        voice[3].gateOn();
        delay(200);
        voice[3].gateOff();*/

        delay(300);
    }

    void test_tones() {
        //hw.write(23,0xF1); //1ch filter
        //hw.write(24,0x0F); //volume
        /*setFilterVoice(0);
        unsetFilterVoice(1);
        unsetFilterVoice(2);
        setVolume(MAX_VOLUME);*/
    
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

extern SID6581 sid;

#endif