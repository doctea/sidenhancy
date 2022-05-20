#!/usr/bin/python3

# A440 = midi note 69

MIDI_BASE_NOTE = 69 - 12

BASEFREQ = 440             # This is the Hz for the standard A note.
NOTE = 0	                # This is the note relative to the standard A. 0 = standard A itself, -1 = G# etc. 
STEPS_PER_OCTAVE = 12	    #Normally we use 12 notes per octave.

PAL_PHI = 985248
NTSC_PHI = 1022727 #//This is for machines with 6567R8 VIC. 6567R56A is slightly different.

#CONSTANT = round(pow(256,3) / NTSC_PHI,2)  # //Select the constant appropriate for your machine (PAL vs NTSC).
CONSTANT = pow(256,3) / PAL_PHI  # //Select the constant appropriate for your machine (PAL vs NTSC).

print("// MIDI_BASE_NOTE of %s" % MIDI_BASE_NOTE)
print("// BASEFREQ of %s" % BASEFREQ)
print("// using PAL_PHI of %s" % 985248)
print("// constant is %s" % CONSTANT)

print("double sidinote[] = {")

for i in range(0,127):
    #print("// for midi note %s" % i, end='\t')

    # ok so first midi note is MIDI_BASE_NOTE - 69
    # so when i = 0 note should be -69
    # when i = 1 note should be -68

    note = i - MIDI_BASE_NOTE

    FREQ_HZ = BASEFREQ * pow(2,note/STEPS_PER_OCTAVE)

    #print("// freq_hz is %s" % FREQ_HZ, end='\t')

    """SID_FREQ = round(CONSTANT * FREQ_HZ) # //Calculate SID freq for a certain note (specified in Hz).

    if (SID_FREQ > pow(2,16)):
        break
    #print("// so SID value is %s" % SID_FREQ)
    print ("\t%s, // note %s freq %s" % (SID_FREQ, i, FREQ_HZ))"""

    print ("\t%s, // note %s freq %s" % (FREQ_HZ, i, FREQ_HZ))

print("};")