#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "ClassiCube/src/BlockID.h"
#include "ClassiCube/src/Constants.h"
#include "ClassiCube/src/String.h"

#include "Parse.h"
#include "Message.h"

static BlockID s_Block1;
static BlockID s_Block2;
static int s_Scale;
static float s_Threshold;
static int s_Permutation[512] = { 0 };

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

    int aaaHash = s_Permutation[s_Permutation[s_Permutation[xIntegralPart] + yIntegralPart] + zIntegralPart];
    int abaHash = s_Permutation[s_Permutation[s_Permutation[xIntegralPart] + yIntegralPart + 1] + zIntegralPart];
    int aabHash = s_Permutation[s_Permutation[s_Permutation[xIntegralPart] + yIntegralPart] + zIntegralPart + 1];
    int abbHash = s_Permutation[s_Permutation[s_Permutation[xIntegralPart] + yIntegralPart + 1] + zIntegralPart + 1];
    int baaHash = s_Permutation[s_Permutation[s_Permutation[xIntegralPart + 1] + yIntegralPart]+ zIntegralPart];
    int bbaHash = s_Permutation[s_Permutation[s_Permutation[xIntegralPart + 1] + yIntegralPart + 1] + zIntegralPart];
    int babHash = s_Permutation[s_Permutation[s_Permutation[xIntegralPart + 1] + yIntegralPart] + zIntegralPart + 1];
    int bbbHash = s_Permutation[s_Permutation[s_Permutation[xIntegralPart + 1] + yIntegralPart + 1] + zIntegralPart + 1];

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

static void GenerateRandomPermutation(void) {
    srand(time(NULL));

    int numbersLeft[256];
    int numbersLeftCount = 256;

    for (int i = 0; i < 256; i++) {
        numbersLeft[i] = i;
    }

    int permutationCount = 0;

    int drawnNumberIndex;
    int drawnNumber;

    while (numbersLeftCount > 0) {
        // Draws a random number in `numbersLeft`
        drawnNumberIndex = rand() % numbersLeftCount;
        drawnNumber = numbersLeft[drawnNumberIndex];

        // Adds `drawnNumber` to the `s_Permutation` array.
        s_Permutation[permutationCount] = drawnNumber;
        permutationCount++;

        // Removes `drawnNumber` from `numbersLeft`.
        numbersLeftCount--;

        for (int i = drawnNumberIndex; i < numbersLeftCount; i++) {
            numbersLeft[i] = numbersLeft[i + 1];
        }
    }

    // Repeating the permutation for easier calculations
    for (int i = 0; i < 256; i++) {
        s_Permutation[i + 256] = s_Permutation[i];
    }
}

static bool BrushPerlin_TryParseArguments(const cc_string* args, int argsCount) {
    if (argsCount <= 2 || argsCount > 4) {
        Message_Player("&b@Perlin&f usage: &b@Perlin <block1> <block2> <scale> [threshold]&f.");
        return false;
    }

    if (!Parse_TryParseBlock(&args[0], &s_Block1)) {
        return false;
    }

    if (!Parse_TryParseBlock(&args[1], &s_Block2)) {
        return false;
    }

    if (!Parse_TryParseNumber(&args[2], &s_Scale)) {
        return false;
    }

    if (s_Scale < 0) {
        Message_Player("&bscale &fmust be a positive integer.");
        return false;
    }

    if (argsCount >= 4) {
        if (!Parse_TryParseFloat(&args[3], &s_Threshold)) {
            return false;
        } else if (s_Threshold <= -1.0 || s_Threshold >= 1.0) {
            Message_Player("&bthreshold &fmust be between &b-1.0 &fand &b1.0&f.");
            return false;
        }
    } else {
        s_Threshold = 0.0;
    }

    GenerateRandomPermutation();
    return true;
}

static BlockID BrushPerlin_Paint(int x, int y, int z) {
    double result = Perlin((double)x / s_Scale, (double)y / s_Scale, (double)z / s_Scale);

    if (result >= (double)s_Threshold) {
        return s_Block2;
    } else {
        return s_Block1;
    }
}

static void BrushPerlin_Help(void) {
    Message_Player("&b@Perlin <block1> <block2> <scale> [threshold]");
    Message_Player("Perlin noise based brush.");
}

Brush BrushPerlin = {
    .TryParseArguments = &BrushPerlin_TryParseArguments,
    .Paint = &BrushPerlin_Paint,
    .HelpFunction = &BrushPerlin_Help,
};
