/*
 * conexionBinario.c
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#include<stdio.h>
#include "conexionBinario.h";

void guardarDatos(t_bitarray* bitarray){
    FILE *fout = fopen("Metadata/bitmap.bin", "wb+");
    fwrite(bitarray->bitarray, bitarray->size, 1, fout);
    fclose(fout);
}

t_bitarray* obtenerDatos(int tamanio){
	FILE *fin = fopen("Metadata/bitmap.bin", "rb");
	void* buffer = malloc(tamanio);
	fread(buffer, 1, tamanio, fin);
	fclose(fin);
	return bitarray_create_with_mode(buffer, tamanio, LSB_FIRST);
}

uint32_t obtenerBloqueLibre(uint32_t blockCantBytes){
	t_bitarray *bitmap = obtenerDatos(blockCantBytes);
	uint32_t valor;
	for(uint32_t i = 0; i < 5192/8; i++){
		valor = bitarray_test_bit(bitmap, i);
		if(valor == 0){
			bitarray_set_bit(bitmap, i);
			guardarDatos(bitmap);
			return (i+1);
		}
	}
}

void limpiarBloque(uint32_t posicion, uint32_t blockCantBytes){
	t_bitarray *bitmap = obtenerDatos(blockCantBytes);
	bitarray_clean_bit(bitmap, posicion-1);
	guardarDatos(bitmap);
}

