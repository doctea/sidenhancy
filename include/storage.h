#include "Adafruit_EEPROM_I2C.h"
#include <LinkedList.h>
#include "Parameter.h"
#include "ParameterInput.h"

//You've recently bought I2C Non-Volatile FRAM Breakout - 256Kbit / 32KByte

Adafruit_EEPROM_I2C i2ceeprom;

#define MAGIC_SLOT_HEADER   { 0xF,0xE,0xE,0xD }
#define MAGIC_PARAMETER     { 0xD,0xE,0xA,0xD }
#define MAGIC_INPUT         { 0xF,0xA,0xD,0xE }

struct SaveSlotHeader {
    byte magic[4] = MAGIC_SLOT_HEADER;
    byte num_params;
    byte num_inputs;
};

struct ParameterSetting {
    byte magic[8] = MAGIC_PARAMETER;
    char label[20]; // for matching up
    double value = 0;
};

struct InputParameterSetting {
    byte magic[8] = MAGIC_INPUT;
    //char label[20];
    char name;
    char parameter_label[20];
    //double internal_value;
    bool inverted = false;
    bool map_unipolar = false;
};

#define EEPROM_ADDR 0x50  // the default address!

#define SLOT_SIZE 1024

int find_parameter_index_for_label(LinkedList<BaseParameter*> *parameters, char *label) {
    int size = parameters->size();
    for (int i = 0 ; i < size ; i++) {
        if (!strcmp(parameters->get(i)->label, label))
            return i;
    }
    Serial.printf("WARNING in storage: find_parameter_index_for_label: didn't find one for '%s'?\n", label);
    return -1;
}
int find_input_index_for_label(LinkedList<BaseParameterInput*> *inputs, char name) {
    int size = inputs->size();
    for (int i = 0 ; i < size ; i++) {
        if (inputs->get(i)->name == name)
            return i;
    }
    Serial.printf("WARNING in storage: find_input_index_for_name: didn't find one for '%c'?\n", name);
    return -1;
}


bool load_parameter_settings(LinkedList<BaseParameter*> *parameters, LinkedList<BaseParameterInput*> *inputs, int memory_slot = 0) {
    byte magic_header[4] = MAGIC_SLOT_HEADER; 
    byte magic_parameter[8] = MAGIC_PARAMETER; 
    byte magic_input[8] = MAGIC_INPUT;

    SaveSlotHeader header;
    int memory_offset = SLOT_SIZE * memory_slot;
    
    i2ceeprom.read(memory_offset, (uint8_t*)&header, sizeof(header));
    for (int i = 0 ; i < sizeof(header.magic) ; i++) {
        if (header.magic[i]!=magic_header[i]) {
            Serial.printf("ERROR reading memory slot %i from address %02x: Magic header not found! (expected %02x, got %02x at %i)\n",
                                memory_slot, memory_offset, header.magic[i], magic_header[i], i
            );
            return false;
        }
    }
    Serial.printf("Found slot magic header for slot %i at address %i!\n", memory_slot, memory_offset);

    memory_offset += sizeof(header);

    Serial.printf("Header says there are %i parameters and %i inputs to read...\n", header.num_params, header.num_inputs);

    // read up to size of parameters values
    int num_params_to_read = min(header.num_params, parameters->size());
    if (header.num_params!=parameters->size())   Serial.printf("WARNING: found %i parameters to load, but current available_parameters is only %i big - only loading first %i!\n", header.num_params, parameters->size(), num_params_to_read);
    ParameterSetting        read_params[num_params_to_read];
    for (int i = 0 ; i < parameters->size() ; i++) {
        i2ceeprom.read(memory_offset, (uint8_t*)&read_params[i], sizeof(ParameterSetting));
        for (int m = 0 ; m < sizeof(read_params[i].magic) ; m++) {
            if (read_params[i].magic[m] != magic_parameter[m]) {
                Serial.printf("ERROR reading param %i from address %02x: Magic header not found! (expected %02x, got %02x at %i)\n",
                                    i, memory_offset, read_params[i].magic[m], magic_parameter[m], m
                );
                return false;
            }
        }
        memory_offset += sizeof(ParameterSetting);
    }

    // skip any remaining parameters mentioned in the save header that dont exist in the current parameter list
    for (int i = num_params_to_read ; i < header.num_params ; i++) {
        Serial.println("Skipping a parameter.. ");
        memory_offset += sizeof(ParameterSetting);
    }
    Serial.println("");


    // read up to size of inputs values
    int num_inputs_to_read = min(header.num_inputs, inputs->size());
    if (header.num_inputs!=inputs->size())       Serial.printf("WARNING: found %i inputs to load, but current available_parameters is only %i big - only loading first %i!\n", header.num_inputs, inputs->size(), num_inputs_to_read);
    InputParameterSetting   read_inputs[num_inputs_to_read];
    for (int i = 0 ; i < parameters->size() ; i++) {
        i2ceeprom.read(memory_offset, (uint8_t*)&read_inputs[i], sizeof(ParameterSetting));
        for (int m = 0 ; m < sizeof(read_inputs[i].magic) ; m++) {
            if (read_inputs[i].magic[m] != magic_input[m]) {
                Serial.printf("ERROR reading input %i from address %02x: Magic header not found! (expected %02x, got %02x at %i)\n",
                                    i, memory_offset, read_inputs[i].magic[m], magic_input[m], m
                );
                return false;
            }
        }
        memory_offset += sizeof(ParameterSetting);
    }

    // skip any remaining inputs mentioned in the save header that dont exist in the current parameter list
    for (int i = num_inputs_to_read ; i < header.num_inputs ; i++) {
        Serial.println("Skipping an input.. ");
        memory_offset += sizeof(ParameterSetting);
    }
    Serial.println("");

    Serial.println("Magics checked out, loaded expected count - now set the avails!");

    Serial.println("Restoring Parameter values...\n");
    for (int i = 0 ; i < num_params_to_read ; i++) {
        int index = find_parameter_index_for_label(parameters, read_params[i].label);
        if (index!=-1) {
            Serial.printf("\tRestoring value ");
            Serial.print(read_params[i].value);
            Serial.printf(" to Parameter '%s' at index '%i\n", read_params[i].label, index);
            parameters->get(index)->setParamValue(read_params[i].value);
        }
    }
    Serial.println("Restoring ParameterInput settings and linkages...\n");
    for (int i = 0 ; i < num_inputs_to_read ; i++) {
        int index_input = find_input_index_for_label(inputs, read_inputs[i].name);
        int index_param = find_parameter_index_for_label(parameters, read_inputs[i].parameter_label);
        if (index_input==-1) {
            Serial.printf("ERROR: no input with name '%c' found!", read_inputs[i].name);
            return false;
        }
        if (index_param==-1) {
            Serial.printf("ERROR: no input with label '%s' found!", read_inputs[i].parameter_label);
            return false;
        }
        Serial.printf("\tFound input index %i and param index %i for %c and %s\n", index_input, index_param, read_inputs[i].name, read_inputs[i].parameter_label);
        BaseParameterInput *input = inputs->get(index_input);
        BaseParameter *param = parameters->get(index_param);
        input->setTarget(param);
        input->map_unipolar = read_inputs[i].map_unipolar;
        input->inverted = read_inputs[i].inverted;
        Serial.println("\tRestored settings and linkages!");
    }

    Serial.println("------- Restore finished!");

    return true;
}

