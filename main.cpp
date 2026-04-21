#include <iostream>
#include <cmath>
#include <string>
#include <complex>
#include <iomanip>
#include <fstream>
#include <climits>
#include "Display.h"

using namespace std;

const int N = 180;
const int HALF = (N - 1) / 2;
typedef complex<double> Complex;

// ramp filter
void createFilter(double[]);
// clear image
void clearImage(int pixels[][N]);
// loads an already made sinogram into the pixels
void loadImage(int pixels[][N], string fileName);
// process the images to turn it into a sinogram
void processImageWithPhantom(int Pixels[][N]);
// image rotation
void rotateImage(int pixels[][N], int newPixels[][N], double angle);
// this normalization is for finding the value between 0 - 255
void normalizeImageColors(int sinogram[][N]);
// for fbp
void filterBackProjection(int Pixels[][N], int newSinogram[][N]);
// for calculating dft
void calculateDFT(Complex dft[], double signal[], int N);
// for separating real and imaginary parts
void extractCoefficients(Complex dft[], double realParts[], double imagParts[], int N);
// for calulating idft
void calculateIDFT(Complex dft[], double signal[], int N);
// for normalization to 0-255
void normalizeImageColors(int pixels[][N]);
// reconstruction
void reconstructImageFromSinogram(int sinogram[][N], int reconstruction[][N]);
//cosine filter just like regular ramp
void createCosineFilter(double filter[]);

int main()
{
    int Pixels[N][N];
    int newSinogram[N][N];
    int reconstruction[N][N];




    clearImage(Pixels);
    clearImage(newSinogram);
    clearImage(reconstruction);

    //Processing a shepp Logan Phantom 
    loadImage(Pixels, "phantoms/headPhantom.txt");
    Display image01Pixels(Pixels, "Phantom1");
    processImageWithPhantom(Pixels);
    // Display image01Sinogram(Pixels, "Phantom1_sinogram");
    filterBackProjection(Pixels, newSinogram);
    normalizeImageColors(Pixels);
    // Display image01fbp(Pixels, "Phantom1_filtered");
    reconstructImageFromSinogram(newSinogram, reconstruction);
    normalizeImageColors(reconstruction);
    Display image01Reconstruction(reconstruction, "Phantom1_reconstruction");


    return 0;
}


void clearImage(int pixels[][N])
{
    for (int row = 0; row < N; row++)
        for (int col = 0; col < N; col++)
            pixels[row][col] = 0;
}

void processImageWithPhantom(int Pixels[][N])
{
    int sinogram[N][N] = {};

    for (int i = 0; i < N; i++)
    {
        int newPixels[N][N];

        clearImage(newPixels);
        rotateImage(Pixels, newPixels, (-1) * i);

        for (int row = 0; row < N; row++)
        {
            for (int col = 0; col < N; col++)
            {
                sinogram[i][col] += newPixels[row][col];
            }
        }
    }

    normalizeImageColors(sinogram);

    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            Pixels[r][c] = sinogram[r][c];
}

void rotateImage(int pixels[][N], int newPixels[][N], double angle)
{
    double delta = angle * M_PI / 180.0;
    double cosineOfDelta = cos(delta);
    double sineOfDelta = sin(delta);

    
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            double x = j - HALF;
            double y = i - HALF;

            double srcX = x * cosineOfDelta + y * sineOfDelta;
            double srcY = -x * sineOfDelta + y * cosineOfDelta;

            double idx = srcX + HALF;
            double idy = srcY + HALF;

            int x1 = (int)floor(idx);
            int y1 = (int)round(idy);
            int x2 = x1 + 1;
            double dx = idx - x1;

            double interpolated = 0.0;
            if (y1 >= 0 && y1 < N)
            {
                if (x1 >= 0 && x2 < N)
                {
                    double p1 = pixels[y1][x1];
                    double p2 = pixels[y1][x2];
                    interpolated = (1 - dx) * p1 + dx * p2;
                }
                else if (x1 >= 0 && x1 < N)
                {
                    interpolated = pixels[y1][x1];
                }
            }
            newPixels[i][j] = (int)round(interpolated);
        }
    }
}

void loadImage(int pixels[][N], string fileName)
{
    ifstream file;
    file.open(fileName);
    if (!file)
        cout << "File does not exist!";
    for (int row = 0; row < N; row++)
        for (int col = 0; col < N; col++)
            file >> pixels[row][col];
    file.close();
}

