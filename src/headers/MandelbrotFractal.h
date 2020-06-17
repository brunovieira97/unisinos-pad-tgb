#include <stdio.h>
#include <stdexcept>
#include <pthread.h>
#include <time.h>
#include <math.h>

#ifndef MANDELBROT_FRACTAL_H
#define MANDELBROT_FRACTAL_H

// Pixel with RGB value
typedef unsigned char Pixel[3];

struct ThreadArguments {
	int
		threadId,
		startX,
		endX,
		startY,
		endY;
	double
		minBoundsReal,
		minBoundsImaginary,
		scaleReal,
		scaleImaginary;
	Pixel
		*colors,
		*pixels;
};

struct Complex {
	double
		real,
		imaginary;
};

static const int
	MAX_THREAD_COUNT	= 32,	// Maximum allowed thread count
	DIV_HEIGHT			= 10,	// Vertical divisions
	DIV_WIDTH			= 10,	// Horizontal divisions
	IMG_HEIGHT			= 5000,	// PPM image height
	IMG_WIDTH			= 5000,	// PPM image width
	MAX_ITERATIONS		= 100;	// Maximum number of iterations

static const unsigned char MAX_RGB_VALUE = 255;
static const double ZOOM = 2;

void generate();
void calculateColors(Pixel* colors);
void* workerThreadExecute(void* threadArguments);

#endif
