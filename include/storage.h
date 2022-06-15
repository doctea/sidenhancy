#ifndef STORAGE__INCLUDED
#define STORAGE__INCLUDED

#include "Adafruit_EEPROM_I2C.h"
#include <LinkedList.h>
#include "Parameter.h"
#include "ParameterInput.h"

extern Adafruit_EEPROM_I2C i2ceeprom;

//You've recently bought I2C Non-Volatile FRAM Breakout - 256Kbit / 32KByte

#define MAGIC_SLOT_HEADER   { 0x0F,0x0E,0x0E,0x0D }
#define MAGIC_PARAMETER     { 0x0D,0x0E,0x0A,0x0D }
#define MAGIC_INPUT         { 0x0F,0x0A,0x0D,0x0E }

struct SaveSlotHeader {
    byte magic[4] = MAGIC_SLOT_HEADER;
    byte num_params;
    byte num_inputs;
};

struct ParameterSetting {
    byte magic[4] = MAGIC_PARAMETER;
    char label[20]; // for matching up
    double value = 0;
};

struct InputParameterSetting {
    byte magic[4] = MAGIC_INPUT;
    //char label[20];
    char name;
    char parameter_label[20];
    //double internal_value;
    bool inverted = false;
    bool map_unipolar = false;
};

#define EEPROM_ADDR 0x50  // the default address!

#define SLOT_SIZE 1024

void setup_storage();
void save_parameter_settings(LinkedList<BaseParameter*> *parameters, LinkedList<BaseParameterInput*> *inputs, int memory_offset = 0);
bool load_parameter_settings(LinkedList<BaseParameter*> *parameters, LinkedList<BaseParameterInput*> *inputs, int memory_slot = 0);
int find_parameter_index_for_label(LinkedList<BaseParameter*> *parameters, char *label);
int find_input_index_for_label(LinkedList<BaseParameterInput*> *inputs, char name);


#endif