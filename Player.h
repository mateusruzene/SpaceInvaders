#ifndef __PLAYER__
#define __PLAYER__

#include "Joystick.h" //Estrutura e procedimentos relacionados ao controle do quadrado
#include "Pistol.h"		//Estrutura e procedimentos relacionados ao controle da arma (pistola) no jogo (!)

#define PLAYER_STEP 15				// Tamanho, em pixels, de um passo do jogador
#define PLAYER_INITIAL_LIFE 3 // Quantidade inicial de vidas do jogador

typedef struct
{											 // Definição da estrutura de um quadrado
	unsigned char side;	 // Tamanmho da lateral de um quadrado
	unsigned char face;	 // A face principal do quadrado, algo como a sua "frente" (!)
	unsigned short x;		 // Posição X do centro do quadrado
	unsigned short y;		 // Posição Y do centro do quadrado
	unsigned short life; // Quantidade de vidas do jogador
	joystick *control;	 // Elemento de controle do quadrado no jogo
	pistol *gun;				 // Elemento para realizar disparos no jogo (!)
} player;							 // Definição do nome da estrutura

// Protótipo da função de criação de um quadrado
player *player_create(unsigned char side, unsigned char face, unsigned short x, unsigned short y, unsigned short max_x, unsigned short max_y);

// Protótipo da função de movimentação de um quadrado
void player_move(player *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y);

// Protótipo da função de disparo de um quadrado (!)
void player_shot(player *element);

// Protótipo da função de destruição de um quadrado
void player_destroy(player *element);

#endif