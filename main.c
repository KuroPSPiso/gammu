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

unsigned char sXPos[] = {
	0x00,
	0x00,
	0x00,
	0x0D, 0x28, 0x20, 0x1F, 0x23
};
unsigned char sYPos[] = {
	0x00,
	0x00,
	0x00,
	0x0D, 0x29, 0x20, 0x1F, 0x23
};
unsigned char sAngle[] = {
	0x00,
	0x00,
	0x00,
	0x0D, 0x11, 0x1E, 0x17, 0x1C, 0x15
};


/*
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
*/

unsigned char offsetTile = 0;
unsigned char offsetXTile = 0;
unsigned char offsetXTileSet = 0;
UINT16 count;
UWORD countFrameTime;

//constants
const unsigned char baseTile = 0x31;
const unsigned char screenSize = 2; //max size 8
const UINT8 mapHeight = 16;
const UINT8 mapWidth = 16;
const fixed16 depth = INT16TOF16(16);

const char map[] = {
	WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,FT,WT,
	WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT,WT
};

const char map2[] = {
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

//FPS content
fixed16 playerX = 0x80; //8.0
fixed16 playerY = 0x80; //8.0
fixed16 playerA = 0x00;
fixed16 FOV;
fixed16 distToWall = 0x00;
int ceilingOffset;
int floorOffset;

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
	//FPS logic start
	fixed16 rayA = (playerA - divF16(FOV, 0x20));
	rayA += mulF16(divF16(INT16TOF16(scanline_x), INT16TOF16(screenSize * 8)), FOV);
	distToWall = 0;
	UINT8 hitWall = FALSE;

	fixed16 eyeX = sinF16(rayA);
	fixed16 eyeY = sinF16(rayA);

	while (hitWall == FALSE && distToWall < depth)
	{
		distToWall += 0x1;

		INT16 testX = F16TOINT16(playerX + mulF16(eyeX, distToWall));
		INT16 testY = F16TOINT16(playerY + mulF16(eyeY, distToWall));

		if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
		{
			hitWall = TRUE;
			distToWall = depth;
		}
		else
		{
			if (map[testY * mapWidth + testX] == WT)
			{
				hitWall = TRUE;
			}
		}
	}

	INT16 ceiling = screenSize * 8 / 2 - (screenSize * 8) / F16TOINT16(distToWall);
	INT16 floor = screenSize * 8 - ceiling;
	//FPS logic end

	UINT16 scanline_y;
	unsigned char height;
	UINT8 x = mod(scanline_x, 8);
	offsetTile = 0x00;

	//offsety + ((time + posx) * timeScale) * amplitudeScale
	//UINT16 yDrawLine = 10 + (F16TOINT16(mulF16(sinF16(intToF16((count + scanline_x) * 1000)), 0x50)));
	//UINT16 yDrawLine2 = 40 + (F16TOINT16(mulF16(cosF16(intToF16((count + scanline_x) * 4000)), 0x100)));
	get_bkg_data(baseTile + offsetTile + offsetXTile, 1, vramTileData);

	for (scanline_y = 0; scanline_y < screenSize * 8; scanline_y++)
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

		if (scanline_y == 0 || scanline_y == (screenSize * 8) - 1 || scanline_x == 0 || scanline_x == (screenSize * 8) - 1)
		{
			vramTileData[height + 0x00] |= 0x01 << 7 - x;
			vramTileData[height + 0x01] |= 0x01 << 7 - x;
		}
		else if (scanline_y > ceiling && scanline_y < floor && hitWall == TRUE)//(hitWall == TRUE)//scanline_y == yDrawLine || scanline_y == yDrawLine2) //draw diagonal line
		{
			vramTileData[height + 0x00] |= 0x01 << 7 - x;
			vramTileData[height + 0x01] |= 0x01 << 7 - x;
		}
		else if (scanline_y >= floor)//(hitWall == TRUE)//scanline_y == yDrawLine || scanline_y == yDrawLine2) //draw diagonal line
		{
			vramTileData[height + 0x00] &= ~(0x00 << 7 - x);
			vramTileData[height + 0x01] &= ~(0x00 << 7 - x);
		}
		else if (scanline_y <= ceiling)//(hitWall == TRUE)//scanline_y == yDrawLine || scanline_y == yDrawLine2) //draw diagonal line
		{
			vramTileData[height + 0x00] |= 0x00 << 7 - x;
			vramTileData[height + 0x01] |= 0x01 << 7 - x;
		}
		else
		{
			vramTileData[height + 0x00] |= 0x01 << 7 - x;
			vramTileData[height + 0x01] |= 0x00 << 7 - x;
		}
		
		if (height == 0xE)
		{
			set_bkg_data(baseTile + offsetTile + offsetXTile, 1, vramTileData);
			//vramTiles[offsetTile] = baseTile + offsetTile + offsetXTile; //draw each loop
			if(tileRenderSet == FALSE) vramTiles[offsetTile + 9 * (scanline_x/8)] = baseTile + offsetTile + offsetXTile;
			offsetTile += 0x01;
			get_bkg_data(baseTile + offsetTile + offsetXTile, 1, vramTileData);
		}
	}
}

