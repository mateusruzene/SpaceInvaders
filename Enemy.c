#include <stdlib.h>

#include "Enemy.h"

enemyMatrix *createEnemyMatrix(unsigned short size)
{
	enemyMatrix *matrix = (enemyMatrix *)malloc(sizeof(enemyMatrix));
	if (matrix == NULL)
	{
		return NULL;
	}

	matrix->size = size;
	matrix->enemies = (enemy **)malloc(size * sizeof(enemy *));
	if (matrix->enemies == NULL)
	{
		free(matrix);
		return NULL;
	}

	for (unsigned short i = 0; i < size; i++)
	{
		matrix->enemies[i] = NULL;
	}

	return matrix;
}

enemy *createEnemy(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y, EnemyType type)
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

	return newEnemy;
}

void deleteEnemyMatrix(enemyMatrix *matrix)
{
	if (matrix != NULL)
	{
		for (unsigned short i = 0; i < matrix->size; i++)
		{
			if (matrix->enemies[i] != NULL)
			{
				free(matrix->enemies[i]);
			}
		}
		free(matrix->enemies);
		free(matrix);
	}
}

void deleteEnemy(enemy *e)
{
	if (e != NULL)
	{
		free(e);
	}
}
