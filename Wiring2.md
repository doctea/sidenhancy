# Wiring for Sidenhancy

## ATMega 1284 pinout

	 SID_RW to PB0 [ 1] -      - [40] PA0 to SID_D7
	 SID_CS to PB1 [ 2] -      - [39] PA1 to SID_D6
	           PB2 [ 3] x      - [38] PA2 to SID_D5
	SID_RES to PB3 [ 4] -      - [37] PA3 to SID_D4
	    SPI SS PB4 [ 5] -      - [36] PA4 to SID_D3
	  SPI Mosi PB5 [ 6] -      - [35] PA5 to SID_D2
	  SPI Miso PB6 [ 7] -      - [34] PA6 to SID_D1
	   SPI Sck PB7 [ 8] -      - [33] PA7 to SID_D0
	        Reset  [ 9] -      - [32] AREF
	 +5v VCC       [10] -      - [31] GND
	 GND           [11] -      - [30] AVCC
	 Xtal2         [12] -      - [29] PC7 to PIN_BUTTON_B
	 Xtal1         [13] -      - [28] PC6 to SID_A4
	 RxD0      PD0 [14] -      - [27] PC5 to SID_A3
	 TxD0      PD1 [15] -      - [26] PC4 to SID_A2
	           PD2 [16] x      - [25] PC3 to SID_A1
	           PD3 [17] x      - [24] PC2 to SID_A0
	 LED       PD4 [18] -      - [23] PC1/SDA to SDA i2c
	 SID_CLOCK PD5 [19] -      - [22] PC0/SCL to SCL i2c
	           PD6 [20] x      x [21] PD7


## SID pinout

           	   CAP [ 1] x      - [28] Vdd +12V (+5v!)
	           CAP [ 2] x      - [27] Audio out
	           CAP [ 3] x      x [26] Ext. In
	           CAP [ 4] x      - [25] +5v
	PB3 to SID_RES [ 5] -      - [24] Pot X
      PD5 to SID_CLOCK [ 6] -      - [23] Pot Y
         PB0 to SID_RW [ 7] -      - [22] SID_D7 to PA0
	 PB1 to SID_CS [ 8] -      - [21] SID_D6 to PA1
	     PC2 to A0 [ 9] -      - [20] SID_D5 to PA2
	     PC3 to A1 [10] -      - [19] SID_D4 to PA3
	     PC4 to A2 [11] -      - [18] SID_D3 to PA4
	     PC5 to A3 [12] -      - [17] SID_D2 to PA5
	     PC6 to A4 [13] -      - [16] SID_D1 to PA6
	           GND [14] -      - [15] SID_D0 to PA7

