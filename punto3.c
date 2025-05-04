#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *image, *outputImage;
    image = fopen("sample.bmp", "rb");
    outputImage = fopen("mirror_vertical.bmp", "wb");

    if (!image || !outputImage) {
        printf("Error al abrir archivos.\n");
        return 1;
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, image);
    fwrite(header, sizeof(unsigned char), 54, outputImage);

    // Obtener ancho y alto (little endian)
    int ancho = *(int*)&header[18];
    int alto = *(int*)&header[22];

    int filaSize = ancho * 3;
    int padding = (4 - (filaSize % 4)) % 4;
    int filaCompleta = filaSize + padding;

    // Reservar memoria para toda la imagen
    unsigned char *data = (unsigned char *)malloc(alto * filaCompleta);

    // Leer todas las filas (de arriba hacia abajo en BMP)
    fread(data, sizeof(unsigned char), alto * filaCompleta, image);

    // Escribir filas en orden inverso (de abajo hacia arriba)
    for (int i = alto - 1; i >= 0; i--) {
        fwrite(&data[i * filaCompleta], sizeof(unsigned char), filaCompleta, outputImage);
    }

    free(data);
    fclose(image);
    fclose(outputImage);

    printf("Imagen generada: mirror_vertical.bmp\n");
    return 0;
}