void computeGraphics()
{
	UINT8 drawXCount;
	for (drawXCount = 0; drawXCount < 8; drawXCount++)
	{
		computeVerticalLine();
		scanline_x++;

		if (!tileRenderSet)
		{
			vramTilesRenderColumn[0x00] = vramTiles[0x00 + 9 * drawXCount];
			vramTilesRenderColumn[0x01] = vramTiles[0x01 + 9 * drawXCount];
			vramTilesRenderColumn[0x02] = vramTiles[0x02 + 9 * drawXCount];
			vramTilesRenderColumn[0x03] = vramTiles[0x03 + 9 * drawXCount];
			vramTilesRenderColumn[0x04] = vramTiles[0x04 + 9 * drawXCount];
			vramTilesRenderColumn[0x05] = vramTiles[0x05 + 9 * drawXCount];
			vramTilesRenderColumn[0x06] = vramTiles[0x06 + 9 * drawXCount];
			vramTilesRenderColumn[0x07] = vramTiles[0x07 + 9 * drawXCount];
			set_bkg_tiles(37 + drawXCount, 35, 1, screenSize, vramTilesRenderColumn);
		}

		if (joypad() & J_UP) {
			playerX += mulF16(sinF16(playerA), 0x01);
			playerY -= mulF16(cosF16(playerA), 0x01);

			//playerY += 0x01; //TODO:Angle to X,Y
		}
		if (joypad() & J_DOWN) {
			playerX -= mulF16(sinF16(playerA), 0x01);
			playerY += mulF16(cosF16(playerA), 0x01);
			//playerY -= 0x01;
		}
		if (joypad() & J_LEFT) {
			playerA -= 0x01;

			if (playerA < -PI) {
				playerA = PI - 0x01;
			}
		}
		if (joypad() & J_RIGHT) {
			playerA += 0x01;
			if (playerA >= PI) {
				playerA = -PI;
			}
		}
	}
	offsetXTileSet++;

	offsetXTile += screenSize;

	drawXCount = 0;
	
	countFrameTime++;

	if (scanline_x >= 8 * screenSize - 1) {
		/*unsigned char renderLoop;
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
		}*/
		tileRenderSet = TRUE;
		scanline_x = 0x00;
		offsetXTile = 0x00;
		offsetXTileSet = 0x00;
		countFrameTime = 0x00;
	}
}

void renderTimer() {

	sXPos[0x00] = mod16(F16TOINT16(mulF16(playerX, 100)), 1000) / 100;
	sXPos[0x01] = mod16(F16TOINT16(mulF16(playerX, 100)), 100) / 10;
	sXPos[0x02] = mod16(F16TOINT16(mulF16(playerX, 100)), 10);
	set_bkg_tiles(0, 1, 8, 1, sXPos);

	sYPos[0x00] = mod16(F16TOINT16(mulF16(playerY, 100)), 1000) / 100;
	sYPos[0x01] = mod16(F16TOINT16(mulF16(playerY, 100)), 100) / 10;
	sYPos[0x02] = mod16(F16TOINT16(mulF16(playerY, 100)), 10);
	set_bkg_tiles(0, 2, 8, 1, sYPos);

	sAngle[0x00] = mod16(playerA, 1000) / 100;
	sAngle[0x01] = mod16(playerA, 100) / 10;
	sAngle[0x02] = mod16(playerA, 10);
	set_bkg_tiles(0, 3, 9, 1, sAngle);

	/*timer[0x00] = mod16(countFrameTime, 1000) / 100;
	timer[0x01] = mod16(countFrameTime, 100) / 10;
	timer[0x02] = mod16(countFrameTime, 10);
	set_bkg_tiles(0, 1, 11, 1, timer);		// frametime for drawing

	if (count > 999) count -= 999;
	timer2[0x00] = mod16(count, 1000) / 100;
	timer2[0x01] = mod16(count, 100) / 10;
	timer2[0x02] = mod16(count, 10);

	set_bkg_tiles(0, 2, 7, 1, timer2);		// frame
	*/
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

	FOV = divF16(PI, 0x40);
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