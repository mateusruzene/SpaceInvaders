#include <stdlib.h>
#include "Pistol.h"

pistol *pistol_create()
{ // Implementação da função "pistol_create" (!)

	pistol *new_pistol = (pistol *)malloc(sizeof(pistol)); // Aloca a memória na heap para uma instância de pistola (!)
	if (!new_pistol)
		return NULL;						// Verifica o sucesso da alocação de memória; retorna NULL em caso de falha (!)
	new_pistol->timer = 0;		// Inicializa o relógio de disparos em zero (pronto para atirar) (!)
	new_pistol->shots = NULL; // Inicializa a lista de disparos com NULL; ou seja, sem disparos (!)
	return new_pistol;				// Retorna a nova instância de pistola (!)
}

bullet *pistol_shot(unsigned short x, unsigned short y, unsigned char trajectory, pistol *gun)
{
	if (verify_column_bullet(x, gun->shots))
		return NULL;

	bullet *new_bullet = bullet_create(x, y, trajectory, gun->shots);
	if (!new_bullet)
		return NULL;

	return new_bullet;
}

void pistol_destroy(pistol *element)
{
	bullet *sentinel; // Sentinela para a remoção de projéteis ativos (!)

	// Para cada projétil na lista de disparos (!)
	for (bullet *index = element->shots; index != NULL; index = sentinel)
	{
		sentinel = (bullet *)index->next; // Armazena o próximo projétil (!)
		bullet_destroy(index);						// Chama o destrutor do projétil atual (!)
	}

	free(element); // Libera a memória da instância de pistola (!)
}