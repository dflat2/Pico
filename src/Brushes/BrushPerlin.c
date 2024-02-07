#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "ClassiCube/src/BlockID.h"
#include "ClassiCube/src/Constants.h"
#include "ClassiCube/src/String.h"

#include "Parse.h"
#include "Messaging.h"

static bool BrushPerlin_TryParseArguments(const cc_string* args, int argsCount);
static BlockID BrushPerlin_Paint(int x, int y, int z);

Brush BrushPerlin = {
	.TryParseArguments = &BrushPerlin_TryParseArguments,
	.Paint = &BrushPerlin_Paint,
}; 

static const int permutation[512] = {
	158,83,170,101,150,200,118,236,63,135,149,235,109,189,153,73,207,171,157,97,
	188,45,245,138,110,255,8,55,222,37,196,126,111,198,168,145,187,5,22,191,125,
	12,186,179,90,129,223,44,64,182,71,162,159,76,59,215,232,18,224,15,152,74,7,
	89,184,249,33,108,156,229,246,214,92,16,96,75,107,176,4,116,61,124,52,66,26,
	234,154,227,40,13,3,106,24,30,228,60,56,139,122,19,190,136,54,204,80,51,2,220,
	104,134,86,10,144,181,238,41,14,27,50,231,20,180,46,173,251,123,212,178,62,
	210,166,130,155,137,43,199,146,161,112,206,98,160,93,201,244,113,0,94,95,69,
	49,48,85,253,141,23,225,143,78,100,131,205,254,6,68,84,121,239,219,217,247,194,
	91,218,233,11,119,102,35,57,169,65,1,120,203,42,105,132,221,17,38,133,53,164,
	250,128,34,28,183,114,163,151,202,31,209,127,185,226,237,32,167,142,213,147,29,
	177,241,99,82,252,175,79,197,208,115,148,248,72,77,25,165,81,240,174,243,39,
	230,193,58,140,88,216,70,87,36,242,21,211,9,103,195,67,192,117,47,172,

	// Repeating for easier calculations.
	158,83,170,101,150,200,118,236,63,135,149,235,109,189,153,73,207,171,157,97,
	188,45,245,138,110,255,8,55,222,37,196,126,111,198,168,145,187,5,22,191,125,
	12,186,179,90,129,223,44,64,182,71,162,159,76,59,215,232,18,224,15,152,74,7,
	89,184,249,33,108,156,229,246,214,92,16,96,75,107,176,4,116,61,124,52,66,26,
	234,154,227,40,13,3,106,24,30,228,60,56,139,122,19,190,136,54,204,80,51,2,220,
	104,134,86,10,144,181,238,41,14,27,50,231,20,180,46,173,251,123,212,178,62,
	210,166,130,155,137,43,199,146,161,112,206,98,160,93,201,244,113,0,94,95,69,
	49,48,85,253,141,23,225,143,78,100,131,205,254,6,68,84,121,239,219,217,247,194,
	91,218,233,11,119,102,35,57,169,65,1,120,203,42,105,132,221,17,38,133,53,164,
	250,128,34,28,183,114,163,151,202,31,209,127,185,226,237,32,167,142,213,147,29,
	177,241,99,82,252,175,79,197,208,115,148,248,72,77,25,165,81,240,174,243,39,
	230,193,58,140,88,216,70,87,36,242,21,211,9,103,195,67,192,117,47,172
};

static double PerlinSmoothFunction(double t) {
	return t * t * t * (t * (6 * t - 15) + 10);
	return t;
}

// Source: http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html
static double Gradient(int hash, double x, double y, double z) {
	switch(hash & 0xF)
    {
        case 0x0: return  x + y;
        case 0x1: return -x + y;
        case 0x2: return  x - y;
        case 0x3: return -x - y;
        case 0x4: return  x + z;
        case 0x5: return -x + z;
        case 0x6: return  x - z;
        case 0x7: return -x - z;
        case 0x8: return  y + z;
        case 0x9: return -y + z;
        case 0xA: return  y - z;
        case 0xB: return -y - z;
        case 0xC: return  y + x;
        case 0xD: return -y + z;
        case 0xE: return  y - x;
        case 0xF: return -y - z;
        default: return 0;
    }
}

