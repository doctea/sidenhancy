#include "Adafruit_EEPROM_I2C.h"
#include <LinkedList.h>
#include "Parameter.h"
#include "ParameterInput.h"

//You've recently bought I2C Non-Volatile FRAM Breakout - 256Kbit / 32KByte

Adafruit_EEPROM_I2C i2ceeprom;

struct SaveSlotHeader {
    byte magic[4] = { 0xF,0xE,0xE,0xD };
    byte num_params;
    byte num_inputs;
};

struct ParameterSetting {
    byte magic[8] = { 0xD,0xE,0xA,0xD };
    char label[20]; // for matching up
    double value = 0;
};

struct InputParameterSetting {
    byte magic[8] = { 0xF,0xA,0xD,0xE };
    //char label[20];
    char name;
    char parameter_label[20];
    //double internal_value;
    bool inverted = false;
    bool map_unipolar = false;
};

#define EEPROM_ADDR 0x50  // the default address!

void save_parameter_settings(LinkedList<BaseParameter*> *parameters, LinkedList<BaseParameterInput*> *inputs, int memory_offset = 0) {
    int size = parameters->size();

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

    memory_offset *= 512;    // start 16
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