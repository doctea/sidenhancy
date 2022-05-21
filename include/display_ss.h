#ifdef ENABLE_SCREEN

#include <ss_oled.h>

#define GROVE_SDA_PIN -1
#define GROVE_SCL_PIN -1
// Set this to -1 to disable or the GPIO pin number connected to the reset
// line of your display if it requires an external reset
#define RESET_PIN -1
// let ss_oled figure out the display address
#define OLED_ADDR 0x3C
// don't rotate the display
#define FLIP180 0
// don't invert the display
#define INVERT 0
// Bit-Bang the I2C bus
#define USE_HW_I2C 1

// Change this if you're using different OLED displays
#define MY_OLED OLED_128x64
uint8_t ucBackBuffer[1024];

// The SSOLED structure. Each structure is about 56 bytes
// There is no limit to the number of simultaneous displays which can be controlled by ss_oled 
SSOLED ssoled;


void setup_display() {
    int rc;
    char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D",(char *)"SH1106 @ 0x3C",(char *)"SH1106 @ 0x3D"};

    rc = oledInit(&ssoled, MY_OLED, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, GROVE_SDA_PIN, GROVE_SCL_PIN, RESET_PIN, 800000L); // use standard I2C bus at 400Khz
    if (rc != OLED_NOT_FOUND) {
        oledFill(&ssoled, 0, 1);
        oledWriteString(&ssoled, 0,0,0,msgs[rc], FONT_NORMAL, 0, 1);
        delay(2000);
    } else {
        while (1) {};
    }
    oledSetBackBuffer(&ssoled, ucBackBuffer);
}

void tft_print(char *text, int x, int y) {
    //oledWriteString(&ssoled, x, y, 0, text, FONT_NORMAL, 0, 1);
    oledFill(&ssoled, 0, 1);
    oledWriteString(&ssoled, 0, x, y, text, FONT_NORMAL, random(1), 1);
}

#endif