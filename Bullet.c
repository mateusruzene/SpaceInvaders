#include <stdlib.h>
#include <stdio.h>
#include "Bullet.h"

int verify_column_bullet(unsigned short x, bullet *elements)
{
	for (bullet *index = elements; index != NULL; index = (bullet *)index->next)
		if (x == index->x)
			return 1;

	return 0;
}

bullet *bullet_create(unsigned short x, unsigned short y, unsigned char trajectory, bullet *next)
{
	if ((trajectory < 0) || (trajectory > 1))
		return NULL; // Verifica se a trajetória informada para o projétil é válida

	bullet *new_bullet = (bullet *)malloc(sizeof(bullet)); // Aloca memória na heap para uma instância de projétil
	if (!new_bullet)
		return NULL;														// Verifica o sucesso da alocação de memória; retorna NULL em caso de falha
	new_bullet->x = x;												// Armazena a posição X informada
	new_bullet->y = y;												// Armazena a posição Y informada
	new_bullet->trajectory = trajectory;			// Armazena a trajetória informada
	new_bullet->next = (struct bullet *)next; // Armazena o próximo projétil informado
	return new_bullet;												// Retorna a instância criada de projétil
}

void bullet_move(bullet *elements)
{
	for (bullet *index = elements; index != NULL; index = (bullet *)index->next)
	{ // Para cada projétil presente na lista de projéteis informada
		if (!index->trajectory)
			index->y = index->y - BULLET_MOVE; // Se a trajetória for para a cima, movimenta um passo à cima
		else
			index->y = index->y + BULLET_MOVE; // Se a trajetória for para a baixo, movimenta um passo à baixo
	}
}

void bullet_destroy(bullet *element)
{
	free(element);
}