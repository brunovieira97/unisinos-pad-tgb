#include <headers/MandelbrotFractal.h>


static const Complex focusPoint = {
	.real		= -0.5,
	.imaginary	=  0
};


pthread_mutex_t threadMutexLock;


void printParameters() {
	printf("Parameters:\n");
	printf("	Image dimensions: %d x %d\n", IMG_WIDTH, IMG_HEIGHT);
	printf("	Max iterations: %d\n", MAX_ITERATIONS);
	printf("	Sector count (each thread does 1 sector): %d\n\n", (DIV_HEIGHT * DIV_WIDTH));
}

void generate() {
	Pixel* pixels = new Pixel[IMG_HEIGHT * IMG_WIDTH];
	Pixel* colors = new Pixel[MAX_ITERATIONS + 1];

	calculateColors(colors);
	colors[MAX_ITERATIONS][0] = MAX_RGB_VALUE;
	colors[MAX_ITERATIONS][1] = MAX_RGB_VALUE;
	colors[MAX_ITERATIONS][2] = MAX_RGB_VALUE;

	const Complex
		minBounds = {
			.real		= focusPoint.real		- ZOOM,
			.imaginary	= focusPoint.imaginary	- ZOOM
		},
		maxBounds = {
			.real		= focusPoint.real		+ ZOOM,
			.imaginary	= focusPoint.imaginary	+ ZOOM
		},
		scale = {
			.real		= (maxBounds.real - minBounds.real) / IMG_WIDTH,
			.imaginary	= (maxBounds.real - minBounds.real) / IMG_HEIGHT
		};

	int
		qtdX = (IMG_WIDTH / DIV_WIDTH),
		qtdY = (IMG_HEIGHT / DIV_HEIGHT),
		tmpCont = 0,
		threadCount = DIV_WIDTH * DIV_HEIGHT;
		
	int (*numThread)[4] = new int[threadCount][4];

	int imgY = 0;
	while (imgY < IMG_HEIGHT) {
		int imgX = 0;
		while (imgX < IMG_WIDTH) {
			numThread[tmpCont][0] = imgX;
			numThread[tmpCont][1] = imgY;
			numThread[tmpCont][2] = imgX + qtdX;
			numThread[tmpCont][3] = imgY + qtdY;

			tmpCont++;
			imgX += qtdX;
		}

		imgY += qtdY;
	}

	pthread_t* threads = new pthread_t[threadCount];
	ThreadArguments* threadArguments = new ThreadArguments[threadCount];

	for (int i = 0; i < threadCount; i++) {
		ThreadArguments currentThreadArguments = {
			.threadId = i,

			.startX = numThread[i][0],
			.endX = numThread[i][2],
			.startY = numThread[i][1],
			.endY = numThread[i][3],

			.minBoundsReal = minBounds.real,
			.minBoundsImaginary = minBounds.imaginary,
			.scaleReal = scale.real,
			.scaleImaginary = scale.imaginary,
			.colors = colors,
			.pixels = pixels
		};

		threadArguments[i] = currentThreadArguments;

		pthread_create(&threads[i], NULL, threadFunction, &threadArguments[i]);
	}

	// Wait for threads to complete
	for (int i = 0; i < threadCount; i++)
		pthread_join(threads[i], NULL);

	printf("Writing image...\n");

	FILE *fp = fopen("MandelbrotSet.ppm", "wb");
    fprintf(fp, "P6\n %d %d\n %d\n", IMG_WIDTH, IMG_HEIGHT, MAX_RGB_VALUE);
    fwrite(pixels, sizeof(Pixel), IMG_WIDTH * IMG_HEIGHT, fp);
    fclose(fp);

	printf("Image MandelbrotSet.ppm created.\n");
	printf("Destroying mutex...\n");

    pthread_mutex_destroy(&threadMutexLock);
}

void* threadFunction(void* threadArguments) {
	ThreadArguments* args = static_cast<ThreadArguments*>(threadArguments);

	const clock_t startTime = clock();

	printf("[Thread %d]: Started\n", args -> threadId);

	for (int y = args -> startY; (y < args -> endY) && (y < IMG_HEIGHT); y++)
		for (int x = args -> startX; (x < args -> endX) && (x < IMG_WIDTH); x++) {
			Complex c = {
				.real		= (args -> minBoundsReal		+ (x * args -> scaleReal)),
				.imaginary	= (args -> minBoundsImaginary	+ (y * args -> scaleImaginary))
			};

			Complex z = {
				.real		= 0,
				.imaginary	= 0
			};

			Complex squaredZ = {
				.real		= 0,
				.imaginary	= 0
			};

			int iterations = 0;
			while ((squaredZ.real + squaredZ.imaginary <= 4) && (iterations <= MAX_ITERATIONS)) {
				z.imaginary  = z.real * z.imaginary;
				z.imaginary += z.imaginary;
				z.imaginary += c.imaginary;

				z.real = squaredZ.real - squaredZ.imaginary + c.real;

				squaredZ.real		= pow(z.real, 2);
				squaredZ.imaginary	= pow(z.imaginary, 2);

				iterations++;
			}

			pthread_mutex_lock(&threadMutexLock);

			args -> pixels[y * IMG_WIDTH + x][0] = args -> colors[iterations][0];
			args -> pixels[y * IMG_WIDTH + x][1] = args -> colors[iterations][1];
			args -> pixels[y * IMG_WIDTH + x][2] = args -> colors[iterations][2];

			pthread_mutex_unlock(&threadMutexLock);
		}
	
	printf(
		"[Thread %d] Ended at %d seconds\n",
		args -> threadId,
		(int) ((clock() - startTime) / CLOCKS_PER_SEC)
	);

	return NULL;
}

void calculateColors(Pixel* colors) {
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		double t = (double) i / MAX_ITERATIONS;

		colors[i][0] = (unsigned char) (9	* pow((1 - t), 1) * pow(t, 3) * MAX_RGB_VALUE);
        colors[i][1] = (unsigned char) (15	* pow((1 - t), 2) * pow(t, 2) * MAX_RGB_VALUE);
        colors[i][2] = (unsigned char) (8.5	* pow((1 - t), 3) * pow(t, 1) * MAX_RGB_VALUE);
	}
}
