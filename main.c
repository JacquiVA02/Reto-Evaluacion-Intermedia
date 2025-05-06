// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "image_processing.h"


void generar_nombre_salida(const char* original, const char* sufijo, char* destino) {
    const char *nombre = strrchr(original, '/');
    nombre = (nombre) ? nombre + 1 : original;

    char base[256];
    strcpy(base, nombre);
    char *punto = strrchr(base, '.');
    if (punto) *punto = '\0';

    sprintf(destino, "results/%s_%s.bmp", base, sufijo);
}

int main() {
    DIR *dir;
    struct dirent *ent;
    int kernel;

    do{
        printf("Ingrese el valor de kernel para el blur (de 55 a 155): ");
        scanf("%d", &kernel);
    } while(kernel < 55 || kernel > 155);
    

    dir = opendir("images/");
    if (dir == NULL) {
        perror("No se pudo abrir el directorio 'imagenes'");
        return 1;
    }


    while ((ent = readdir(dir)) != NULL) {
        if (strstr(ent->d_name, ".bmp") != NULL) {
            char ruta_entrada[300];
            sprintf(ruta_entrada, "images/%s", ent->d_name);

            // Obtener nombre base (sin extensión)
            char nombre_base[256];
            strcpy(nombre_base, ent->d_name);
            char *punto = strrchr(nombre_base, '.');
            if (punto) *punto = '\0';
 
            // Crear carpeta destino
            char carpeta_resultado[300];
            sprintf(carpeta_resultado, "results/%s", nombre_base);
            // crear_directorio_si_no_existe("results");
            // crear_directorio_si_no_existe(carpeta_resultado);

            char out1[300], out2[300], out3[300], out4[300], out5[300];

            generar_nombre_salida(ent->d_name, "grayscale", out1);
            generar_nombre_salida(ent->d_name, "mirrorh", out2);
            generar_nombre_salida(ent->d_name, "mirrorv", out3);
            generar_nombre_salida(ent->d_name, "gray_mirrorh", out4);
            generar_nombre_salida(ent->d_name, "gray_mirrorv", out5);

            grayscale(ruta_entrada, out1);
            mirror_horizontal(ruta_entrada, out2);
            mirror_vertical(ruta_entrada, out3);
            grayscale_mirror_horizontal(ruta_entrada, out4);
            grayscale_mirror_vertical(ruta_entrada, out5);

            char salida[300];
            char sufijo_blur[64];
            sprintf(sufijo_blur, "blur_%d", kernel);
            generar_nombre_salida(ent->d_name, sufijo_blur, salida);

            blur_image(ruta_entrada, salida, kernel);
            printf("Imagen procesada: %s -> kernel de %d\n", ent->d_name, kernel);
           
        }
    }

    closedir(dir);
    return 0;
}
