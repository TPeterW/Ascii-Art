/***
 *
 * Ascii Art project
 *
 * Tao Peter Wang
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <iostream>
#include <fstream>

#include "ascii.h"
#include "imageLib.h"

#define INPUT(x, y) input.Pixel(x, y, 0)
#define DSMAP(x, y) densMap.Pixel(x, y, 0)

CByteImage convertToGrey(CByteImage &, int, int);
void calcIntegrals(CByteImage &, CIntImage &, int, int);
void writeToFile(CIntImage &, int, int, int, ofstream &);
bool inside(int, int, int, int);
char toChar(int);

using namespace std;

int main(int argc, char ** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: ascii in.png output.txt\n");   
        exit(1);
    }
    char *inname = argv[1];
    char *outname = argv[2];
    
    CByteImage input;
    ofstream output;
    output.open(outname);
    
    try {
        // read in
        ReadImageVerb(input, inname, VERBOSE);
        CShape sh = input.Shape();
        int width = sh.width, height = sh.height, nB = sh.nBands;
        cout << "Image Info: \n";
        cout << "Width: " << width << "\t";
        cout << "Height: " << height << "\t";
        cout << "Bands: " << nB << "\n";
        
        if (nB == 4) {
            input = convertToGrey(input, width, height);
            nB = 1;
        }
        
        // WriteImageVerb(input, "tmp.png", VERBOSE);
        
        CIntImage densMap;
        calcIntegrals(input, densMap, width, height);
        writeToFile(densMap, width, height, DEFAULT_RAD, output);
    }
    catch (CError &err) {
        fprintf(stderr, err.message);
        fprintf(stderr, "\n");
        exit(1);
    }
    
    output.close();
    
    return 0;
}

/***
 * Convert colour images to greyscale images
 */
CByteImage convertToGrey(CByteImage &image, int w, int h) {
    CByteImage temp;
    CShape sh = image.Shape();
    sh.nBands = 1;
    temp.ReAllocate(sh);
    
    int y, x, sum;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			sum = 0;
			sum += .114 * image.Pixel(x, y, 0);	// blue
			sum += .587 * image.Pixel(x, y, 1);	// green
			sum += .299 * image.Pixel(x, y, 2);	// red
			temp.Pixel(x, y, 0) = sum;
		}
	}
    
    return temp;
}

/***
 * Turn an image into integral map
 */
void calcIntegrals(CByteImage &input, CIntImage &densMap, int w, int h) {
    CShape sh = input.Shape();
    densMap.ReAllocate(sh);
    
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            DSMAP(x, y) = INPUT(x, y);
            if (inside(x - 1, y, w, h))
                DSMAP(x, y) += INPUT(x - 1, y);
            if (inside(x, y - 1, w, h))
                DSMAP(x, y) += INPUT(x, y - 1);
            
            if (inside(x - 1, y - 1, w, h))
                DSMAP(x, y) -= INPUT(x - 1, y - 1);
        }
    }
}

/***
 * Calculate the density around each pixel
 * using the integral image we just got
 */
void writeToFile(CIntImage &densMap, int w, int h, int rad, ofstream &output) {
    int rows = 0, cols = 0;
    int density;
    for (int y = rad + 1; y < h - rad; y++) {
        for (int x = rad + 1; x < w - rad; x++) {
            density = DSMAP(x + rad, y + rad)
                    + DSMAP(x - rad - 1, y - rad - 1)
                    - DSMAP(x - rad - 1, y + rad)
                    - DSMAP(x + rad, y - rad - 1);
            
            // density = abs(density - 255);
            output << toChar(density * (rad + rad + 1) * (rad + rad + 1));
            cols++;
        }
        output << '\n';
        rows++;
    }
    
    cout << "Result Image: \n";
    cout << "Width: " << cols / rows << "\t";
    cout << "Height: " << rows << "\t";
    cout << "Bands: " << 1 << "\n";
}

/***
 * Checks if a pixel is inside the image
 */
bool inside(int x, int y, int w, int h) {
    return x >= 0 && x < w
        && y >= 0 && y < h;
}


/*** 
 * Convert greyness into character to output
 * In order of desnity (rectified to 0 ~ 255):
 * @ # 8 & o : * .
 */
// char toChar(int density) {
//     if (density <= 50)
//         return ' ';
//     if (density <= 70)
//         return '.';
//     else if (density <= 100)
//         return '*';
//     else if (density <= 130)
//         return ':';
//     else if (density <= 160)
//         return 'o';
//     else if (density <= 180)
//         return '&';
//     else if (density <= 200)
//         return '8';
//     else if (density <= 230)
//         return '#';
//     else
//         return '@';
// }


// /***
//  * Convert greyness into character to output
//  * In order of desnity (rectified to 0 ~ 255):
//  * M N m d h y s o + / : - . '
//  */
char toChar(int density) {
    if (density <= 17)
        return ' ';
    else if (density <= 34)
        return '\'';
    else if (density <= 51)
        return '.';
    else if (density <= 68)
        return '-';
    else if (density <= 85)
        return ':';
    else if (density <= 102)
        return '/';
    else if (density <= 119)
        return '+';
    else if (density <= 136)
        return 'o';
    else if (density <= 153)
        return 's';
    else if (density <= 170)
        return 'y';
    else if (density <= 187)
        return 'h';
    else if (density <= 204)
        return 'd';
    else if (density <= 221)
        return 'm';
    else if (density <= 238)
        return 'N';
    else
        return 'M';
}