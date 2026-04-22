# Filtered Back Projection (FBP) Reconstruction

## 📌 Project Overview

This project implements Filtered Back Projection (FBP), a fundamental algorithm in computed tomography (CT) and medical imaging for reconstructing images from projection data (sinograms). The focus is on improving the accuracy and usability of reconstructions when only partial input is available, which is a common challenge in medical imaging where radiation exposure must be minimized.

## 📋 Description

Optimizing Medical Image Reconstruction with Incomplete Projection Data

Radiation from medical imaging carries long-term risks including DNA damage and cancer, leading to limited scanning angles or missing data. However, lower doses produce high-noise images that minimize image quality from equipment such as CT scans, X-rays, and MRIs. This issue is approached by applying Filtered Back Projection (FBP), which is a reconstruction technique that reduces blurred information and improves the accuracy and usability of reconstructions to recover high-quality images from incomplete projection data.

In this study, we analyze the weaknesses of FBP in such situations and experiment with iterative techniques to fill in gaps in the data. Our results show that Filtered Back Projection improves the speed of reconstruction, which is a significant advantage along with the fine-grain appearance of noise compared to other de-blurring techniques, producing clear visualization of soft-tissue boundaries and fine anatomical structures even with limited projection data.

## 🚀 Features

- **Sinogram Generation**: Convert 2D phantom images into sinograms through rotational projections
- **Filtered Back Projection**: Apply ramp filtering in the frequency domain using DFT/IDFT
- **Image Reconstruction**: Reconstruct the original image from filtered sinograms
- **Phantom Loading**: Support for predefined phantoms (head phantom, shapes phantom)
- **Custom Phantom Creation**: Ability to create and process custom phantoms programmatically
- **Visualization**: HTML-based image display using Canvas API for easy viewing of results
- **Normalization**: Automatic scaling of image intensities to 0-255 range

## 📋 Requirements

- C++ compiler (g++ recommended)
- Standard C++ libraries (included with most compilers)
- HTML template file (`html.dat`) for visualization

## 🛠️ Installation

1. Clone or download the project files
2. Ensure `html.dat` is present in the root directory (used for HTML visualization)
3. Compile the program using g++:

```bash
g++ main.cpp -o fbp_reconstruction
```

## 📖 Usage

### Running the Program

Compile and run the program:

```bash
g++ main.cpp
./a.out
```

The program will process the Shepp-Logan head phantom by default and generate HTML visualization files in the `output/` directory.

### Creating Your Own Reconstruction Using Available Phantoms

You can modify the `main.cpp` to load different phantoms. For example, to process the shapes phantom:

```cpp
clearImage(Pixels);
clearImage(newSinogram);
clearImage(reconstruction);

loadImage(Pixels, "phantoms/shapesPhantom.txt");
Display image01Pixels(Pixels, "Phantom1");
processImageWithPhantom(Pixels);
Display image01Sinogram(Pixels, "Phantom1_sinogram");
filterBackProjection(Pixels, newSinogram);
normalizeImageColors(Pixels);
Display image01fbp(Pixels, "Phantom1_filtered");
reconstructImageFromSinogram(Pixels, reconstruction);
normalizeImageColors(reconstruction);
Display image01Reconstruction(reconstruction, "Phantom1_reconstruction");
```

### Creating a Phantom Yourself and Finding the FBP

You can create custom phantoms programmatically. For example, this code creates a triangle and square adjacent to each other:

```cpp
clearImage(Pixels);
clearImage(newSinogram);
clearImage(reconstruction);

// Create a square
for (int row = 100; row < 150; row++)
{
    for (int col = 110; col < 160; col++)
    {
        Pixels[row][col] = 125;
    }
}

// Create a circle
int positionXonTheAxis = 50;
int radius = 20;
for (int row = 0; row < N; row++)
{
    for (int col = 0; col < N; col++)
    {
        int xDistanceFromCenter = col - positionXonTheAxis;
        int yDistanceFromCenter = row - positionXonTheAxis;
        float distance = sqrt(xDistanceFromCenter * xDistanceFromCenter + yDistanceFromCenter * yDistanceFromCenter);

        if (distance <= radius)
        {
            Pixels[row][col] = 255;
        }
    }
}

Display image3(Pixels, "Phantom_03");
processImageWithPhantom(Pixels);
Display image03Sinogram(Pixels, "Phantom_03_sinogram");
filterBackProjection(Pixels, newSinogram);
normalizeImageColors(Pixels);
Display image03fbp(Pixels, "Phantom3_filtered");
reconstructImageFromSinogram(newSinogram, reconstruction);
normalizeImageColors(reconstruction);
Display image03Reconstruction(reconstruction, "Phantom3_reconstruction");
```

## 📁 File Structure

- `main.cpp`: Main program file containing FBP implementation
- `Display.h`: Header file for HTML visualization class
- `phantoms/`: Directory containing predefined phantom images
    - `headPhantom.txt`: Shepp-Logan head phantom
    - `shapesPhantom.txt`: Geometric shapes phantom
    - `sinogram_400x400.txt`: Pre-computed sinogram
- `output/`: Directory where HTML visualization files are generated
- `html.dat`: Template file for HTML visualization

## 🔧 Technical Details

- **Image Size**: 180x180 pixels
- **Projections**: 180 angles (1° increments)
- **Filtering**: Ramp filter applied in frequency domain using DFT
- **Interpolation**: Bilinear interpolation for image rotation
- **Normalization**: Linear scaling to 0-255 range

## 📊 Algorithm Overview

1. **Sinogram Creation**: Rotate the phantom image through 180 angles and sum projections
2. **Filtering**: Apply ramp filter to each projection row using DFT/IDFT
3. **Back Projection**: Reconstruct image by back-projecting filtered sinograms
4. **Visualization**: Generate HTML files with canvas-based image display

## 🤝 Contributing

Feel free to contribute by:

- Adding new phantom types
- Implementing additional filters (cosine, Hamming, etc.)
- Optimizing performance
- Adding support for different image formats

## 📄 License

This project is for educational and research purposes. Please cite appropriately if used in academic work.

## 📞 Contact

For questions or suggestions, please refer to the project documentation or create an issue.
