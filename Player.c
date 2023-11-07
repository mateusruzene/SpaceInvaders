#include <stdlib.h>
#include "Player.h"

player *player_create(unsigned char side, unsigned char face, unsigned short x, unsigned short y, unsigned short max_x, unsigned short max_y)
{ // Implementação da função "player_create"

	if ((x - side / 2 < 0) || (x + side / 2 > max_x) || (y - side / 2 < 0) || (y + side / 2 > max_y))
		return NULL; // Verifica se a posição inicial é válida; caso não seja, retorna NULL
	if (face > 3)
		return NULL; // Verifica se a face principal do quadrado é válida (!)

	player *new_player = (player *)malloc(sizeof(player)); // Aloca memória na heap para um novo quadrado
	if (!new_player)
		return NULL;													 // Se a alocação não deu certo, retorna erro
	new_player->side = side;								 // Insere o tamanho do lado de um quadrado
	new_player->face = face;								 // Insere a indicação da face principal do quadrado (!)
	new_player->x = x;											 // Insere a posição inicial central de X
	new_player->y = y;											 // Insere a posição inicial central de Y
	new_player->control = joystick_create(); // Insere o elemento de controle do quadrado
	new_player->gun = pistol_create();			 // Insere o elemento de disparos do quadrado (!)
	new_player->life = PLAYER_INITIAL_LIFE;	 // Insere a quantidade de vidas do jogador (!)
	return new_player;											 // Retorna o novo quadrado
}

void player_move(player *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y)
{ // Implementação da função "player_move"

	if (!trajectory)
	{
		if ((element->x - steps * PLAYER_STEP) - element->side / 2 >= 0)
			element->x = element->x - steps * PLAYER_STEP;
	} // Verifica se a movimentação para a esquerda é desejada e possível; se sim, efetiva a mesma
	else if (trajectory == 1)
	{
		if ((element->x + steps * PLAYER_STEP) + element->side / 2 <= max_x)
			element->x = element->x + steps * PLAYER_STEP;
	} // Verifica se a movimentação para a direita é desejada e possível; se sim, efetiva a mesma
}

void player_shot(player *element)
{ // Implementação da função "player_shot" (!)
	bullet *shot;

	if (!element->face)
		shot = pistol_shot(element->x, element->y - element->side / 2, element->face, element->gun); // Quadrado atira para a esquerda (!)
	else if (element->face == 1)
		shot = pistol_shot(element->x, element->y - element->side / 2, element->face, element->gun); // Quadrado atira para a esquerda (!)
	if (shot)
		element->gun->shots = shot;
}

void player_destroy(player *element)
{																			// Implementação da função "player_destroy"
	pistol_destroy(element->gun);				// Destrói o armemento do quadrado (!)
	joystick_destroy(element->control); // Destrói o controle do quadrado
	free(element);											// Libera a memória do quadrado na heap
}