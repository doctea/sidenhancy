# sidenhancy

- Eurorack module based around the ARMSID SID chip clone
- Based around Atmega 1284p
 - oled display and rotary encoder options
 - CV inputs assignable to SID parameters
- Using [mymenu](https://github.com/doctea/mymenu) and [parameters](https://github.com/doctea/parameters)

## Hardware/PCB

- See [Wiring.md](Wiring.md) for pin wiring (unchecked / incomplete / untested!!)
- Note that **PCB FILES in pcb/ ARE NOT FINISHED, DO NOT USE THESE YET!!**

## Todo

- Parameters
  - Oscillator 1/root pitch
  - Oscillator 2 pitch
  - Oscillator 3 pitch
  - Oscillator 1 waveforms selectable 
  - Oscillator 2 waveforms selectable 
  - Oscillator 3 waveforms selectable 
  - Oscillator 1 Pulse width
  - Oscillator 2 Pulse width
  - Oscillator 3 Pulse width
  - Oscillator Detune
  - Chord
  - Filter on/off for oscillator 1
  - Filter on/off for oscillator 2
  - Filter on/off for oscillator 3
  - Filter modes LP / BP / HP
  - Filter cutoff
  - Pitch Quantization settings
- Use SID ADSR?
  - optional so module can just be used as an oscillator
  - otherwise have ADSR for each oscillator be configurable?
- ~~Menu system (using https://github.com/doctea/mymenu)~~
 - ~~Input via rotary encoder + buttons~~
   - (unreliably working with straight encoder)
   - ~~todo: i2c rotary encoder~~
 - ~~System to map assignable inputs to parameters~~
- ~~Display via i2c ss_oled~~
- CV inputs
  - Pitch
    - (working for 0-5v, quantised and not)
    - Make work with 2xPimoroni 24v ADS1015 breakouts
    - tuning / calibration functions
    - ~~three of them, one for each oscillator?~~
    - pitch modulation
    - portamento/glide
  - Filter cutoff
  - Assignable input?
- internal LFOs?
- MIDI input
- Circuit
  - Schematic
    - in progress in pcb/
  - PCB
    - in progress in pcb/
- Panel