void filterBackProjection(int Pixels[][N], int newSinogram[][N])
{
    // create an empty image where you'll store each row one by one to complete 180 rows
    for (int i = 0; i < N; i++)
    {
        double eachRow[N];
        Complex dft1[N];
        double therealParts[N];
        double imaginaryParts[N];
        double rampFilter[N];
        Complex dft2[N];
        for (int k = 0; k < N; k++)
        {
            // create an empty array of size 180 where youll store a single row to perform dft and ramp
            eachRow[k] = Pixels[i][k];
        }
        // dft setup
        calculateDFT(dft1, eachRow, N);

        // declare real and imaginary part for separation
        extractCoefficients(dft1, therealParts, imaginaryParts, N);

        // create a ramp filter so you can multiply both the real and imaginary
        // createFilter(rampFilter);
        createFilter(rampFilter);

        // multiply both by rampfilter
        for (int r = 0; r < N; r++)
        {
            therealParts[r] *= rampFilter[r];
            imaginaryParts[r] *= rampFilter[r];
        }

        // perform Inverse dft/idft
        for (int m = 0; m < N; m++)
        {
            dft2[m] = Complex(therealParts[m], imaginaryParts[m]);
        }

        double backToEachRow[N];
        calculateIDFT(dft2, backToEachRow, N);

        // put the backtoeachrow into the new sinogram
        for (int b = 0; b < N; b++)
        {
            newSinogram[i][b] = backToEachRow[b];
        }
    }

    // for loop that sends all the values of newSinogram back into pixels
    for (int row = 0; row < N; row++)
    {
        for (int col = 0; col < N; col++)
        {
            Pixels[row][col] = newSinogram[row][col];
        }
    }
}

void calculateDFT(Complex dft[], double signal[], int N)
{
    const double PI = 3.141592653589793116;

    for (int k = 0; k < N; k++)
    {
        Complex sum(0, 0);
        for (int n = 0; n < N; n++)
        {
            double angle = -2.0 * PI * k * n / N;
            Complex exponential(cos(angle), sin(angle));
            sum += signal[n] * exponential;
        }
        dft[k] = sum;
    }
}

void extractCoefficients(Complex dft[], double realParts[], double imagParts[], int N)
{
    for (int k = 0; k < N; k++)
    {
        realParts[k] = dft[k].real();
        imagParts[k] = dft[k].imag();
    }
}

void createFilter(double filter[])
{
    double c;
    for (int i = 0; i < N; i++)
    {
        c = abs(i - N / 2);
        filter[i] = (N - 2 * c) / N;
    }
}

void createCosineFilter(double filter[])
{
    double c;
    double ramp;
    for (int i = 0; i < N; i++)
    {
        c = abs(i - N / 2);
        ramp = (N - 2 * c) / (double)N;
        filter[i] = (N - 2 * c) / N;
    }
}

void calculateIDFT(Complex dft[], double signal[], int N)
{
    const double PI = 3.141592653589793;

    for (int n = 0; n < N; n++)
    {
        Complex sum(0, 0);
        for (int k = 0; k < N; k++)
        {
            double angle = 2.0 * PI * k * n / N;
            Complex exponential(cos(angle), sin(angle));
            sum += dft[k] * exponential;
        }
        signal[n] = sum.real() / N;
    }
}

void normalizeImageColors(int pixels[][N])
{
    // Find min and max values for normalization to 0-255
    int minVal = INT_MAX;
    int maxVal = INT_MIN;
    for (int row = 0; row < N; row++)
        for (int col = 0; col < N; col++)
        {
            if (pixels[row][col] < minVal) {
                minVal = pixels[row][col];
            }
                
            if (pixels[row][col] > maxVal){
                maxVal = pixels[row][col];
            }
        }

    if (maxVal == minVal)
    {
        
        for (int row = 0; row < N; row++)
            for (int col = 0; col < N; col++)
                pixels[row][col] = 0;
        return;
    }

    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            pixels[row][col] = (int)((pixels[row][col] - minVal) * 255.0 / (maxVal - minVal));
        }

    }

}

void reconstructImageFromSinogram(int sinogram[][N], int reconstruction[][N])
{
    double accum[N][N] = {0.0};

    for (int i = 0; i < N; i++)
    {
        double theta = i * M_PI / 180.0; // angle in radians
        double cosTheta = cos(theta);
        double sinTheta = sin(theta);

        for (int x = 0; x < N; x++)
        {
            for (int y = 0; y < N; y++)
            {
                double cx = x - HALF;
                double cy = y - HALF;
                double s = cy * cosTheta + cx * sinTheta;
                int det = round(s + HALF);
                if (det >= 0 && det < N)
                {
                    accum[x][y] += sinogram[i][det];
                }
            }
        }
    }

    // Normalize and convert to int
    double maxVal = 0.0;
    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            if (accum[x][y] > maxVal)
                maxVal = accum[x][y];

    if (maxVal > 0)
    {
        for (int x = 0; x < N; x++)
            for (int y = 0; y < N; y++)
                reconstruction[x][y] = (int)(accum[x][y] * 255.0 / maxVal);
    }
}

// g++ main.cpp
// ./a.out