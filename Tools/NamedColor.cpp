/**------------------------------------------------------------------------
 * All Rights Reserved
 * Author: Diego Macrini
 *-----------------------------------------------------------------------*/
#include <RootHeader.h>
#include "NamedColor.h"

NamedColor::ColorData NamedColor::s_palette[] = {
	{"Black", 0x00, 0x00, 0x00},
	{"Navy", 0x00, 0x00, 0x80},
	{"DarkBlue", 0x00, 0x00, 0x8B},
	{"MediumBlue", 0x00, 0x00, 0xCD},
	{"Blue", 0x00, 0x00, 0xFF},
	{"DarkGreen", 0x00, 0x64, 0x00},
	{"Green", 0x00, 0x80, 0x00},
	{"Teal", 0x00, 0x80, 0x80},
	{"DarkCyan", 0x00, 0x8B, 0x8B},
	{"DeepSkyBlue", 0x00, 0xBF, 0xFF},
	{"DarkTurquoise", 0x00, 0xCE, 0xD1},
	{"MediumSpringGreen", 0x00, 0xFA, 0x9A},
	{"Lime", 0x00, 0xFF, 0x00},
	{"SpringGreen", 0x00, 0xFF, 0x7F},
	{"Aqua", 0x00, 0xFF, 0xFF},
	{"Cyan", 0x00, 0xFF, 0xFF},
	{"MidnightBlue", 0x19, 0x19, 0x70},
	{"DodgerBlue", 0x1E, 0x90, 0xFF},
	{"LightSeaGreen", 0x20, 0xB2, 0xAA},
	{"ForestGreen", 0x22, 0x8B, 0x22},
	{"SeaGreen", 0x2E, 0x8B, 0x57},
	{"DarkSlateGray", 0x2F, 0x4F, 0x4F},
	{"LimeGreen", 0x32, 0xCD, 0x32},
	{"MediumSeaGreen", 0x3C, 0xB3, 0x71},
	{"Turquoise", 0x40, 0xE0, 0xD0},
	{"RoyalBlue", 0x41, 0x69, 0xE1},
	{"SteelBlue", 0x46, 0x82, 0xB4},
	{"DarkSlateBlue", 0x48, 0x3D, 0x8B},
	{"MediumTurquoise", 0x48, 0xD1, 0xCC},
	{"Indigo", 0x4B, 0x00, 0x82},
	{"DarkOliveGreen", 0x55, 0x6B, 0x2F},
	{"CadetBlue", 0x5F, 0x9E, 0xA0},
	{"CornflowerBlue", 0x64, 0x95, 0xED},
	{"MediumAquaMarine", 0x66, 0xCD, 0xAA},
	{"DimGray", 0x69, 0x69, 0x69},
	{"SlateBlue", 0x6A, 0x5A, 0xCD},
	{"OliveDrab", 0x6B, 0x8E, 0x23},
	{"SlateGray", 0x70, 0x80, 0x90},
	{"LightSlateGray", 0x77, 0x88, 0x99},
	{"MediumSlateBlue", 0x7B, 0x68, 0xEE},
	{"LawnGreen", 0x7C, 0xFC, 0x00},
	{"Chartreuse", 0x7F, 0xFF, 0x00},
	{"Aquamarine", 0x7F, 0xFF, 0xD4},
	{"Maroon", 0x80, 0x00, 0x00},
	{"Purple", 0x80, 0x00, 0x80},
	{"Olive", 0x80, 0x80, 0x00},
	{"Gray", 0x80, 0x80, 0x80},
	{"SkyBlue", 0x87, 0xCE, 0xEB},
	{"LightSkyBlue", 0x87, 0xCE, 0xFA},
	{"BlueViolet", 0x8A, 0x2B, 0xE2},
	{"DarkRed", 0x8B, 0x00, 0x00},
	{"DarkMagenta", 0x8B, 0x00, 0x8B},
	{"SaddleBrown", 0x8B, 0x45, 0x13},
	{"DarkSeaGreen", 0x8F, 0xBC, 0x8F},
	{"LightGreen", 0x90, 0xEE, 0x90},
	{"MediumPurple", 0x93, 0x70, 0xD8},
	{"DarkViolet", 0x94, 0x00, 0xD3},
	{"PaleGreen", 0x98, 0xFB, 0x98},
	{"DarkOrchid", 0x99, 0x32, 0xCC},
	{"YellowGreen", 0x9A, 0xCD, 0x32},
	{"Sienna", 0xA0, 0x52, 0x2D},
	{"Brown", 0xA5, 0x2A, 0x2A},
	{"DarkGray", 0xA9, 0xA9, 0xA9},
	{"LightBlue", 0xAD, 0xD8, 0xE6},
	{"GreenYellow", 0xAD, 0xFF, 0x2F},
	{"PaleTurquoise", 0xAF, 0xEE, 0xEE},
	{"LightSteelBlue", 0xB0, 0xC4, 0xDE},
	{"PowderBlue", 0xB0, 0xE0, 0xE6},
	{"FireBrick", 0xB2, 0x22, 0x22},
	{"DarkGoldenRod", 0xB8, 0x86, 0x0B},
	{"MediumOrchid", 0xBA, 0x55, 0xD3},
	{"RosyBrown", 0xBC, 0x8F, 0x8F},
	{"DarkKhaki", 0xBD, 0xB7, 0x6B},
	{"Silver", 0xC0, 0xC0, 0xC0},
	{"MediumVioletRed", 0xC7, 0x15, 0x85},
	{"IndianRed", 0xCD, 0x5C, 0x5C},
	{"Peru", 0xCD, 0x85, 0x3F},
	{"Chocolate", 0xD2, 0x69, 0x1E},
	{"Tan", 0xD2, 0xB4, 0x8C},
	{"LightGrey", 0xD3, 0xD3, 0xD3},
	{"PaleVioletRed", 0xD8, 0x70, 0x93},
	{"Thistle", 0xD8, 0xBF, 0xD8},
	{"Orchid", 0xDA, 0x70, 0xD6},
	{"GoldenRod", 0xDA, 0xA5, 0x20},
	{"Crimson", 0xDC, 0x14, 0x3C},
	{"Gainsboro", 0xDC, 0xDC, 0xDC},
	{"Plum", 0xDD, 0xA0, 0xDD},
	{"BurlyWood", 0xDE, 0xB8, 0x87},
	{"LightCyan", 0xE0, 0xFF, 0xFF},
	{"Lavender", 0xE6, 0xE6, 0xFA},
	{"DarkSalmon", 0xE9, 0x96, 0x7A},
	{"Violet", 0xEE, 0x82, 0xEE},
	{"PaleGoldenRod", 0xEE, 0xE8, 0xAA},
	{"LightCoral", 0xF0, 0x80, 0x80},
	{"Khaki", 0xF0, 0xE6, 0x8C},
	{"AliceBlue", 0xF0, 0xF8, 0xFF},
	{"HoneyDew", 0xF0, 0xFF, 0xF0},
	{"Azure", 0xF0, 0xFF, 0xFF},
	{"SandyBrown", 0xF4, 0xA4, 0x60},
	{"Wheat", 0xF5, 0xDE, 0xB3},
	{"Beige", 0xF5, 0xF5, 0xDC},
	{"WhiteSmoke", 0xF5, 0xF5, 0xF5},
	{"MintCream", 0xF5, 0xFF, 0xFA},
	{"GhostWhite", 0xF8, 0xF8, 0xFF},
	{"Salmon", 0xFA, 0x80, 0x72},
	{"AntiqueWhite", 0xFA, 0xEB, 0xD7},
	{"Linen", 0xFA, 0xF0, 0xE6},
	{"LightGoldenRodYellow", 0xFA, 0xFA, 0xD2},
	{"OldLace", 0xFD, 0xF5, 0xE6},
	{"Red", 0xFF, 0x00, 0x00},
	{"Fuchsia", 0xFF, 0x00, 0xFF},
	{"Magenta", 0xFF, 0x00, 0xFF},
	{"DeepPink", 0xFF, 0x14, 0x93},
	{"OrangeRed", 0xFF, 0x45, 0x00},
	{"Tomato", 0xFF, 0x63, 0x47},
	{"HotPink", 0xFF, 0x69, 0xB4},
	{"Coral", 0xFF, 0x7F, 0x50},
	{"Darkorange", 0xFF, 0x8C, 0x00},
	{"LightSalmon", 0xFF, 0xA0, 0x7A},
	{"Orange", 0xFF, 0xA5, 0x00},
	{"LightPink", 0xFF, 0xB6, 0xC1},
	{"Pink", 0xFF, 0xC0, 0xCB},
	{"Gold", 0xFF, 0xD7, 0x00},
	{"PeachPuff", 0xFF, 0xDA, 0xB9},
	{"NavajoWhite", 0xFF, 0xDE, 0xAD},
	{"Moccasin", 0xFF, 0xE4, 0xB5},
	{"Bisque", 0xFF, 0xE4, 0xC4},
	{"MistyRose", 0xFF, 0xE4, 0xE1},
	{"BlanchedAlmond", 0xFF, 0xEB, 0xCD},
	{"PapayaWhip", 0xFF, 0xEF, 0xD5},
	{"LavenderBlush", 0xFF, 0xF0, 0xF5},
	{"SeaShell", 0xFF, 0xF5, 0xEE},
	{"Cornsilk", 0xFF, 0xF8, 0xDC},
	{"LemonChiffon", 0xFF, 0xFA, 0xCD},
	{"FloralWhite", 0xFF, 0xFA, 0xF0},
	{"Snow", 0xFF, 0xFA, 0xFA},
	{"Yellow", 0xFF, 0xFF, 0x00},
	{"LightYellow", 0xFF, 0xFF, 0xE0},
	{"Ivory", 0xFF, 0xFF, 0xF0},
	{"White", 0xFF, 0xFF, 0xFF},
	{NULL, 0, 0, 0} // needs an empty color name to mark the end of the palette
};

/*NamedColor::NamedColor(const char* name)
{
	if (s_paletteMap.empty())
	{
		NamedColor::ColorData* p;

		for (p = s_pallete; *p != '\0'; p++)
		{
			RGBColor& c = s_paletteMap[p->name];

			c.r = p->r;
			c.g = p->g;
			c.b = p->
		}
	}
}*/
