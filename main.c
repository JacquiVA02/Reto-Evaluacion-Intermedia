#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <omp.h>
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
    char archivos[100][100];
    int kernel;
    int total = 0;

    double inicio = omp_get_wtime();
    FILE* reporte = fopen("results.txt", "w");
    if(!reporte){
        perror("No se pudo crear el archivo .txt de salida");
        return 1;
    }

    int lecturas = 6;
    int escrituras = 6;
    int instrucciones = (lecturas + escrituras) * 20;

    do{
        printf("Ingrese el valor de kernel para el blur (de 55 a 155): ");
        scanf("%d", &kernel);
    } while(kernel < 55 || kernel > 155);
    

    // Abrir carpeta 'images' y obtener rutas válidas
    dir = opendir("images/");
    if (!dir) {
        perror("No se pudo abrir el directorio 'imagenes'");
        return 1;
    }

    while((ent = readdir(dir)) != NULL && total < 100){
        if(strstr(ent->d_name, ".bmp") != NULL){
            sprintf(archivos[total], "images/%s", ent->d_name);
            total++;
        }
    }
    closedir(dir);

    #pragma omp parallel for schedule(dynamic)
    //while ((ent = readdir(dir)) != NULL) 
    for(int i = 0; i < total; i++){
        //if (strstr(namelist[i]->d_name, ".bmp") == NULL) continue;

        char *ruta_entrada = archivos[i];
        const char *nombre_archivo = strrchr(ruta_entrada, '/');
        nombre_archivo = (nombre_archivo) ? nombre_archivo + 1 : ruta_entrada;

            // Obtener nombre base (sin extensión)
            //char nombre_base[256];
            //strcpy(nombre_base, ent->d_name);
            //char *punto = strrchr(nombre_base, '.');
            //if (punto) *punto = '\0';
 
            // Crear carpeta destino
            //char carpeta_resultado[300];
            //sprintf(carpeta_resultado, "results/%s", nombre_base);
            // crear_directorio_si_no_existe("results");
            // crear_directorio_si_no_existe(carpeta_resultado);

            char out1[300], out2[300], out3[300], out4[300], out5[300], out_blur[300];
            char sufijo_blur[64];

            generar_nombre_salida(nombre_archivo, "grayscale", out1);
            generar_nombre_salida(nombre_archivo, "mirrorh", out2);
            generar_nombre_salida(nombre_archivo, "mirrorv", out3);
            generar_nombre_salida(nombre_archivo, "gray_mirrorh", out4);
            generar_nombre_salida(nombre_archivo, "gray_mirrorv", out5);
            sprintf(sufijo_blur, "blur_%d", kernel);
            generar_nombre_salida(nombre_archivo, sufijo_blur, out_blur);

            // Procesamiento de imagenes
            grayscale(ruta_entrada, out1);
            mirror_horizontal(ruta_entrada, out2);
            mirror_vertical(ruta_entrada, out3);
            grayscale_mirror_horizontal(ruta_entrada, out4);
            grayscale_mirror_vertical(ruta_entrada, out5);
            blur_image(ruta_entrada, out_blur, kernel);

             //printf("Imagen procesada: %s -> kernel de %d\n", namelist[i]->d_name, kernel);
            printf("Hilo %d terminado - Imagen procesada: %s\n", omp_get_thread_num(), nombre_archivo);

            fprintf(reporte, "Imagen: %s -> Lecturas: %d, Escrituras: %d, Instrucciones: %d\n", nombre_archivo, lecturas, escrituras, instrucciones);
        
    }

    double fin = omp_get_wtime();
    double tiempo_total = fin - inicio;

    int total_lecturas = total * 6;
    int total_escrituras = total * 6;
    int total_instrucciones = (total_lecturas + total_escrituras) * 20;

    double MIPS = total_instrucciones / (1e6 * tiempo_total);

    fprintf(reporte, "\nResumen general:\n");
    fprintf(reporte, "Tiempo total de ejecución: %.4f segundos\n", tiempo_total);
    fprintf(reporte, "Total de instrucciones: %d\n", total_instrucciones);
    fprintf(reporte, "MIPS estimados: %.10f\n", MIPS);

    fclose(reporte);

    //closedir(dir);
    printf("** PROCESAMIENTO TERMINADO. **\n");
    return 0;
}