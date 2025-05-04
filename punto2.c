#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *image, *outputImage;
    image = fopen("sample.bmp", "rb");
    outputImage = fopen("mirror_horizontal.bmp", "wb");

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

    // Calcular padding por fila
    int padding = (4 - (ancho * 3) % 4) % 4;

    // Crear buffer para una fila (pixeles + padding)
    unsigned char *fila = (unsigned char *)malloc((ancho * 3 + padding) * sizeof(unsigned char));

    for (int i = 0; i < alto; i++) {
        fread(fila, sizeof(unsigned char), ancho * 3 + padding, image);

        // Escribir píxeles en orden inverso (espejo horizontal)
        for (int j = ancho - 1; j >= 0; j--) {
            fwrite(&fila[j * 3], sizeof(unsigned char), 3, outputImage);
        }

        // Escribir el mismo padding
        fwrite(&fila[ancho * 3], sizeof(unsigned char), padding, outputImage);
    }

    free(fila);
    fclose(image);
    fclose(outputImage);

    printf("Imagen generada: mirror_horizontal.bmp\n");
    return 0;
}
