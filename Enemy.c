#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Enemy.h"
#include "Pistol.h"

enemyMatrix *create_enemy_matrix(unsigned short max_x, unsigned short max_y, unsigned short matrix_columns, unsigned short matrix_lines, unsigned short enemy_width, unsigned short enemy_height, unsigned short space_x, unsigned short space_y)
{
	enemyMatrix *matrix = (enemyMatrix *)malloc(sizeof(enemyMatrix));
	if (matrix == NULL)
	{
		return NULL;
	}

	matrix->max_x = matrix_columns;
	matrix->max_y = matrix_lines;

	// Calcula o espaço disponível para os inimigos
	int espaco_disponivel_x = max_x - (matrix_columns * enemy_width + (matrix_columns - 1) * space_x);
	int espaco_disponivel_y = max_y - (matrix_lines * enemy_height + (matrix_lines - 1) * space_y);

	// Calcula a posição inicial (x, y) do primeiro inimigo
	int start_x = espaco_disponivel_x / 4;
	int start_y = espaco_disponivel_y / 4;

	matrix->enemies = (enemy **)malloc(max_x * sizeof(enemy *));
	if (matrix->enemies == NULL)
	{
		free(matrix);
		return NULL;
	}

	for (unsigned short i = 0; i < matrix_lines; i++)
	{
		matrix->enemies[i] = (enemy *)malloc(max_y * sizeof(enemy));
		if (matrix->enemies[i] == NULL)
		{
			// Se a alocação falhar, libera a memória já alocada e retorna NULL
			for (unsigned short j = 0; j < i; j++)
			{
				free(matrix->enemies[j]);
			}
			free(matrix->enemies);
			free(matrix);
			return NULL;
		}

		for (unsigned short j = 0; j < matrix_columns; j++)
		{
			int x = start_x + j * (enemy_width + space_x);
			int y = start_y + i * (enemy_height + space_y);

			if (i == 0) // Cria os inimigos fortes na primeira linha
				matrix->enemies[i][j] = *create_enemy(enemy_width, enemy_height, x, y, ENEMY_STRONG, 2);
			if (i == 1 || i == 2) // Cria os inimigos médios na segunda e terceira linha
				matrix->enemies[i][j] = *create_enemy(enemy_width, enemy_height, x, y, ENEMY_NORMAL, 2);
			if (i == 3 || i == 4) // Cria os inimigos fracos na quarta e quinta linha
				matrix->enemies[i][j] = *create_enemy(enemy_width, enemy_height, x, y, ENEMY_WEAK, 1);
		}
	}

	return matrix;
}

enemy *create_enemy(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y, EnemyType type, unsigned short damage)
{
	enemy *newEnemy = (enemy *)malloc(sizeof(enemy));
	if (newEnemy == NULL)
	{
		return NULL;
	}

	newEnemy->sizeX = sizeX;
	newEnemy->sizeY = sizeY;
	newEnemy->x = x;
	newEnemy->y = y;
	newEnemy->type = type;
	newEnemy->aliveOrDead = IS_ALIVE;
	newEnemy->damage = damage;
	newEnemy->gun = pistol_create();

	if (type == ENEMY_WEAK)
		newEnemy->points = ENEMY_POINTS_WEAK;
	if (type == ENEMY_NORMAL)
		newEnemy->points = ENEMY_POINTS_NORMAL;
	if (type == ENEMY_STRONG)
		newEnemy->points = ENEMY_POINTS_STRONG;

	return newEnemy;
}

// void destroy_enemy_matrix(enemyMatrix *matrix)
// {
// 	if (matrix != NULL)
// 	{
// 		for (unsigned short i = matrix->max_x - 1; i >= 0; i--)
// 		{
// 			if (matrix->enemies[i] != NULL)
// 			{
// 				// Libera a memória alocada para os inimigos em cada coluna
// 				for (unsigned short j = matrix->max_y - 1; j >= 0; j--)
// 				{
// 					destroy_enemy(&matrix->enemies[i][j]);
// 				}
// 				free(matrix->enemies[i]);
// 			}
// 		}
// 		free(matrix->enemies);
// 		free(matrix);
// 	}
// }

void destroy_enemy_matrix(enemyMatrix *matrix)
{
	if (matrix != NULL)
	{
		for (unsigned short i = 0; i < matrix->max_x - 1; i++) // Correção aqui
		{
			if (matrix->enemies[i] != NULL)
			{
				// Libera a memória alocada para os inimigos em cada coluna
				for (unsigned short j = matrix->max_y - 1; j >= 0; j--)
				{
					destroy_enemy(&matrix->enemies[i][j]);
				}
				free(matrix->enemies[i]);
				matrix->enemies[i] = NULL;
			}
		}
		free(matrix->enemies);
		free(matrix);
		matrix = NULL;
	}
}

void destroy_enemy(enemy *e)
{
	if (e != NULL)
	{
		if (e->gun != NULL)
			pistol_destroy(e->gun);

		free(e);
	}
}

