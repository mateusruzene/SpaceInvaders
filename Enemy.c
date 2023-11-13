#include <stdlib.h>
#include <stdio.h>

#include "Enemy.h"

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
				matrix->enemies[i][j] = *create_enemy(enemy_width, enemy_height, x, y, ENEMY_STRONG);
			if (i == 1 || i == 2) // Cria os inimigos médios na segunda e terceira linha
				matrix->enemies[i][j] = *create_enemy(enemy_width, enemy_height, x, y, ENEMY_NORMAL);
			if (i == 3 || i == 4) // Cria os inimigos fracos na quarta e quinta linha
				matrix->enemies[i][j] = *create_enemy(enemy_width, enemy_height, x, y, ENEMY_WEAK);
		}
	}

	return matrix;
}

enemy *create_enemy(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y, EnemyType type)
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

	if (type == ENEMY_WEAK)
		newEnemy->points = ENEMY_POINTS_WEAK;
	if (type == ENEMY_NORMAL)
		newEnemy->points = ENEMY_POINTS_NORMAL;
	if (type == ENEMY_STRONG)
		newEnemy->points = ENEMY_POINTS_STRONG;

	return newEnemy;
}

void delete_enemy_matrix(enemyMatrix *matrix)
{
	if (matrix != NULL)
	{
		for (unsigned short i = 0; i < matrix->max_x; i++)
		{
			if (matrix->enemies[i] != NULL)
			{
				// Libera a memória alocada para os inimigos em cada coluna
				for (unsigned short j = 0; j < matrix->max_y; j++)
				{
					if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
					{
						delete_enemy(&matrix->enemies[i][j]);
					}
				}
				free(matrix->enemies[i]);
			}
		}
		free(matrix->enemies);
		free(matrix);
	}
}

void delete_enemy(enemy *e)
{
	if (e != NULL)
	{
		free(e);
		e = NULL;
	}
}

void print_enemy_matrix(enemyMatrix *matrix)
{
	if (matrix == NULL)
	{
		printf("Matrix is NULL\n");
		return;
	}
	printf("TESTE (%d, %d):\n", matrix->max_x, matrix->max_y);

	for (unsigned short i = 0; i < matrix->max_y; i++)
	{
		for (unsigned short j = 0; j < matrix->max_x; j++)
		{
			if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
			{
				printf("Enemy at position (%d, %d):\n", i, j);
				printf("SizeX: %d\n", matrix->enemies[i][j].sizeX);
				printf("SizeY: %d\n", matrix->enemies[i][j].sizeY);
				printf("X: %d\n", matrix->enemies[i][j].x);
				printf("Y: %d\n", matrix->enemies[i][j].y);
				printf("Type: %d\n", matrix->enemies[i][j].type);
				printf("\n");
			}
			else
			{
				printf("No enemy at position (%d, %d)\n", i, j);
			}
		}
	}
}

void move_enemies(enemyMatrix *matrix, unsigned short x_screen, unsigned short enemy_height)
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
}