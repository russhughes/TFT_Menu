///////////////////////////////////////////////////////////////////////////
//
//	TFT_Menu - A simple Arduino menu class for small ST7735 TFT displays
//		using a clickable rotary encoder.
//
//	3rd Party arduino libraries used:
//
//		Bodmer's Arduino graphics library for ST7735:
//			https://github.com/Bodmer/TFT_ST7735
//			
//		0xPIT's Atmel AVR C++ RotaryEncoder Implementation
//			https://github.com/0xPIT/encoder
//			
//		Paul Stoffregen's TimerOne Library with optimization and expanded hardware support 
//			https://github.com/PaulStoffregen/TimerOne
//
//	Code by Russ Hughes (russ@owt.com) September 2018
//
//	License:
//	This is free software. You can redistribute it and/or modify it under
//  the terms of Creative Commons Attribution 3.0 United States License. 
//  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
//  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
//
///////////////////////////////////////////////////////////////////////////

//
//  TFT_ST7735 library
//

#include <TFT_ST7735.h> // https://github.com/Bodmer/TFT_ST7735
#include <SPI.h>

// Invoke library, pins defined in User_Setup.h of the 
// TFT_ST7735 library for this example I'm only using font1

TFT_ST7735 tft = TFT_ST7735(128,128);  

//
// Encoder pin use
//

#define ENC_C   3
#define ENC_D   4
#define ENC_S   5

//
// ClickEncoder library config
//

#include <ClickEncoder.h>      	// https://github.com/0xPIT/encoder
#include <TimerOne.h>       	// https://github.com/PaulStoffregen/TimerOne

ClickEncoder *encoder;
ClickEncoder::Button button;

//
// Timer interrupt service for encoder
//

void timerIsr() {
  encoder->service();
}

//
// TFT_Menu library
//


#include "TFT_Menu.h"
uint8_t textSize = 2;
TFT_MENU menu(tft, encoder, 1, textSize);

int8_t option = 1;

//
//	Test menu function
//

int showMyMenu(int active) {
    MENU myMenu[] =
    {
        { F("My Menu"),    	  0 },
        { F(" 1 One"), 		  1 },
        { F(" 2 Two"),        2 },
        { F(" 3 Three"),      3 }, 
        { F(" 4 Four"),       4 },
        { F(" 5 Five"),       5 },      
        { F(" 6 Six"),        6 },
        { F(" 7 Seven"),      7 },
        { F(" 8 Eight"),      8 },
        { F(" 9 Nine"),       9 },
        { F("10 Ten"),       10 },
        { F("11 Eleven"),    11 },
        { F("12 Twelve"),    12 },
        { F("13 Thirteen"),  13 },
        { F("14 Fourteen"),  14 },
        { F("15 Fifteen"),   15 },
        { F("16 Sixteen"),   16 },
        { F("17 Seventeen"), 17 },
        { F("18 Eighteen"),  18 },
        { F("19 Nineteen"),  19 },
        { F("20 Twenty"),    20 },
        { NULL,               0 }
    };

    return menu.show(myMenu, active);    
}

//
//	The setup
//

void setup(void)
{
	// configure the TFT display
	
	tft.init();
    tft.setRotation(2);			// mine is mounted upside down
    tft.setTextWrap(false);		// don't wrap text 
    tft.setTextSize(textSize);	// works for reasonable values

	// setup the encoder
	
	encoder = new ClickEncoder(ENC_C, ENC_D, ENC_S, 4);		// 4 pulses per option
	Timer1.initialize(1000);
  	Timer1.attachInterrupt(timerIsr); 
}

// The rat race.

void loop() {

	// Show MyMenu 
	
	option = showMyMenu(option);
	button = menu.getButton();

	// Display the results
	
	tft.fillScreen(TFT_BLUE);
	tft.setCursor(0, 0);
	tft.println("You Chose");
	tft.println(option);
	tft.println();
	
	tft.println("Using");
	switch (button) {
		case ClickEncoder::Clicked:
			tft.println("Click");
		break;
		
		case ClickEncoder::DoubleClicked:
			tft.println("Dbl Click");
		break;
		
		case ClickEncoder::Held:
			tft.println("Holding");	
		break;
	}

	tft.println();
	tft.println("Click To");
	tft.println("Try Again");

	// wait for a click to try again!
	
	while(encoder->getButton() != ClickEncoder::Clicked)  
		delay(100);
}

