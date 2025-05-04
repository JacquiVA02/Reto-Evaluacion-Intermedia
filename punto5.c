#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *image, *outputImage;
    image = fopen("sample.bmp", "rb");
    outputImage = fopen("img_grayscale_vertical_mirror.bmp", "wb");

    if (!image || !outputImage) {
        printf("Error al abrir los archivos.\n");
        return 1;
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, image);
    fwrite(header, sizeof(unsigned char), 54, outputImage);

    // Obtener ancho y alto de la imagen
    int ancho = *(int*)&header[18];
    int alto = *(int*)&header[22];
    int padding = (4 - (ancho * 3) % 4) % 4;
    int filaSize = ancho * 3 + padding;

    // Reservar espacio para toda la imagen
    unsigned char *data = (unsigned char *)malloc(alto * filaSize);
    unsigned char *grises = (unsigned char *)malloc(alto * filaSize);

    // Leer toda la imagen
    fread(data, sizeof(unsigned char), alto * filaSize, image);

    // Convertir a grises fila por fila
    for (int i = 0; i < alto; i++) {
        for (int j = 0; j < ancho; j++) {
            int idx = i * filaSize + j * 3;
            unsigned char b = data[idx];
            unsigned char g = data[idx + 1];
            unsigned char r = data[idx + 2];

            unsigned char gray = 0.21 * r + 0.72 * g + 0.07 * b;

            grises[idx] = gray;       // B
            grises[idx + 1] = gray;   // G
            grises[idx + 2] = gray;   // R
        }
        // Copiar el padding sin cambios
        for (int p = 0; p < padding; p++) {
            grises[i * filaSize + ancho * 3 + p] = data[i * filaSize + ancho * 3 + p];
        }
    }

    // Escribir las filas en orden inverso (espejo vertical)
    for (int i = alto - 1; i >= 0; i--) {
        fwrite(&grises[i * filaSize], sizeof(unsigned char), filaSize, outputImage);
    }

    free(data);
    free(grises);
    fclose(image);
    fclose(outputImage);

    printf("Imagen generada: img_grayscale_vertical_mirror.bmp\n");
    return 0;
}
