#include <gb/gb.h>
#include <stdint.h>
#include "alpha.c"

#define TRUE 1
#define FALSE 0

#define FT 0 //floor tile
#define WT 1 //wall tile

UINT8 test = 1;
UINT16 playerX = 00;
UINT16 playerY = 00;
UINT8 updateScreen = TRUE;
UINT8 tick = 0x00;

unsigned char vramData = {
	0x00
};

//constants
const char map[] = {
	WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,WT,FT,FT,FT,FT,FT,FT,FT,FT,WT,FT,FT,WT,
	WT,FT,WT,WT,FT,FT,FT,FT,FT,FT,FT,FT,WT,WT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,WT,FT,WT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,WT,FT,WT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,WT,WT,FT,WT,WT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,WT,WT,WT,FT,FT,FT,WT,WT,WT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,WT,WT,WT,FT,FT,FT,WT,WT,WT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,WT,WT,FT,WT,WT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,WT,FT,WT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,WT,FT,WT,FT,FT,FT,FT,FT,WT,
	WT,FT,WT,WT,FT,FT,FT,FT,FT,FT,FT,FT,WT,WT,FT,WT,
	WT,FT,FT,WT,FT,FT,FT,FT,FT,FT,FT,FT,WT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT
};

//methods
void clearScreen()
{
	set_bkg_tiles(0, 0, 20, 18, blankScreen);
}

void draw()
{
	if (LYC_REG == 0x00)
	{
		
	}
}

void computeGraphics()
{

}

void renderGraphics()
{

}

void init()
{
	DISPLAY_ON;		// Turn on the display
	NR52_REG = 0x8F;	// Turn on the sound
	NR51_REG = 0x11;	// Enable the sound channels
	NR50_REG = 0x77;	// Increase the volume to its max
	
	STAT_REG = 0x45;
	LYC_REG = 0x00;

	set_bkg_data(0, 47, alpha);		// Load 47 tiles into background memory

	disable_interrupts();
	add_LCD(draw);
	enable_interrupts();

	set_interrupts(VBL_IFLAG | LCD_IFLAG);
}


void checkInput() {
	if (joypad() & J_A) {
		// The B button was pressed!
		set_bkg_tiles(0, 6, 10, 2, helloWorld);
	}
	if (joypad() & J_B) {
		// The B button was pressed!
		clearScreen();
	}
}

void updateSwitches() {


	HIDE_WIN;
	SHOW_SPRITES;
	SHOW_BKG;
}

void renderFrame()
{

	//todo renderArray

	unsigned char data1[] = {
		tick, tick+2, tick+4, tick+6
	};
	unsigned char data2[] = {
		tick+1, tick + 3, tick + 5, tick + 7
	};
	unsigned char data3[] = {
		tick+2, tick + 4, tick + 6, tick + 8
	};
	unsigned char data4[] = {
		tick+3, tick + 5, tick + 7, tick + 9
	};

	set_bkg_data(50, 1, data1);
	set_bkg_data(51, 1, data2);
	set_bkg_data(52, 1, data3);
	set_bkg_data(53, 1, data4);
	tick ++;
	//set_data(vramaddy, data, len);

	//get_data(data, vramaddy,  len);
}

void main(void)
{
	init();
	clearScreen();
    // Loop forever
    while(1) {

		checkInput();
		updateSwitches();

		// Game main loop processing goes here
		renderFrame();

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
