#ifndef __ENEMY__
#define __ENEMY__

typedef enum
{
	ENEMY_WEAK,
	ENEMY_NORMAL,
	ENEMY_STRONG
} EnemyType;

typedef struct obstacle
{
	unsigned short sizeX;
	unsigned short sizeY;
	unsigned short x;
	unsigned short y;
	EnemyType type;
} enemy;

typedef struct
{
	enemy **enemies;
	unsigned short size;
} enemyMatrix;

enemyMatrix *createEnemyMatrix(unsigned short size);

enemy *createEnemy(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y, EnemyType type);

void deleteEnemyMatrix(enemyMatrix *matrix);

void deleteEnemy(enemy *e);

#endif
