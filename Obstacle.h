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
	struct obstacle *next;
} obstacle;

obstacle *create_obstacle(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y, unsigned short life);

obstacle *create_obstacle_list(unsigned short boardSizeX, unsigned short boardSizeY, unsigned short obstacleSizeX, unsigned short obstacleSizeY, unsigned short spaceX, unsigned short amount);

void destroy_obstacle(obstacle *element);

void destroy_obstacle_list(obstacle *list);

#endif