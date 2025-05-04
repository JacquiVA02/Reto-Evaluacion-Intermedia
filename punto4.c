#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *image, *outputImage;
    image = fopen("sample.bmp", "rb");          // Imagen original
    outputImage = fopen("img2_dd_mirror.bmp", "wb");  // Imagen transformada

    if (!image || !outputImage) {
        printf("Error al abrir los archivos.\n");
        return 1;
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, image);
    fwrite(header, sizeof(unsigned char), 54, outputImage);

    // Obtener dimensiones
    int ancho = *(int*)&header[18];
    int alto = *(int*)&header[22];
    int padding = (4 - (ancho * 3) % 4) % 4;

    unsigned char *fila = (unsigned char *)malloc(ancho * 3 + padding);
    unsigned char *grisFila = (unsigned char *)malloc(ancho * 3);

    for (int i = 0; i < alto; i++) {
        fread(fila, sizeof(unsigned char), ancho * 3 + padding, image);

        // Convertir a escala de grises y reflejar
        for (int j = 0; j < ancho; j++) {
            int idx = j * 3;
            unsigned char b = fila[idx];
            unsigned char g = fila[idx + 1];
            unsigned char r = fila[idx + 2];
            unsigned char gray = 0.21 * r + 0.72 * g + 0.07 * b;

            // Insertar al revés (espejo horizontal)
            int revIdx = (ancho - 1 - j) * 3;
            grisFila[revIdx] = gray;
            grisFila[revIdx + 1] = gray;
            grisFila[revIdx + 2] = gray;
        }

        // Escribir la fila reflejada
        fwrite(grisFila, sizeof(unsigned char), ancho * 3, outputImage);

        // Escribir el padding original
        fwrite(&fila[ancho * 3], sizeof(unsigned char), padding, outputImage);
    }

    free(fila);
    free(grisFila);
    fclose(image);
    fclose(outputImage);

    printf("Imagen generada: img2_dd_mirror.bmp\n");
    return 0;
}
