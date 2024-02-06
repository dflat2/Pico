#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "ClassiCube/src/BlockID.h"
#include "ClassiCube/src/Constants.h"
#include "ClassiCube/src/String.h"

#include "Parse.h"
#include "Messaging.h"

#define MAX_BLOCKS 10

static bool BrushPerlin_TryParseArguments(const cc_string* args, int argsCount);
static BlockID BrushPerlin_Paint(int x, int y, int z);
static bool TryParseBlockWeight(const cc_string* argument, BlockID* out_block, int* out_weight);
static void UpdateCumulativeWeights(void);
static void UpdateTotal(void);

Brush BrushPerlin = {
	.TryParseArguments = &BrushPerlin_TryParseArguments,
	.Paint = &BrushPerlin_Paint,
}; 

static BlockID s_Blocks[MAX_BLOCKS];
static int s_Weights[MAX_BLOCKS];
static int s_CumulativeWeights[MAX_BLOCKS];
static int s_Total = 0;
static int s_Count = 0;
static const BlockID BLOCK_ERROR = BLOCK_GOLD;

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
}

static float Perlin(float x, float y, float z) {
	x = fmod(x, 256.0);
	y = fmod(y, 256.0);
	z = fmod(z, 256.0);

	float xFloor = floor(x);
	float yFloor = floor(y);
	float zFloor = floor(z);

	float xFractionalPart = x - xFloor;
	float yFractionalPart = y - yFloor;
	float zFractionalPart = z - zFloor;	
}

static bool BrushPerlin_TryParseArguments(const cc_string* args, int argsCount) {
	if (argsCount > MAX_BLOCKS) {
		Message_Player("Cannot use &b@Perlin &fwith more than ten blocks.");
		return false;
	} else if (argsCount == 0) {
		Message_Player("&b@Perlin&f: no blocks provided.");
		Message_Player("Example: &b@Perlin Stone/9 Air/1&f.");
		return false;
	}

	for (int i = 0; i < argsCount; i++) {
		if (!TryParseBlockWeight(&args[i], &s_Blocks[i], &s_Weights[i])) {
			return false;
		}
	}

	s_Count = argsCount;
	UpdateCumulativeWeights();
	UpdateTotal();

	srand(time(NULL));
	return true;
}

static BlockID BrushPerlin_Paint(int x, int y, int z) {
	int random = rand() % s_Total;

	for (int i = 0; i < s_Count; i++) {
		if (random < s_CumulativeWeights[i]) {
			return s_Blocks[i];
		}
	}

	return BLOCK_ERROR;
}

static bool TryParseBlockWeight(const cc_string* argument, BlockID* out_block, int* out_weight) {
	cc_string parts[2];
	int partsCount;

	partsCount = String_UNSAFE_Split(argument, '/', parts, 2);

	if (!Parse_TryParseBlock(&parts[0], out_block)) {
		return false;
	}

	if (partsCount == 1) {
		*out_weight = 1;
		return true;
	}

	char buffer[STRING_SIZE];
	cc_string errorMessage = String_FromArray(buffer);

	if (!Convert_ParseInt(&parts[1], out_weight)) {
		String_Format1(&errorMessage, "Could not parse integer &b%s&f.", &parts[0]);
		return false;
	} else if (*out_weight <= 0) {
		String_Format1(&errorMessage, "Invalid &b@Perlin &fweight: &b%s&f.", &parts[0]);
		return false;
	}

	return true;
}

static void UpdateCumulativeWeights(void) {
	s_CumulativeWeights[0] = s_Weights[0];

	for (int i = 1; i < s_Count; i++) {
		s_CumulativeWeights[i] = s_CumulativeWeights[i - 1] + s_Weights[i];
	}
}

static void UpdateTotal(void) {
	s_Total = 0;

	for (int i = 0; i < s_Count; i++) {
		s_Total += s_Weights[i];
	}
}
