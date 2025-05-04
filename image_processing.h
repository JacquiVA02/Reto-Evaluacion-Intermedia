// image_processing.h
#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>

void grayscale(const char *input_path, const char *output_path) {
    FILE *image = fopen(input_path, "rb");
    FILE *outputImage = fopen(output_path, "wb");
    if (!image || !outputImage) return;

    unsigned char r, g, b;
    for (int i = 0; i < 54; i++) fputc(fgetc(image), outputImage);
    while (!feof(image)) {
        b = fgetc(image);
        g = fgetc(image);
        r = fgetc(image);
        if (feof(image)) break;
        unsigned char pixel = 0.21 * r + 0.72 * g + 0.07 * b;
        fputc(pixel, outputImage);
        fputc(pixel, outputImage);
        fputc(pixel, outputImage);
    }
    fclose(image);
    fclose(outputImage);
}

void mirror_horizontal(const char *input_path, const char *output_path) {
    FILE *image = fopen(input_path, "rb");
    FILE *outputImage = fopen(output_path, "wb");
    if (!image || !outputImage) return;

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, image);
    fwrite(header, sizeof(unsigned char), 54, outputImage);

    int ancho = *(int*)&header[18];
    int alto = *(int*)&header[22];
    int padding = (4 - (ancho * 3) % 4) % 4;
    unsigned char *fila = (unsigned char *)malloc(ancho * 3 + padding);

    for (int i = 0; i < alto; i++) {
        fread(fila, sizeof(unsigned char), ancho * 3 + padding, image);
        for (int j = ancho - 1; j >= 0; j--) {
            fwrite(&fila[j * 3], sizeof(unsigned char), 3, outputImage);
        }
        fwrite(&fila[ancho * 3], sizeof(unsigned char), padding, outputImage);
    }

    free(fila);
    fclose(image);
    fclose(outputImage);
}

void mirror_vertical(const char *input_path, const char *output_path) {
    FILE *image = fopen(input_path, "rb");
    FILE *outputImage = fopen(output_path, "wb");
    if (!image || !outputImage) return;

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, image);
    fwrite(header, sizeof(unsigned char), 54, outputImage);

    int ancho = *(int*)&header[18];
    int alto = *(int*)&header[22];
    int filaSize = ancho * 3;
    int padding = (4 - (filaSize % 4)) % 4;
    int filaCompleta = filaSize + padding;

    unsigned char *data = (unsigned char *)malloc(alto * filaCompleta);
    fread(data, sizeof(unsigned char), alto * filaCompleta, image);

    for (int i = alto - 1; i >= 0; i--) {
        fwrite(&data[i * filaCompleta], sizeof(unsigned char), filaCompleta, outputImage);
    }

    free(data);
    fclose(image);
    fclose(outputImage);
}

void grayscale_mirror_horizontal(const char *input_path, const char *output_path) {
    FILE *image = fopen(input_path, "rb");
    FILE *outputImage = fopen(output_path, "wb");
    if (!image || !outputImage) return;

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, image);
    fwrite(header, sizeof(unsigned char), 54, outputImage);

    int ancho = *(int*)&header[18];
    int alto = *(int*)&header[22];
    int padding = (4 - (ancho * 3) % 4) % 4;

    unsigned char *fila = (unsigned char *)malloc(ancho * 3 + padding);
    unsigned char *grisFila = (unsigned char *)malloc(ancho * 3);

    for (int i = 0; i < alto; i++) {
        fread(fila, sizeof(unsigned char), ancho * 3 + padding, image);
        for (int j = 0; j < ancho; j++) {
            int idx = j * 3;
            unsigned char b = fila[idx];
            unsigned char g = fila[idx + 1];
            unsigned char r = fila[idx + 2];
            unsigned char gray = 0.21 * r + 0.72 * g + 0.07 * b;
            int revIdx = (ancho - 1 - j) * 3;
            grisFila[revIdx] = gray;
            grisFila[revIdx + 1] = gray;
            grisFila[revIdx + 2] = gray;
        }
        fwrite(grisFila, sizeof(unsigned char), ancho * 3, outputImage);
        fwrite(&fila[ancho * 3], sizeof(unsigned char), padding, outputImage);
    }

    free(fila);
    free(grisFila);
    fclose(image);
    fclose(outputImage);
}

