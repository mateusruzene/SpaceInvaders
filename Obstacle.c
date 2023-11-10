#include <stdlib.h>
#include <stdio.h>

#include "Obstacle.h"

obstacle *createObstacle(unsigned short sizeX, unsigned short sizeY, unsigned short x, unsigned short y, unsigned short life, unsigned short id)
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
  newObstacle->id = id;
  newObstacle->next = NULL;

  return newObstacle;
}

obstacle *createObstacleList(unsigned short boardSizeX, unsigned short boardSizeY, unsigned short obstacleSizeX, unsigned short obstacleSizeY)
{
  obstacle *list = NULL;

  // Adicione obstáculos de acordo com o tamanho do tabuleiro
  for (unsigned short x = 0; x < boardSizeX; x += obstacleSizeX)
  {
    for (unsigned short y = 0; y < boardSizeY; y += obstacleSizeY)
    {
      obstacle *newObstacle = createObstacle(obstacleSizeX, obstacleSizeY, x, y, 100, list ? list->id + 1 : 1);
      newObstacle->next = list;
      list = newObstacle;
    }
  }

  return list;
}

void destroyObstacle(obstacle *element)
{
  free(element);
}

void destroyObstacleList(obstacle *list)
{
  while (list != NULL)
  {
    obstacle *aux = list;
    list = list->next;
    destroyObstacle(aux);
  }
}