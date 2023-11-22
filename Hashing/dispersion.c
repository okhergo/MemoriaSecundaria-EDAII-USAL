// OSCAR HERNANDEZ GOMEZ (70919457S) - GRUPO A1

#include "dispersion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Lee el contenido del fichero hash organizado mediante el metodo de DISPERSION segun los criterios
// especificados en la practica. Se leen todos los cubos completos tengan registros asignados o no. La
// salida que produce esta funcion permite visualizar el metodo de DISPERSION

int leeHash(char *fichHash){ 
  FILE *f;
  tipoCubo cubo;
  regConfig regC;
  int j, nCubo=0, densidadOcupacion;

  f = fopen(fichHash,"rb");
  fread(&regC,sizeof(regConfig),1,f);
  fread(&cubo,sizeof(cubo),1,f); 
  
  while (!feof(f)){
	  for (j = 0; j < C; j++) {
      if (j==0) printf("Cubo %2d (%2d reg. ASIGNADOS)",nCubo,cubo.numRegAsignados);
      else if ((j==1) && cubo.desbordado) printf("DESBORDADO\t\t");
      else printf("\t\t\t");
      
      if (j < cubo.numRegAsignados) printf("\t%s %s %s %s %s\n",cubo.reg[j].dni, cubo.reg[j].nombre,cubo.reg[j].ape1,cubo.reg[j].ape2,cubo.reg[j].provincia);
      else printf ("\n");
    }

		nCubo++;
    fread(&cubo,sizeof(cubo),1,f);
  }
  
  fclose(f);
  
  printf("ORGANIZACION METODO DISPERSION:\n\t %d CUBOS y %d CUBOS DESBORDE con capacidad %d\n",regC.nCubos,regC.nCubosDes, C);   
  printf("\t Contiene %d registros de los cuales se han desbordado %d:\n",regC.numReg,regC.numRegDes);
  densidadOcupacion = 100*regC.numReg/((regC.nCubos+regC.nCubosDes)*C);
  printf("Densidad ACTUAL de ocupacion: %d \n\t(MINIMA permitida %d  MAXIMA permitida %d)\n",densidadOcupacion,regC.densidadMin,regC.densidadMax);
  
  if (densidadOcupacion>regC.densidadMax) {
   	printf("No se respeta la densidad maxima de ocupacion\n");
   	return -3;
  }	
  
  if (densidadOcupacion<regC.densidadMin) {
    printf("No se respeta la densidad minima de ocupacion\n");
   	return -4;
  }	
  
  if ((densidadOcupacion > regC.densidadMin) && (densidadOcupacion < regC.densidadMax)) return 0;
  return -1;	
}

int  creaHash(char *fichEntrada, char *fichHash, regConfig *regC){
  FILE *fE, *fH;
  tipoAlumno alumno;
  tipoCubo cubo;
  int densidadOcupacion, indice, i;
  long pos;

  creaHvacio(fichHash, regC);

  if ((fE = fopen(fichEntrada,"rb")) == NULL){
    printf("Error en el fichero de entrada\n");
   	return -1;
  }

  if ((fH = fopen(fichHash,"rb+")) == NULL){
    printf("Error en el fichero de salida\n");
   	return -2;
  }

  rewind(fE);
  fread(&alumno, sizeof(tipoAlumno), 1, fE);
  while (!feof(fE)){
    indice = atoi(alumno.dni) % regC->nCubos;
    pos = sizeof(regConfig) + sizeof(tipoCubo) * indice;

    fseek(fH, pos, SEEK_SET);
    fread(&cubo, sizeof(tipoCubo), 1, fH);
    
    if(cubo.numRegAsignados >= C){
      desborde(fH, &alumno, regC);
      regC->numRegDes++;
      fseek(fH, pos, SEEK_SET);
      fread(&cubo, sizeof(tipoCubo), 1, fH);
      cubo.desbordado = 1;
    } else {
      strcpy(cubo.reg[cubo.numRegAsignados].dni, alumno.dni);
      strcpy(cubo.reg[cubo.numRegAsignados].nombre, alumno.nombre);
      strcpy(cubo.reg[cubo.numRegAsignados].ape1, alumno.ape1);
      strcpy(cubo.reg[cubo.numRegAsignados].ape2, alumno.ape2);
      strcpy(cubo.reg[cubo.numRegAsignados].provincia, alumno.provincia);
      cubo.numRegAsignados++;
    }
    fseek(fH, pos, SEEK_SET);
    fwrite(&cubo, sizeof(tipoCubo), 1, fH);

    regC->numReg++; 
    fread(&alumno, sizeof(tipoAlumno), 1, fE);
  }

  densidadOcupacion = 100 * regC->numReg / ((regC->nCubos + regC->nCubosDes)*C);
  
  fseek(fH, 0, SEEK_SET);
  fwrite(regC, sizeof(regConfig), 1, fH);

  if (densidadOcupacion > regC->densidadMax) {
   	printf("No se respeta la densidad maxima de ocupacion\n");
    fclose(fE);
    fclose(fH);
   	return -3;
  }	
  
  if (densidadOcupacion < regC->densidadMin) {
    printf("No se respeta la densidad minima de ocupacion\n");
    fclose(fE);
    fclose(fH);
   	return -4;
  }	
  
  fclose(fE);
  fclose(fH);
  
  if ((densidadOcupacion > regC->densidadMin) && (densidadOcupacion < regC->densidadMax)) return 0;
  return -5;
}