static double LinearInterpolation(double t, double min, double max) {
	return min + t * (max - min);
}

static double Perlin(double x, double y, double z) {
	// Each input must be between 0.0 (inclusive) and 256.0 (exclusive).
	x = fmod(x, 256.0);
	y = fmod(y, 256.0);
	z = fmod(z, 256.0);

	int xIntegralPart = (int)floor(x);
	int yIntegralPart = (int)floor(y);
	int zIntegralPart = (int)floor(z);

	double xFractionalPart = x - (double)xIntegralPart;
	double yFractionalPart = y - (double)yIntegralPart;
	double zFractionalPart = z - (double)zIntegralPart;	

	double xFadedFractionalPart = PerlinSmoothFunction(xFractionalPart);
	double yFadedFractionalPart = PerlinSmoothFunction(yFractionalPart);
	double zFadedFractionalPart = PerlinSmoothFunction(zFractionalPart);

    int aaaHash = permutation[permutation[permutation[xIntegralPart] + yIntegralPart] + zIntegralPart];
    int abaHash = permutation[permutation[permutation[xIntegralPart] + yIntegralPart + 1] + zIntegralPart];
    int aabHash = permutation[permutation[permutation[xIntegralPart] + yIntegralPart] + zIntegralPart + 1];
    int abbHash = permutation[permutation[permutation[xIntegralPart] + yIntegralPart + 1] + zIntegralPart + 1];
    int baaHash = permutation[permutation[permutation[xIntegralPart + 1] + yIntegralPart]+ zIntegralPart];
    int bbaHash = permutation[permutation[permutation[xIntegralPart + 1] + yIntegralPart + 1] + zIntegralPart];
    int babHash = permutation[permutation[permutation[xIntegralPart + 1] + yIntegralPart] + zIntegralPart + 1];
    int bbbHash = permutation[permutation[permutation[xIntegralPart + 1] + yIntegralPart + 1] + zIntegralPart + 1];

    double aaaInfluence = Gradient(aaaHash, xFractionalPart, yFractionalPart, zFractionalPart);
	double abaInfluence = Gradient(abaHash, xFractionalPart, yFractionalPart - 1, zFractionalPart);
	double aabInfluence = Gradient(aabHash, xFractionalPart, yFractionalPart, zFractionalPart - 1);
	double abbInfluence = Gradient(abbHash, xFractionalPart, yFractionalPart - 1, zFractionalPart - 1);
	double baaInfluence = Gradient(baaHash, xFractionalPart - 1, yFractionalPart, zFractionalPart);
	double bbaInfluence = Gradient(bbaHash, xFractionalPart - 1, yFractionalPart - 1, zFractionalPart);
	double babInfluence = Gradient(babHash, xFractionalPart - 1, yFractionalPart, zFractionalPart - 1);
	double bbbInfluence = Gradient(bbbHash, xFractionalPart - 1, yFractionalPart - 1, zFractionalPart - 1);

	double xaaInterpolation = LinearInterpolation(xFadedFractionalPart, aaaInfluence, baaInfluence);
	double xbaInterpolation = LinearInterpolation(xFadedFractionalPart, abaInfluence, bbaInfluence);
	double xabInterpolation = LinearInterpolation(xFadedFractionalPart, aabInfluence, babInfluence);
	double xbbInterpolation = LinearInterpolation(xFadedFractionalPart, abbInfluence, bbbInfluence);
	
	double xyaInterpolation = LinearInterpolation(yFadedFractionalPart, xaaInterpolation, xbaInterpolation);
	double xybInterpolation = LinearInterpolation(yFadedFractionalPart, xabInterpolation, xbbInterpolation);

	return LinearInterpolation(zFadedFractionalPart, xyaInterpolation, xybInterpolation);;
}

static bool BrushPerlin_TryParseArguments(const cc_string* args, int argsCount) {
	return true;
}

static BlockID BrushPerlin_Paint(int x, int y, int z) {
	double scale = 10.0;
	double result = Perlin((double)x / scale, (double)y / scale, (double)z / scale);

	if (result >= 0.0) {
		return BLOCK_WHITE;
	} else {
		return BLOCK_BLACK;
	}
}
