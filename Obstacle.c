#include <stdlib.h>
#include <stdio.h>

#include "Obstacle.h"

obstacle *create_obstacle(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y, unsigned short life)
{
  obstacle *newObstacle = (obstacle *)malloc(sizeof(obstacle));
  if (newObstacle == NULL)
  {
    perror("Falha ao alocar memória para o obstáculo");
    exit(1);
  }

  newObstacle->sizeX = sizeX;
  newObstacle->sizeY = sizeY;
  newObstacle->x = x;
  newObstacle->y = y;
  newObstacle->life = life;
  newObstacle->next = NULL;

  return newObstacle;
}

obstacle *create_obstacle_list(unsigned short boardSizeX, unsigned short boardSizeY, unsigned short obstacleSizeX, unsigned short obstacleSizeY, unsigned short spaceX, unsigned short amount)
{
  obstacle *list = NULL;

  // Calcula o espaço disponível para os obstáculos
  int availableSpaceX = boardSizeX - (amount * obstacleSizeX + (amount - 1) * spaceX);

  // Calcula a posição inicial (x, y) do primeiro obstáculo
  int startX = availableSpaceX / 2;

  for (unsigned short i = 0; i < amount; i++)
  {
    int posX = startX + i * (obstacleSizeX + spaceX);

    obstacle *newObstacle = create_obstacle(obstacleSizeX, obstacleSizeY, posX, boardSizeY - 200, OBSTACLE_INITIAL_LIFE);
    newObstacle->next = list;
    list = newObstacle;
  }

  return list;
}

void destroy_obstacle(obstacle *element)
{
  free(element);
}

void destroy_obstacle_list(obstacle *list)
{
  while (list != NULL)
  {
    obstacle *aux = list;
    list = list->next;
    destroy_obstacle(aux);
  }
}