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

#ifndef __TFT_MENU_H__
#define __TFT_MENU_H__

///////////////////////////////////////////////////////////////////////////
//
// The R & B color bits are reversed on my ebay green tag display
//	so these colors work for mine... YMMV.
//
///////////////////////////////////////////////////////////////////////////

#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_RED         0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_BLUE        0xF800      /* 255,   0,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */

///////////////////////////////////////////////////////////////////////////
//
//	MENU structure:
//		
//	option:	Value to display
//	value:	Value to return on button press
//
//  NOTE: must be defined in a function since it is stored in flash 
//		using the __FlashStringHelper
//
///////////////////////////////////////////////////////////////////////////

typedef struct _MENU
{
    const __FlashStringHelper *option;
    int value;        

} MENU;

///////////////////////////////////////////////////////////////////////////
//
//	TFT_MENU Class to display a simple menu on a TFT display
//
///////////////////////////////////////////////////////////////////////////

class TFT_MENU
{
    public:
    	
        TFT_MENU(TFT_ST7735 tft, ClickEncoder *encoder, int menuFont = 1, uint8_t textSize = 1);
        
        int8_t show(MENU menu[], int8_t active = 1);
        
        ClickEncoder::Button getButton();
        
		void setColors(
				uint16_t headerForground, 
				uint16_t headerBackground, 
				uint16_t normalForeground,
				uint16_t normalBackground,
				uint16_t selectedForgound,
				uint16_t selectedBackground);
		
    private:

    	printSpaces(int8_t spaces = 1);

        int     font;
        int16_t fontHeight;
        int16_t fontWidth;
        uint8_t maxChars;
        uint8_t maxLines;

		uint16_t hF = TFT_BLUE;
		uint16_t hB = TFT_WHITE;
		uint16_t nF = TFT_WHITE;
		uint16_t nB = TFT_BLUE;
		uint16_t sF = TFT_WHITE;
		uint16_t sB = TFT_RED;
			
		ClickEncoder::Button button = ClickEncoder::Open;
};

///////////////////////////////////////////////////////////////////////////
//
//	TFT_MENU constuctor
//
//	tft: 		tft object
//	encoder: 	ClickEncoder object
//	menuFont:	Font to use for the menu, defaults to 1
//	textSize:	Size of text to use for the menu, defaults to 1
//
///////////////////////////////////////////////////////////////////////////

TFT_MENU::TFT_MENU(TFT_ST7735 tft, ClickEncoder *encoder, int menuFont, uint8_t textSize) {
    font = menuFont;

    fontHeight = tft.fontHeight(font) * textSize;
    fontWidth = tft.textWidth((char *) " ", font) * textSize;
    maxChars = tft.width() / (fontWidth);
    maxLines = tft.height() / (fontHeight);
}

///////////////////////////////////////////////////////////////////////////
//
// show: Show the menu[]
//
// Parameters:
//
// 		menu[]:	Array of MENU structures containing the menu
//	
//			The First entry's option will be shown as the menus Title and is not selectable.
//			The Last entry's option must be NULL.
//		
//		active: Active menu option at start, defaults to the first
//
// Returns:
//		the current option when the button was held, clicked or doubleclicked.
//	
//	Note:
//		use getButton to determine how the button was clicked.
//
///////////////////////////////////////////////////////////////////////////

int8_t TFT_MENU::show(MENU menu[], int8_t active = 1) {

    uint8_t menuCount = 0;
    uint8_t first = 1;
    uint8_t lfirst = 0;
    uint8_t count = 0;
    uint8_t line = 0;
    uint8_t	current = active;
    uint8_t lCurrent = 0;
    uint8_t i = 0;
    uint8_t j = 0;
    boolean again = true;

	// adjust first if the current option would be
	// off the screen
	
	if (current > maxLines-1)
		first = current - maxLines+2;

	// Clear screen
    
    tft.fillScreen(nB);

    // Print the menu header centered on the first line
    
    tft.setTextColor(hF, hB);
	uint8_t len = strlen_PF(menu[0].option);
    if (len < maxChars)
        i = ((maxChars/2)-(len/2)) * fontWidth;
    else
        i = 1;

    tft.fillRect(0,0,127,fontHeight, hB);
    tft.setCursor(i, 0);
    tft.println(menu[0].option);


	// determine the number of items in the menu
	
    i = 0;       
    while(menu[i++].option)
        menuCount++;

    // display the menu
    
    tft.setTextColor(nF, nB);
    
	do {
		if (current != lCurrent || first != lfirst) {	// if current or first has changed draw the menu
		    tft.setCursor(0, fontHeight);				// skip the first display line (the title)
			lCurrent = current;
			lfirst = first;
			for (i = 0; i < maxLines-1; i++) {			// for as many lines that wil fit on the display
				j = first+i;
				if (j < menuCount) {
					if (j == current) {	
						tft.setTextColor(sF, sB);
						tft.print(F(">")); 
						tft.setTextColor(nF, nB);
						tft.print(menu[j].option);
					}
					else {
						printSpaces(1);
						tft.print(menu[j].option);	
					}

					printSpaces(maxChars - strlen_PF(menu[j].option));
					tft.println();	
				}
			}
		}
		
		switch(encoder->getValue()) {
			case 0:
				button = encoder->getButton();
				switch(button) {
					case ClickEncoder::Clicked:
					case ClickEncoder::DoubleClicked:
						again = false;
					break;

					case ClickEncoder::Held:
						while (encoder->getButton() != ClickEncoder::Released);
						again = false;
					break;
				}
			break;
			
			case 1:
				if (current > 1) {
					current--;
					if (current < first)
						first--;
				}
			break;

			case -1:
				if (current < menuCount-1) {
					current++;
					if (current > maxLines -1)
						first = current - maxLines+2;
				}
			break;
		}
	}
	while (again);

	return current;
}

///////////////////////////////////////////////////////////////////////////
//
//	getButton: Returns how the button was clicked 
//
///////////////////////////////////////////////////////////////////////////

ClickEncoder::Button TFT_MENU::getButton() {
	return button;
}

///////////////////////////////////////////////////////////////////////////
//
//	setColors: Set the text colors used by the menu
//
///////////////////////////////////////////////////////////////////////////

void TFT_MENU::setColors( 
	uint16_t headerForground, 
	uint16_t headerBackground, 
	uint16_t normalForeground,
	uint16_t normalBackground,
	uint16_t selectedForgound,
	uint16_t selectedBackground)
{
	hF = headerForground; 
	hB = headerBackground; 
	nF = normalForeground;
	nB = normalBackground;
	sF = selectedForgound;
	sB = selectedBackground;
}

///////////////////////////////////////////////////////////////////////////
//
//	printSpaces: Prints spaces on TFT, used to clear to the end of the line
//
///////////////////////////////////////////////////////////////////////////

TFT_MENU::printSpaces(int8_t spaces) {
	if (spaces > 0)
		for (int i = 0; i < spaces; i++)
			tft.print(F(" "));
}

#endif // __TFT_MENU_H__
