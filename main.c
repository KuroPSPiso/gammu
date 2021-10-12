#include <gb/gb.h>
#include <stdint.h>
#include "alpha.c"
#include "fixed16_min.c"

#define TRUE 1
#define FALSE 0

#define FT 0 //floor tile
#define WT 1 //wall tile

//#define PI 3.141

//global variables
UINT8 modMultiplier = 0x00;
UINT8 updateScreen = TRUE;
UINT8 tick = 0x00;
UINT8 scanline_x = 0x00;
BOOL tileRenderSet = FALSE;

unsigned char vramTileData[] = {
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //tile (16bytes)
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

unsigned char vramTiles[] = {
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};
unsigned char vramTilesRenderColumn[] = {
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF
};

/*
//draw each loop
unsigned char vramTiles[] = {
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF
};*/

unsigned char timer[] = {
	0x00,
	0x00,
	0x00,
	0x23,
	0x13,
	0x1E,
	0x0D,
	0x16,
	0x22,
	0x1D,
	0x23
};
unsigned char timer2[] = {
	0x00,
	0x00,
	0x00,
	0x16,
	0x22,
	0x1D,
	0x23
};

unsigned char offsetTile = 0;
unsigned char offsetXTile = 0;
unsigned char offsetXTileSet = 0;
UINT16 count;
UWORD countFrameTime;

//constants
const unsigned char baseTile = 0x31;
const unsigned char screenSize = 8; //max size 8

//FPS content
float playerX = 8;
float playerY = 8;
float playerA = 00;
UINT16 mapHeight = 16;
UINT16 mapWidth = 16;
float FOV = PI / 4;
float distToWall = 0;
int ceilingOffset;
int floorOffset;
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
UINT16 mod16(UINT16 a, UINT16 divider);

//methods
void clearScreen()
{
	set_bkg_tiles(0, 0, 20, 18, blankScreen);
}

void computeVerticalLine()
{
	UINT16 scanline_y;
	unsigned char height;
	UINT8 x = mod(scanline_x, 8);
	offsetTile = 0x00;

	//offsety + ((time + posx) * timeScale) * amplitudeScale
	UINT16 yDrawLine = 10 + (F16TOINT16(mulF16(sinF16(intToF16((count + scanline_x) * 1000)), 0x50)));
	UINT16 yDrawLine2 = 40 + (F16TOINT16(mulF16(cosF16(intToF16((count + scanline_x) * 4000)), 0x100)));
	get_bkg_data(baseTile + offsetTile + offsetXTile, 1, vramTileData);

	for (scanline_y = 0; scanline_y < screenSize * 8 - 1; scanline_y++)
	{
		height = scanline_y * 2;
		height &= 0x0F;
		if (x == 0)
		{
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
		}

		if (scanline_y == yDrawLine || scanline_y == yDrawLine2) //draw diagonal line
		{
			vramTileData[height + 0x00] |= 0x01 << 7 - x;
			vramTileData[height + 0x01] |= 0x01 << 7 - x;
		}
		else
		{
			vramTileData[height + 0x00] &= ~(0x00 << 7 - x);
			vramTileData[height + 0x01] &= ~(0x00 << 7 - x);
		}
		
		if (height == 0xE)
		{
			set_bkg_data(baseTile + offsetTile + offsetXTile, 1, vramTileData);
			//vramTiles[offsetTile] = baseTile + offsetTile + offsetXTile; //draw each loop
			if(!tileRenderSet) vramTiles[offsetTile + 9 * (scanline_x/8)] = baseTile + offsetTile + offsetXTile;
			offsetTile += 0x01;
			get_bkg_data(baseTile + offsetTile + offsetXTile, 1, vramTileData);
		}
	}
}

void computeGraphics()
{
	//FPS logic start
	UINT16 rayA = (playerA - FOV / 2) + (scanline_x / screenSize * 8) * FOV;
	UINT16 distToWall = 0;
	UINT8 hitWall = FALSE;
	while (!hitWall)
	{
		distToWall += 1;

		hitWall = TRUE;
	}
	//FPS logic end

	UINT8 drawXCount;
	for (drawXCount = 0; drawXCount < 8; drawXCount++)
	{
		computeVerticalLine();
		scanline_x++;
	}
	//set_bkg_tiles(37 + offsetXTileSet, 35, 1, screenSize, vramTiles); //draw each loop
	offsetXTileSet++;

	offsetXTile += screenSize;

	drawXCount = 0;
	
	countFrameTime++;

	if (scanline_x >= 8 * screenSize - 1) {
		unsigned char renderLoop;
		if (!tileRenderSet)
		{
			for (renderLoop = 0; renderLoop < 8; renderLoop++)
			{
				vramTilesRenderColumn[0x00] = vramTiles[0x00 + 9 * renderLoop];
				vramTilesRenderColumn[0x01] = vramTiles[0x01 + 9 * renderLoop];
				vramTilesRenderColumn[0x02] = vramTiles[0x02 + 9 * renderLoop];
				vramTilesRenderColumn[0x03] = vramTiles[0x03 + 9 * renderLoop];
				vramTilesRenderColumn[0x04] = vramTiles[0x04 + 9 * renderLoop];
				vramTilesRenderColumn[0x05] = vramTiles[0x05 + 9 * renderLoop];
				vramTilesRenderColumn[0x06] = vramTiles[0x06 + 9 * renderLoop];
				vramTilesRenderColumn[0x07] = vramTiles[0x07 + 9 * renderLoop];
				//vramTilesRenderColumn[0x08] = vramTiles[0x08 + 9 * renderLoop];
				set_bkg_tiles(37 + renderLoop, 35, 1, screenSize, vramTilesRenderColumn);
			}
			tileRenderSet = TRUE;
		}
		//set_bkg_tiles(37 + 1, 35, 9, screenSize, vramTiles);
		scanline_x = 0x00;
		offsetXTile = 0x00;
		offsetXTileSet = 0x00;
		countFrameTime = 0x00;
	}
}

void renderTimer() {

	timer[0x00] = mod16(countFrameTime, 1000) / 100;
	timer[0x01] = mod16(countFrameTime, 100) / 10;
	timer[0x02] = mod16(countFrameTime, 10);
	set_bkg_tiles(0, 1, 11, 1, timer);		// frametime for drawing

	if (count > 999) count -= 999;
	timer2[0x00] = mod16(count, 1000) / 100;
	timer2[0x01] = mod16(count, 100) / 10;
	timer2[0x02] = mod16(count, 10);

	set_bkg_tiles(0, 2, 7, 1, timer2);		// frame
}

void timer_isr(void)
{
	count++;
}

void init()
{
	DISPLAY_ON;		// Turn on the display
	NR52_REG = 0x8F;	// Turn on the sound
	NR51_REG = 0x11;	// Enable the sound channels
	NR50_REG = 0x77;	// Increase the volume to its max

	disable_interrupts();
	add_TIM(timer_isr);
	enable_interrupts();

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

	//set_interrupts(TIM_IFLAG);
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

		//debug-data
		renderTimer();
		count++;

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

UINT16 mod16(UINT16 a, UINT16 divider)
{
	if (divider == 0) return 0;
	modMultiplier = a / divider;
	return a - divider * modMultiplier;
}