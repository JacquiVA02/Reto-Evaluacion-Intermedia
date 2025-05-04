#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <direct.h>  // para Windows
#include <omp.h>


struct imageMetadata {
    int width;
    int height;
    int imageSize;
    char name[512];              // Nombre base del archivo (sin extensión)
    char destinationFolder[128]; // Carpeta de destino
    char inputPath[512];         // Ruta del archivo BMP original a color
};

void blur(unsigned short kernelSize, struct imageMetadata imageData) {
    FILE *originalImage = fopen(imageData.inputPath, "rb");
    if (!originalImage) {
        printf("Error al abrir la imagen original.\n");
        return;
    }

    char sufix[32];
    sprintf(sufix, "_blurred_%d.bmp", kernelSize);
    char outputFilename[1028];
    strcpy(outputFilename, imageData.destinationFolder);
    strcat(outputFilename, imageData.name);
    strcat(outputFilename, sufix);

    FILE *outputImage = fopen(outputFilename, "wb");

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, originalImage);
    fwrite(header, sizeof(unsigned char), 54, outputImage);

    unsigned char *pixelData = (unsigned char *)malloc(imageData.imageSize);
    fread(pixelData, imageData.imageSize, 1, originalImage);

    int kernelRadius = (kernelSize - 1) / 2;
    int widthWithPadding = (imageData.width * 3 + 3) & -4;

    #pragma omp parallel for
    for (int y = 0; y < imageData.height; y++) {
        for (int x = 0; x < imageData.width; x++) {
            unsigned int rSum = 0, gSum = 0, bSum = 0;
            int pond = 0;

            for (int ky = -kernelRadius; ky <= kernelRadius; ky++) {
                for (int kx = -kernelRadius; kx <= kernelRadius; kx++) {
                    int i = y + ky;
                    int j = x + kx;
                    if (i < 0 || i >= imageData.height || j < 0 || j >= imageData.width)
                        continue;

                    int index = (i * widthWithPadding) + (j * 3);
                    bSum += pixelData[index];
                    gSum += pixelData[index + 1];
                    rSum += pixelData[index + 2];
                    pond++;
                }
            }

            int index = (y * widthWithPadding) + (x * 3);
            pixelData[index]     = bSum / pond;
            pixelData[index + 1] = gSum / pond;
            pixelData[index + 2] = rSum / pond;
        }
    }


    fwrite(pixelData, imageData.imageSize, 1, outputImage);
    fclose(outputImage);
    fclose(originalImage);
    free(pixelData);
}

int main(int argc, char *argv[]) {
    char *filename = NULL;
    char *destinationFolder = NULL;
    int initialMask = 55;

    struct imageMetadata imageData;

    // Leer argumentos
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
            filename = argv[i + 1];
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc)
            destinationFolder = argv[i + 1];
        if (strcmp(argv[i], "-m") == 0 && i + 1 < argc && isdigit(argv[i + 1][0]))
            initialMask = atoi(argv[i + 1]);
    }

    _mkdir(destinationFolder);

    if (!filename || !destinationFolder) {
        printf("Uso: blur_seq -f archivo.bmp -d ./output/ [-m 55]\n");
        return 1;
    }

    // Guardar ruta completa del archivo
    strcpy(imageData.inputPath, filename);

    // Extraer nombre base (sin extensión)
    char *tmp = strrchr(filename, '/');
    tmp = (tmp) ? tmp + 1 : filename;

    char tmpCopy[512];
    strcpy(tmpCopy, tmp);
    char *dot = strrchr(tmpCopy, '.');
    if (dot) *dot = '\0';  // Quitar extensión
    strcpy(imageData.name, tmpCopy);

    // Guardar carpeta de destino
    strcpy(imageData.destinationFolder, destinationFolder);

    // Leer metadatos BMP
    FILE *originalImage = fopen(filename, "rb");
    if (!originalImage) {
        printf("Error al abrir la imagen de entrada.\n");
        return 1;
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, originalImage);
    imageData.width = *(int *)&header[18];
    imageData.height = *(int *)&header[22];
    imageData.imageSize = *(int *)&header[34];
    fclose(originalImage);

    // Verificación de apertura
    printf("Ruta del archivo que se intentará abrir: %s\n", imageData.inputPath);
    FILE *test = fopen(imageData.inputPath, "rb");
    if (!test) {
        printf("No se pudo abrir el archivo de entrada desde la ruta: %s\n", imageData.inputPath);
        return 1;
    }
    fclose(test);

    // Aplicar kernels de blur
    for (int k = initialMask; k <= 155; k += 2) {
        printf("Aplicando blur con kernel %d...\n", k);
        blur(k, imageData);
    }

    printf("Proceso terminado.\n");
    return 0;
}
