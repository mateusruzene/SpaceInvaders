#ifndef __ENEMY__
#define __ENEMY__

#define ENEMY_SPEED 1
#define ENEMY_POINTS_WEAK 10
#define ENEMY_POINTS_NORMAL 20
#define ENEMY_POINTS_STRONG 40
#define ENEMY_POINTS_BONUS 100

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

typedef struct obstacle
{
	unsigned short sizeX;
	unsigned short sizeY;
	unsigned short x;
	unsigned short y;
	unsigned short points;
	AliveOrDead aliveOrDead;
	EnemyType type;
} enemy;

typedef struct
{
	enemy **enemies;
	unsigned short max_x;
	unsigned short max_y;
} enemyMatrix;

enemyMatrix *create_enemy_matrix(unsigned short max_x, unsigned short max_y, unsigned short matrix_columns, unsigned short matrix_lines, unsigned short enemy_width, unsigned short enemy_height, unsigned short space_x, unsigned short space_y);

enemy *create_enemy(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y, EnemyType type);

void delete_enemy_matrix(enemyMatrix *matrix);

void delete_enemy(enemy *e);

void print_enemy_matrix(enemyMatrix *matrix);

void move_enemies(enemyMatrix *matrix, unsigned short x_screen, unsigned short enemy_height);

#endif
