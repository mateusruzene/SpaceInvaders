#ifndef __ENEMY__
#define __ENEMY__

#include "Pistol.h"

#define ENEMY_SPEED 1
#define ENEMY_POINTS_WEAK 10
#define ENEMY_POINTS_NORMAL 20
#define ENEMY_POINTS_STRONG 40
#define ENEMY_POINTS_BONUS 100
#define ENEMY_SHOOTS_AMOUNT 2
#define BONUS_ENEMY_SPEED 4

typedef enum
{
	IS_DEAD,
	IS_ALIVE,
} AliveOrDead;

typedef enum
{
	ENEMY_WEAK,
	ENEMY_NORMAL,
	ENEMY_STRONG
} EnemyType;

// Estrutura para representar o inimigo bônus
typedef struct
{
	unsigned short sizeX;
	unsigned short sizeY;
	unsigned short x;
	unsigned short y;
	unsigned short points;
} bonusEnemy;

typedef struct enemy
{
	unsigned short sizeX;
	unsigned short sizeY;
	unsigned short x;
	unsigned short y;
	unsigned short points;
	unsigned short damage;
	AliveOrDead aliveOrDead;
	EnemyType type;
	pistol *gun;
} enemy;

typedef struct
{
	enemy **enemies;
	unsigned short max_x;
	unsigned short max_y;
} enemyMatrix;

enemyMatrix *create_enemy_matrix(unsigned short max_x, unsigned short max_y, unsigned short matrix_columns, unsigned short matrix_lines, unsigned short enemy_width, unsigned short enemy_height, unsigned short space_x, unsigned short space_y);

enemy *create_enemy(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y, EnemyType type, unsigned short damage);

void destroy_enemy_matrix(enemyMatrix *matrix);

void destroy_enemy(enemy *e);

void move_enemies(enemyMatrix *matrix, unsigned short x_screen, unsigned short y_screen);

int has_living_enemy_below(enemyMatrix *matrix, int row, int col);

void enemy_shot(enemy *enemyElement);

void update_enemy_bullets(enemyMatrix *matrix, unsigned short y_screen);

void random_enemy_bullets(enemyMatrix *matrix, int numEnemies);

int any_enemy_alive(enemyMatrix *matrix);

bonusEnemy *create_bonus_enemy(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y);

void destroy_bonus_enemy(bonusEnemy *bonus);

void move_bonus_enemy(bonusEnemy *bonus, unsigned short screenWidth);

#endif