void grayscale_mirror_vertical(const char *input_path, const char *output_path) {
    FILE *image = fopen(input_path, "rb");
    FILE *outputImage = fopen(output_path, "wb");
    if (!image || !outputImage) return;

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, image);
    fwrite(header, sizeof(unsigned char), 54, outputImage);

    int ancho = *(int*)&header[18];
    int alto = *(int*)&header[22];
    int padding = (4 - (ancho * 3) % 4) % 4;
    int filaSize = ancho * 3 + padding;

    unsigned char *data = (unsigned char *)malloc(alto * filaSize);
    unsigned char *grises = (unsigned char *)malloc(alto * filaSize);

    fread(data, sizeof(unsigned char), alto * filaSize, image);

    for (int i = 0; i < alto; i++) {
        for (int j = 0; j < ancho; j++) {
            int idx = i * filaSize + j * 3;
            unsigned char b = data[idx];
            unsigned char g = data[idx + 1];
            unsigned char r = data[idx + 2];
            unsigned char gray = 0.21 * r + 0.72 * g + 0.07 * b;
            grises[idx] = gray;
            grises[idx + 1] = gray;
            grises[idx + 2] = gray;
        }
        for (int p = 0; p < padding; p++) {
            grises[i * filaSize + ancho * 3 + p] = data[i * filaSize + ancho * 3 + p];
        }
    }

    for (int i = alto - 1; i >= 0; i--) {
        fwrite(&grises[i * filaSize], sizeof(unsigned char), filaSize, outputImage);
    }

    free(data);
    free(grises);
    fclose(image);
    fclose(outputImage);
}

struct imageMetadata {
    int width;
    int height;
    int imageSize;
    char name[512];
    char destinationFolder[128];
};

void blur_image(const char *input_path, const char *output_path, int kernelSize) {
    FILE *originalImage = fopen(input_path, "rb");
    FILE *outputImage = fopen(output_path, "wb");
    if (!originalImage || !outputImage) return;

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, originalImage);
    fwrite(header, sizeof(unsigned char), 54, outputImage);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int imageSize = *(int*)&header[34];

    unsigned char *pixelData = (unsigned char *)malloc(imageSize);
    fread(pixelData, imageSize, 1, originalImage);

    int kernelRadius = (kernelSize - 1) / 2;
    int widthWithPadding = (width * 3 + 3) & -4;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned int rSum = 0, gSum = 0, bSum = 0, count = 0;

            for (int ky = -kernelRadius; ky <= kernelRadius; ky++) {
                for (int kx = -kernelRadius; kx <= kernelRadius; kx++) {
                    int i = y + ky;
                    int j = x + kx;
                    if (i < 0 || i >= height || j < 0 || j >= width) continue;

                    int index = (i * widthWithPadding) + (j * 3);
                    bSum += pixelData[index];
                    gSum += pixelData[index + 1];
                    rSum += pixelData[index + 2];
                    count++;
                }
            }

            unsigned char bAvg = bSum / count;
            unsigned char gAvg = gSum / count;
            unsigned char rAvg = rSum / count;

            int index = (y * widthWithPadding) + (x * 3);
            pixelData[index] = bAvg;
            pixelData[index + 1] = gAvg;
            pixelData[index + 2] = rAvg;
        }
    }

    fwrite(pixelData, imageSize, 1, outputImage);

    fclose(outputImage);
    fclose(originalImage);
    free(pixelData);
}

#endif // IMAGE_PROCESSING_H