void creaHvacio(char *fichHash, regConfig *reg){
  FILE *f;
  tipoCubo cubo;
  int i;

  if ((f = fopen(fichHash,"wb")) == NULL){
    printf("Error en el fichero de entrada\n");
   	return;
  }

  reg->numReg = 0;
  reg->numRegDes = 0;
  reg->nCuboDesAct = reg->nCubos;

  fwrite(reg, sizeof(regConfig), 1, f);

  memset(&cubo, 0, sizeof(tipoCubo));
  for (i = 0; i < (reg->nCubos+reg->nCubosDes); i++) fwrite(&cubo, sizeof(tipoCubo), 1, f);

  fclose(f);
}

void desborde(FILE *fHash, tipoAlumno *reg, regConfig *regC){
  tipoCubo cubo;
  long pos;
  
  pos = sizeof(regConfig) + sizeof(tipoCubo) * regC->nCuboDesAct;
  
  fseek(fHash, pos, SEEK_SET);
  fread(&cubo, sizeof(tipoCubo), 1, fHash);

  if (cubo.numRegAsignados >= C){
    regC->nCuboDesAct++;
    if (regC->nCuboDesAct >= (regC->nCubosDes+regC->nCubos)){
      regC->nCubosDes++;
      fseek(fHash, 0, SEEK_END);
      memset(&cubo, 0, sizeof(tipoCubo));
      fwrite(&cubo, sizeof(tipoCubo), 1, fHash);
    }
    pos = sizeof(regConfig) + sizeof(tipoCubo) * regC->nCuboDesAct;
    fseek(fHash, pos, SEEK_SET);
    fread(&cubo, sizeof(tipoCubo), 1, fHash);
  }

  strcpy(cubo.reg[cubo.numRegAsignados].dni, reg->dni);
  strcpy(cubo.reg[cubo.numRegAsignados].nombre, reg->nombre);
  strcpy(cubo.reg[cubo.numRegAsignados].ape1, reg->ape1);
  strcpy(cubo.reg[cubo.numRegAsignados].ape2, reg->ape2);
  strcpy(cubo.reg[cubo.numRegAsignados].provincia, reg->provincia);
  cubo.numRegAsignados++;
  fseek(fHash, pos, SEEK_SET);
  fwrite(&cubo, sizeof(tipoCubo), 1, fHash);
}

tipoAlumno *busquedaHash(FILE *f, char *dni, int *nCubo, int *nCuboDes, int *posReg, int *error){
  tipoCubo cubo;
  regConfig regC;
  tipoAlumno *alumno;
  int indice, i, j;
  long pos;

  if (*error == -2) return NULL;
  
  fseek(f, 0, SEEK_SET);
  fread(&regC, sizeof(regConfig), 1, f);

  indice = atoi(dni) % regC.nCubos;
  pos = sizeof(regConfig) + sizeof(tipoCubo) * indice;
  *error = -4;

  fseek(f, pos, SEEK_SET);
  fread(&cubo, sizeof(tipoCubo), 1, f);

  for (i = 0; i < cubo.numRegAsignados; i++){
    if (strcmp(cubo.reg[i].dni, dni) == 0){
      *nCubo = indice;
      *nCuboDes = -1;
      *posReg = i;
      *error = 0;
      alumno = &cubo.reg[i];
      return alumno;
    }
  }

  if (cubo.desbordado == 1){
    pos = sizeof(regConfig) + sizeof(tipoCubo) * regC.nCubos;
    fseek(f, pos, SEEK_SET);
    fread(&cubo, sizeof(tipoCubo), 1, f);
    j = 0;
    while (!feof(f)){
      for(i = 0; i < cubo.numRegAsignados; i++){
        if(strcmp(cubo.reg[i].dni, dni) == 0){
          *nCubo = indice;
          *nCuboDes = j;
          *posReg = i;
          *error = 0;
          alumno = &cubo.reg[i];
          return alumno;
        }
      }
      fread(&cubo, sizeof(tipoCubo), 1, f);
      j++;
    } 
  }

  *nCuboDes = -1;
  *error = -1;
  return NULL;
}

int modificarReg(char *fichero, char *dni, char *provincia){
  tipoCubo cubo;
  regConfig regC;
  FILE *f;
  tipoAlumno *alumno;
  int nCubo, nCuboDes, posReg, error;
  long pos;

  if ((f = fopen(fichero, "r+b")) == NULL) error = -2;
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

  fseek(f, 0, SEEK_SET);
  fread(&regC, sizeof(regConfig), 1, f);

  if (nCuboDes == -1) pos = sizeof(regConfig) + sizeof(tipoCubo) * nCubo;
  else pos = sizeof(regConfig) + sizeof(tipoCubo) * (regC.nCubos+nCuboDes);

  fseek(f, pos, SEEK_SET);
  fread(&cubo, sizeof(tipoCubo), 1, f);
  printf("\t%s %s %s %s %s\n",cubo.reg[posReg].dni, cubo.reg[posReg].nombre,cubo.reg[posReg].ape1,cubo.reg[posReg].ape2,cubo.reg[posReg].provincia);
  strcpy(cubo.reg[posReg].provincia, provincia);
  fseek(f, pos, SEEK_SET);
  fwrite(&cubo, sizeof(tipoCubo), 1, f);
  printf("\tRegistro actualizado: %s %s %s %s %s\n",cubo.reg[posReg].dni, cubo.reg[posReg].nombre,cubo.reg[posReg].ape1,cubo.reg[posReg].ape2,cubo.reg[posReg].provincia);
      
  fclose(f);

  if (nCuboDes == -1) return nCubo;
  else return nCubo+nCuboDes;
}