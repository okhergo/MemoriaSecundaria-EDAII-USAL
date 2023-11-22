// OSCAR HERNANDEZ GOMEZ (70919457S) - GRUPO A1

#include <stdio.h>
#include <stdlib.h>
#include "dispersion.h"

// Definiciones de estructuras y funciones anteriores...

int main() {
    char *fichEntrada = "alumnos.dat";
    char *fichHash = "alumnosC.hash";
    regConfig regC;
    tipoAlumno *alumno;
    FILE *f;
    char *dni, *provincia;
    int nCubo, nCuboDes, posReg, error;

    // Parametros regConfig 
    regC.nCubos = 10;
    regC.nCubosDes = 4;
    regC.densidadMax = 120;
    regC.densidadMin = 40;
    regC.numReg = 0;
    regC.numRegDes = 0;
    regC.nCuboDesAct = regC.nCubos;

    // Crear el fichero Hash
    int resultado = creaHash(fichEntrada, fichHash, &regC);

    if (resultado == 0) {
        printf("Proceso finalizado correctamente.\n");
    } else {
        printf("Error en el proceso.\n");
        return -1;
    }

    leeHash(fichHash);

    // Buscar un registro
    dni = "11000258";
    printf("\nBUSQUEDA ALUMNO DNI %s:\n", dni);

    if ((f = fopen(fichHash, "rb")) == NULL) error = -2;
    alumno = busquedaHash(f, dni, &nCubo, &nCuboDes, &posReg, &error);
    if (error == -2) {
        printf("Error en el fichero de entrada\n");
        fclose(f);
        return -2;
    } else if (error == -4) {
        printf("Error en el proceso de busqueda\n");
        fclose(f);
        return -4;
    } else if (error == -1) {
        printf("El registro no existe\n");
        fclose(f);
        return -1;
    }
    
    if (nCuboDes == -1) printf("\tEl alumno se encuentra en el cubo %d, registro %d\n", nCubo, posReg);
    else printf("\tEl alumno (perteneciente al cubo %d desbordado) se encuentra en el cubo de desborde %d, registro %d\n", nCubo, nCuboDes, posReg);
    printf("\t%s %s %s %s %s\n",alumno->dni,alumno->nombre,alumno->ape1,alumno->ape2,alumno->provincia);
    fclose(f);

    // Modificar un registro
    dni = "8433839";
    provincia = "SANTANDER";
    printf("\nMODIFICAR ALUMNO DNI %s:\n", dni);
    nCubo = modificarReg(fichHash, dni, provincia);
    if(error == 0) printf("\tEl alumno se encuentra en el cubo %d\n", nCubo);

    return 0;
}