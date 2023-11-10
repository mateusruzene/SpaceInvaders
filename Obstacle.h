#ifndef __OBSTACLE__
#define __OBSTACLE__

#define OBSTACLE_INITIAL_LIFE 10 // Quantidade inicial de vidas do jogador

typedef struct obstacle
{
	unsigned short sizeX;
	unsigned short sizeY;
	unsigned short x;
	unsigned short y;
	unsigned short life;
	unsigned short id;
	struct obstacle *next;
} obstacle;

obstacle *createObstacle(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y, unsigned short life, unsigned short id);

obstacle *createObstacleList(unsigned short boardSizeX, unsigned short boardSizeY, unsigned short obstacleSizeX, unsigned short obstacleSizeY);

void destroyObstacle(obstacle *element);

void destroyObstacleList(obstacle *list);

#endif