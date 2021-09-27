#include <gb/gb.h>
#include <stdint.h>
#include "alpha.c"

#define TRUE 1
#define FALSE 0

#define FT 0 //floor tile
#define WT 1 //wall tile

//global variables
UINT8 modMultiplier = 0x00;
UINT8 test = 1;
UINT16 playerX = 00;
UINT16 playerY = 00;
UINT8 updateScreen = TRUE;
UINT8 tick = 0x00;
UINT8 scanline_x = 0x00;

unsigned char vramTileData[] = {
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //tile (16bytes)
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

unsigned char vramTiles[] = {
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF
};

unsigned char offsetTile = 0;
unsigned char offsetXTile = 0;
unsigned char offsetXTileSet = 0;

//constants
const unsigned char baseTile = 0x31;
const unsigned char screenSize = 7; //max size 8

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

//implicit methods
UINT8 pixColour(UINT8 px);
UINT8 mod(UINT8 a, UINT8 divider);

//methods
void clearScreen()
{
	set_bkg_tiles(0, 0, 20, 18, blankScreen);
}

void computeVerticalLine()
{
	//UINT8 scanline_y;
	UINT8 height;
	UINT8 x = mod(scanline_x, 8);
	offsetTile = 0x00;
	for (height = 0; height < screenSize; height++) {
				
		vramTileData[0x0 + 0x00] |= 0x01 << 7 - x;
		vramTileData[0x0 + 0x01] |= 0x01 << 7 - x;
		vramTileData[0x2 + 0x00] |= 0x01 << 7 - x;
		vramTileData[0x2 + 0x01] |= 0x01 << 7 - x;
		vramTileData[0x4 + 0x00] |= 0x01 << 7 - x;
		vramTileData[0x4 + 0x01] |= 0x01 << 7 - x;
		vramTileData[0x6 + 0x00] |= 0x01 << 7 - x;
		vramTileData[0x6 + 0x01] |= 0x01 << 7 - x;
		vramTileData[0x8 + 0x00] |= 0x01 << 7 - x;
		vramTileData[0x8 + 0x01] |= 0x01 << 7 - x;
		vramTileData[0xA + 0x00] |= 0x01 << 7 - x;
		vramTileData[0xA + 0x01] |= 0x01 << 7 - x;
		vramTileData[0xC + 0x00] |= 0x01 << 7 - x;
		vramTileData[0xC + 0x01] |= 0x01 << 7 - x;
		vramTileData[0xE + 0x00] |= 0x01 << 7 - x;
		vramTileData[0xE + 0x01] |= 0x01 << 7 - x;
		
		set_bkg_data(baseTile + offsetTile + offsetXTile, 1, vramTileData);
		vramTiles[offsetTile] = baseTile + offsetTile + offsetXTile;
		offsetTile+=0x01;
	}
}

void computeGraphics()
{
	UINT8 drawXCount;
	for (drawXCount = 0; drawXCount < 8; drawXCount++)
	{
		computeVerticalLine();
		scanline_x++;
	}
	set_bkg_tiles(40 - 8 / screenSize + offsetXTileSet, 37 - 8 / screenSize, 1, screenSize, vramTiles);
	offsetXTileSet++;

	offsetXTile += screenSize;

	drawXCount = 0;

	//TODO: can be disabled when screen is rendering (causes banding)
	vramTileData[0x0 + 0x00] = 0x00;
	vramTileData[0x0 + 0x01] = 0x00;
	vramTileData[0x2 + 0x00] = 0x00;
	vramTileData[0x2 + 0x01] = 0x00;
	vramTileData[0x4 + 0x00] = 0x00;
	vramTileData[0x4 + 0x01] = 0x00;
	vramTileData[0x6 + 0x00] = 0x00;
	vramTileData[0x6 + 0x01] = 0x00;
	vramTileData[0x8 + 0x00] = 0x00;
	vramTileData[0x8 + 0x01] = 0x00;
	vramTileData[0xA + 0x00] = 0x00;
	vramTileData[0xA + 0x01] = 0x00;
	vramTileData[0xC + 0x00] = 0x00;
	vramTileData[0xC + 0x01] = 0x00;
	vramTileData[0xE + 0x00] = 0x00;
	vramTileData[0xE + 0x01] = 0x00;

	if (scanline_x >= 8 * screenSize - 1) {
		scanline_x = 0x00;
		offsetXTile = 0x00;
		offsetXTileSet = 0x00;
	}
}

void init()
{
	DISPLAY_ON;		// Turn on the display
	NR52_REG = 0x8F;	// Turn on the sound
	NR51_REG = 0x11;	// Enable the sound channels
	NR50_REG = 0x77;	// Increase the volume to its max

	set_bkg_data(0, 47, alpha);		// Load 47 tiles into background memory
	
}


void checkInput() {
	if (joypad() & J_A) {
		// The B button was pressed!
		//set_bkg_tiles(0, 6, 10, 2, helloWorld);
		computeGraphics();
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

void main(void)
{
	init();
	clearScreen();
    // Loop forever
    while(1) {

		checkInput();
		updateSwitches();

		// Game main loop processing goes here
		computeGraphics();
		//renderFrame();

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}

UINT8 pixColour(UINT8 px)
{
	if (px == 0) return 0x80;
	if (px == 1) return 0x40;
	if (px == 2) return 0x20;
	if (px == 3) return 0x10;

	return 0x00;
}

UINT8 mod(UINT8 a, UINT8 divider)
{
	if (divider == 0) return 0;
	modMultiplier = a / divider;
	return a - divider * modMultiplier;
}