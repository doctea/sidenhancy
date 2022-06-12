#include "menuitems.h"

#include "sid6581.h"
/*
class WaveSelectorControl : public SelectorControl {
    byte value;
    public:
        Voice *voice;
        WaveSelectorControl(char *label, Voice *voice, byte initial_value) : SelectorControl(label) {
            this->voice = voice;
            this->value = initial_value;
        }

        virtual void setter (int new_value) {
        }
        virtual int getter () {
            return voice->getControl();
        }

        virtual const char*get_label_for_value(int value) {
            static char value_label[20];
            sprintf(value_label, "%i", value);
            return value_label;
        }
};*/