void save_parameter_settings(LinkedList<BaseParameter*> *parameters, LinkedList<BaseParameterInput*> *inputs, int memory_offset = 0) {
    byte buffer[sizeof(SaveSlotHeader) + (parameters->size() * sizeof(ParameterSetting)) + (inputs->size() * sizeof(InputParameterSetting))];

    SaveSlotHeader header;
    header.num_params = (uint16_t)parameters->size();
    header.num_inputs = (uint16_t)inputs->size();

    ParameterSetting        param_settings[header.num_params]; 
    memset(&param_settings, 0, sizeof(param_settings));
    for (int i = 0 ; i < parameters->size() ; i++) {
        DataParameter *p = (DataParameter*)parameters->get(i);
        param_settings[i].value = p->getCurrentValue();
        strcpy(param_settings[i].label, p->label);
        Serial.printf("Saving Parameter %i: '%s'\n", i, param_settings[i].label); //, param_settings[i].parameter_label);
    }
    Serial.println("---------");

    InputParameterSetting   input_settings[header.num_inputs];
    memset(&input_settings, 0, sizeof(input_settings));
    for (int i = 0 ; i < inputs->size() ; i++) {
        BaseParameterInput *inp = (BaseParameterInput*)inputs->get(i);
        //strcpy(input_settings[i].label, (char*)inp->name);
        input_settings[i].name = inp->name;
        if (inp->target_parameter!=nullptr)
            strcpy(input_settings[i].parameter_label, inp->target_parameter->label);
        else   
            strcpy(input_settings[i].parameter_label, "None");
        Serial.printf("Saving ParameterInput %i: %c linked to parameter with label '%s'\n", i, input_settings[i].name, input_settings[i].parameter_label);
        input_settings[i].inverted = inp->inverted;
        input_settings[i].map_unipolar = inp->map_unipolar;
    }
    Serial.println("---------");

    memory_offset *= SLOT_SIZE;    // start 16
    int total_size = sizeof(header)+sizeof(param_settings)+sizeof(input_settings);
    Serial.printf("Got header of size %i, param_settings of size %i, input_settings of size %i - total is %i to be written starting at %i!\n",
                        sizeof(header), sizeof(param_settings), sizeof(input_settings), total_size, memory_offset
    );

    int position = memory_offset;
    position += i2ceeprom.write(position, (uint8_t*)&header, sizeof(header));
    position += i2ceeprom.write(position, (uint8_t*)&param_settings, sizeof(param_settings));
    position += i2ceeprom.write(position, (uint8_t*)&input_settings, sizeof(input_settings));
    
    Serial.printf("Finished writing at top address %i\n", position);

    // test reading back
    Serial.printf("Reading back, starting at %i...\n", memory_offset);
    char row[9];
    int count = 0;
    for (int i = memory_offset ; i < memory_offset+total_size; i++) {
        int v = i2ceeprom.read(i);
        if (v>=0x20 && v<=0x7E) {
            row[count%8] = v; 
        } else if (v==0) {
            row[count%8] = ' '; 
        } else { 
            row[count%8] = '.';
        }
        //row[i%8] = (v>=0x20 && v<=0x7E) ? v : (v==0 ? ' ' : '.');

        count++;

        Serial.printf("%02x ", v, i2ceeprom.read(i));
        if (count>0 && count%8==0) { 
            row[8] = '\0'; 
            Serial.printf("%s\n", row); 
        }
    }

    Serial.printf("DONE!");
}

void setup_storage() {
  if (i2ceeprom.begin(0x50)) {  // you can stick the new i2c addr in here, e.g. begin(0x51);
    Serial.println("Found I2C EEPROM");
  } else {
    Serial.println("I2C EEPROM not identified ... check your connections?\r\n");
    //while (1) delay(10);
  }
}