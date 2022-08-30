# Wiring for Sidenhancy

## ATMega 1284 pinout

 SID_RW to PB0 [ 1] -      - [40] PA0 to SID_D7
 SID_CS to PB1 [ 2] -      - [39] PA1 to SID_D6
           PB2 [ 3] -      - [38] PA2 to SID_D5
SID_RES to PB3 [ 4] -      - [37] PA3 to SID_D4
    SPI SS PB4 [ 5] -      - [36] PA4 to SID_D3
  SPI Mosi PB5 [ 6] -      - [35] PA5 to SID_D2
  SPI Miso PB6 [ 7] -      - [34] PA6 to SID_D1
   SPI Sck PB7 [ 8] -      - [33] PA7 to SID_D0
        Reset  [ 9] -      - [32] AREF
 +5v VCC       [10] -      - [31] GND
 GND           [11] -      - [30] AVCC
 Xtal2         [12] -      - [29] PC7 to PIN_BUTTON_B
 Xtal1         [13] -      - [28] PC6
 RxD0      PD0 [14] -      - [27] PC5
 TxD0      PD1 [15] -      - [26] PC4
 SID_A0 to PD2 [16] -      - [25] PC3
 SID_A1 to PD3 [17] -      - [24] PC2 to SID_A4
 LED       PD4 [18] -      - [23] PC1/SDA to SDA i2c
 SID_CLOCK PD5 [19] -      - [22] PC0/SCL to SCL i2c
 SID_A2 to PD6 [20] -      - [21] PD7 to SID_A3


## SID pinout

           CAP [ 1] -      - [28] Vdd +12V (+5v!)
           CAP [ 2] -      - [27] Audio out
           CAP [ 3] -      - [26] Ext. In
           CAP [ 4] -      - [25] +5v
       SID_RES [ 5] -      - [24] Pot X
     SID_CLOCK [ 6] -      - [23] Pot Y
        SID_RW [ 7] -      - [22] SID_D7
        SID_CS [ 8] -      - [21] SID_D6
            A0 [ 9] -      - [20] SID_D5
            A1 [10] -      - [19] SID_D4
            A2 [11] -      - [18] SID_D3
            A3 [12] -      - [17] SID_D2
            A4 [13] -      - [16] SID_D1
           GND [14] -      - [15] SID_D0