void move_enemies(enemyMatrix *matrix, unsigned short x_screen, unsigned short y_screen)
{
	static int direction = 1; // 1 para direita, -1 para esquerda

	for (unsigned short i = 0; i < matrix->max_y; i++)
	{
		for (unsigned short j = 0; j < matrix->max_x; j++)
		{
			if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
			{
				matrix->enemies[i][j].x += direction * ENEMY_SPEED;
				// Verifica se atingiu a borda direita ou esquerda
				if (matrix->enemies[i][j].x < 0 || (matrix->enemies[i][j].x + matrix->enemies[i][j].sizeX) > x_screen)
					direction *= -1;
			}
		}
	}

	update_enemy_bullets(matrix, y_screen);
}

int has_living_enemy_below(enemyMatrix *matrix, int row, int col)
{
	for (int i = row + 1; i < matrix->max_y; i++)
	{
		if (matrix->enemies[i][col].aliveOrDead == IS_ALIVE)
		{
			return 1; // Encontrou um inimigo vivo abaixo
		}
	}
	return 0; // Nenhum inimigo vivo abaixo
};

void enemy_shot(enemy *enemyElement)
{
	bullet *shot = pistol_shot(enemyElement->x + enemyElement->sizeX / 2, enemyElement->y + enemyElement->sizeY, 1, enemyElement->gun);

	if (shot)
		enemyElement->gun->shots = shot;
};

void update_enemy_bullets(enemyMatrix *matrix, unsigned short y_screen)
{
	for (unsigned short i = 0; i < matrix->max_y; i++)
	{
		for (unsigned short j = 0; j < matrix->max_x; j++)
		{
			if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
			{
				bullet *previous = NULL;
				for (bullet *index = matrix->enemies[i][j].gun->shots; index != NULL;)
				{
					index->y += BULLET_MOVE; // Atualiza a posição para baixo

					if ((index->y < 0) || (index->y > y_screen))
					{
						if (previous)
						{
							previous->next = index->next;
							bullet_destroy(index);
							index = (bullet *)previous->next;
						}
						else
						{
							matrix->enemies[i][j].gun->shots = (bullet *)index->next;
							bullet_destroy(index);
							index = matrix->enemies[i][j].gun->shots;
						}
					}
					else
					{
						previous = index;
						index = (bullet *)index->next;
					}
				}
			}
		}
	}
}

// Função para escolher inimigos aleatórios vivos para atirarem contra o jogador
void random_enemy_bullets(enemyMatrix *matrix, int numEnemies)
{
	srand(time(NULL));

	int livingEnemies = 0;

	// Verifica se há algum projétil ativo em algum inimigo
	int projectilesExist = 0;
	for (unsigned short i = 0; i < matrix->max_y; i++)
	{
		for (unsigned short j = 0; j < matrix->max_x; j++)
		{
			if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE && matrix->enemies[i][j].gun->shots != NULL)
			{
				projectilesExist = 1;
				break;
			}
		}
		if (projectilesExist)
			break;
	}

	// Tentar escolher inimigos vivos até atingir o número desejado
	while (livingEnemies < numEnemies && !projectilesExist)
	{
		int row = rand() % matrix->max_y;
		int col = rand() % matrix->max_x;

		if (matrix->enemies[row][col].aliveOrDead == IS_ALIVE)
		{
			// Verifica o tipo de inimigo e adiciona à lista correspondente
			switch (matrix->enemies[row][col].type)
			{
			case ENEMY_WEAK:
				// Verifica se não há inimigo abaixo antes de atirar
				if (row == matrix->max_y - 1 || matrix->enemies[row + 1][col].aliveOrDead == IS_DEAD)
				{
					enemy_shot(&matrix->enemies[row][col]);
					livingEnemies++;
				}
				break;
			case ENEMY_NORMAL:
				// Verifica se não há inimigo abaixo antes de atirar
				if (row == matrix->max_y - 1 || matrix->enemies[row + 1][col].aliveOrDead == IS_DEAD)
				{
					enemy_shot(&matrix->enemies[row][col]);
					livingEnemies++;
				}
				break;
			case ENEMY_STRONG:
				enemy_shot(&matrix->enemies[row][col]);
				livingEnemies++;
				break;
			}
		}
	}
}

int any_enemy_alive(enemyMatrix *matrix)
{
	for (unsigned short i = 0; i < matrix->max_y; i++)
	{
		for (unsigned short j = 0; j < matrix->max_x; j++)
		{
			if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
			{
				return 1;
			}
		}
	}
	return 0;
}

bonusEnemy *create_bonus_enemy(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y)
{
	bonusEnemy *bonus = (bonusEnemy *)malloc(sizeof(bonusEnemy));
	if (!bonus)
	{
		fprintf(stderr, "Falha ao alocar memória para o inimigo bônus.\n");
		exit(EXIT_FAILURE);
	}
	bonus->x = x;
	bonus->y = y;
	bonus->sizeX = sizeX;
	bonus->sizeY = sizeY;
	bonus->points = ENEMY_POINTS_BONUS;
	return bonus;
}

void destroy_bonus_enemy(bonusEnemy *bonus)
{
	free(bonus);
	bonus = NULL;
}

void move_bonus_enemy(bonusEnemy *bonus, unsigned short screenWidth)
{
	// Modifique a posição do bônus para movê-lo
	bonus->x += BONUS_ENEMY_SPEED;

	// Verifique se o bônus atingiu as bordas da tela
	if (bonus->x > screenWidth || bonus->x < 0)
	{
		// Se atingiu, reposicione o bônus na borda oposta
		bonus->x = (bonus->x > screenWidth) ? 0 : screenWidth;
	}
}