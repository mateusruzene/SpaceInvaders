#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <allegro5/allegro5.h>					 //Biblioteca base do Allegro
#include <allegro5/allegro_font.h>			 //Biblioteca de fontes do Allegro
#include <allegro5/allegro_ttf.h>				 //Biblioteca de fontes do Allegro
#include <allegro5/allegro_primitives.h> //Biblioteca de figuras básicas
#include <allegro5/allegro_image.h>			 //Biblioteca de imagens

#include "Player.h"		//Inclusão da biblioteca do jogador
#include "Enemy.h"		//Inclusão da biblioteca dos inimigos
#include "Obstacle.h" //Inclusão da biblioteca dos obstaculos

#define X_SCREEN 700 // Definição do tamanho da tela em pixels no eixo x
#define Y_SCREEN 600 // Definição do tamanho da tela em pixels no eixo y

// Definições do Jogador
#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 32
#define BULLET_WIDTH 2
#define BULLET_HEIGHT 10

// Definições dos inimigos
#define ENEMY_WIDTH 23	 // Largura do inimigo
#define ENEMY_HEIGHT 23	 // Altura do inimigo
#define SPACE_X 15			 // Espaço entre os inimigos X
#define SPACE_Y 15			 // Espaço entre os inimigos Y
#define MATRIX_LINES 5	 // Número de linhas
#define MATRIX_COLUMNS 8 // Número de colunas

// Definições dos obstaculos
#define OBSTACLE_WIDTH 78		 // Largura do obstaculo
#define OBSTACLE_HEIGHT 78	 // Altura do obstaculo
#define OBSTACLE_AMOUNT 4		 // Número de obstaculos
#define OBSTACLE_SPACE_X 100 // Espaço entre os obstaculos X

// ================================== FUNÇÔES DE UPDATE DO PLAYER ==================================

void update_bullets(player *player)
{
	bullet *previous = NULL;
	for (bullet *index = player->gun->shots; index != NULL;)
	{
		if (!index->trajectory)
			index->y -= BULLET_MOVE;
		else if (index->trajectory == 1)
			index->y -= BULLET_MOVE;

		if ((index->y < 0) || (index->y > Y_SCREEN))
		{
			if (previous)
			{
				previous->next = index->next;
				bullet_destroy(index);
				index = (bullet *)previous->next;
			}
			else
			{
				player->gun->shots = (bullet *)index->next;
				bullet_destroy(index);
				index = player->gun->shots;
			}
		}
		else
		{
			previous = index;
			index = (bullet *)index->next;
		}
	}
}

void update_position(player *player_1)
{
	if (player_1->control->left)
		player_move(player_1, 1, 0, X_SCREEN, Y_SCREEN);

	if (player_1->control->right)
		player_move(player_1, 1, 1, X_SCREEN, Y_SCREEN);

	if (player_1->control->fire)
	{
		if (!player_1->gun->timer)
		{
			player_shot(player_1);
			player_1->gun->timer = PISTOL_COOLDOWN;
		}
	}

	update_bullets(player_1);
}

// ================================== FUNÇÔES DE CHECK COLLISION ==================================

int check_player_bonus_collision(player *player, bonusEnemy *bonus, ALLEGRO_BITMAP *sprite)
{
	bullet *previous_bullet = NULL;
	bullet *bullet_index = player->gun->shots;
	int is_dead = 0;

	while (bullet_index != NULL)
	{
		// Verifica colisão entre o tiro e o bônus
		if ((bullet_index->x < bonus->x + bonus->sizeX) &&
				(bullet_index->x + BULLET_WIDTH > bonus->x) &&
				(bullet_index->y < bonus->y + bonus->sizeY) &&
				(bullet_index->y + BULLET_HEIGHT > bonus->y))
		{
			player->points += bonus->points;

			al_draw_bitmap_region(sprite, 118, 200, ENEMY_WIDTH, ENEMY_HEIGHT, bonus->x, bonus->y, 0);
			destroy_bonus_enemy(bonus);
			is_dead = 1;

			if (previous_bullet)
			{
				previous_bullet->next = bullet_index->next;
				bullet_destroy(bullet_index);
				bullet_index = (bullet *)previous_bullet->next;
			}
			else
			{
				player->gun->shots = (bullet *)bullet_index->next;
				bullet_destroy(bullet_index);
				bullet_index = player->gun->shots;
			}
		}
		else
		{
			previous_bullet = bullet_index;
			bullet_index = (bullet *)bullet_index->next;
		}
	}

	return is_dead;
}

// Função para verificar colisões entre tiros e inimigos
void check_player_enemy_collision(player *player, enemyMatrix *matrix, ALLEGRO_BITMAP *sprite)
{
	bullet *previous_bullet = NULL;
	bullet *bullet_index = player->gun->shots;

	while (bullet_index != NULL)
	{
		int bullet_hit = 0;

		for (unsigned short i = 0; i < matrix->max_y; i++)
		{
			for (unsigned short j = 0; j < matrix->max_x; j++)
			{
				if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
				{
					// Verifica se há colisão entre o tiro e o inimigo
					if (bullet_index->x >= matrix->enemies[i][j].x &&
							bullet_index->x <= matrix->enemies[i][j].x + ENEMY_WIDTH &&
							bullet_index->y >= matrix->enemies[i][j].y &&
							bullet_index->y <= matrix->enemies[i][j].y + ENEMY_HEIGHT)
					{
						matrix->enemies[i][j].aliveOrDead = IS_DEAD;
						al_draw_bitmap_region(sprite, 176, 144, ENEMY_WIDTH, ENEMY_HEIGHT, matrix->enemies[i][j].x, matrix->enemies[i][j].y, 0);

						bullet_hit = 1;
						player->points += matrix->enemies[i][j].points;
						break;
					}
				}
			}

			if (bullet_hit)
				break;
		}

		// Remove o tiro da lista se houver colisão
		if (bullet_hit)
		{
			if (previous_bullet)
			{
				previous_bullet->next = bullet_index->next;
				bullet_destroy(bullet_index);
				bullet_index = (bullet *)previous_bullet->next;
			}
			else
			{
				player->gun->shots = (bullet *)bullet_index->next;
				bullet_destroy(bullet_index);
				bullet_index = player->gun->shots;
			}
		}
		else
		{
			// Move para o próximo tiro
			previous_bullet = bullet_index;
			bullet_index = (bullet *)bullet_index->next;
		}
	}
}

void check_player_obstacle_collision(player *player, obstacle *obstacle_list, ALLEGRO_BITMAP *sprite)
{
	bullet *previous_bullet = NULL;
	bullet *bullet_index = player->gun->shots;

	while (bullet_index != NULL)
	{
		int bullet_hit = 0;

		for (obstacle *current_obstacle = obstacle_list; current_obstacle != NULL; current_obstacle = current_obstacle->next)
		{
			if (current_obstacle->life > 0)
			{
				// Verifica se há colisão entre o tiro e o obstáculo
				if (bullet_index->x >= current_obstacle->x &&
						bullet_index->x <= current_obstacle->x + OBSTACLE_WIDTH &&
						bullet_index->y >= current_obstacle->y &&
						bullet_index->y <= current_obstacle->y + OBSTACLE_HEIGHT)
				{
					// Reduz a vida do obstáculo
					current_obstacle->life--;

					if (current_obstacle->life <= 0)
						al_draw_bitmap_region(sprite, 397, 202, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, current_obstacle->x, current_obstacle->y, 0);

					// Remove o tiro da lista se houver colisão
					if (previous_bullet)
					{
						previous_bullet->next = bullet_index->next;
						bullet_destroy(bullet_index);
						bullet_index = (bullet *)previous_bullet->next;
					}
					else
					{
						player->gun->shots = (bullet *)bullet_index->next;
						bullet_destroy(bullet_index);
						bullet_index = player->gun->shots;
					}

					bullet_hit = 1;
					break;
				}
			}
		}

		// Move para o próximo tiro
		if (!bullet_hit)
		{
			previous_bullet = bullet_index;
			bullet_index = (bullet *)bullet_index->next;
		}
	}
}

void check_enemy_obstacle_collision(enemyMatrix *matrix, obstacle *obstacle_list, ALLEGRO_BITMAP *sprite)
{
	for (unsigned short i = 0; i < matrix->max_y; i++)
	{
		for (unsigned short j = 0; j < matrix->max_x; j++)
		{
			if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
			{
				bullet *previous_bullet = NULL;
				bullet *bullet_index = matrix->enemies[i][j].gun->shots;

				while (bullet_index != NULL)
				{
					int bullet_hit = 0;

					for (obstacle *current_obstacle = obstacle_list; current_obstacle != NULL; current_obstacle = current_obstacle->next)
					{
						if (current_obstacle->life > 0)
						{
							// Verifica se há colisão entre o tiro do inimigo e o obstáculo
							if (bullet_index->x >= current_obstacle->x &&
									bullet_index->x <= current_obstacle->x + OBSTACLE_WIDTH &&
									bullet_index->y >= current_obstacle->y &&
									bullet_index->y <= current_obstacle->y + OBSTACLE_HEIGHT)
							{
								// Reduz a vida do obstáculo
								current_obstacle->life -= matrix->enemies[i][j].damage;

								if (current_obstacle->life <= 0)
									al_draw_bitmap_region(sprite, 397, 202, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, current_obstacle->x, current_obstacle->y, 0);

								// Remove o tiro do inimigo da lista
								if (previous_bullet)
								{
									previous_bullet->next = bullet_index->next;
									bullet_destroy(bullet_index);
									bullet_index = (bullet *)previous_bullet->next;
								}
								else
								{
									matrix->enemies[i][j].gun->shots = (bullet *)bullet_index->next;
									bullet_destroy(bullet_index);
									bullet_index = matrix->enemies[i][j].gun->shots;
								}

								bullet_hit = 1;
								break;
							}
						}
					}

					// Move para o próximo tiro
					if (!bullet_hit)
					{
						previous_bullet = bullet_index;
						bullet_index = (bullet *)bullet_index->next;
					}
				}
			}
		}
	}
}

int check_enemy_player_collision(player *player, enemyMatrix *matrix)
{
	int was_shot = 0;

	for (unsigned short i = 0; i < matrix->max_y; i++)
	{
		for (unsigned short j = 0; j < matrix->max_x; j++)
		{
			if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
			{
				for (bullet *index = matrix->enemies[i][j].gun->shots; index != NULL; index = (bullet *)index->next)
				{
					// Verifica se há colisão entre o tiro do inimigo e o jogador
					if (index->x >= player->x - PLAYER_WIDTH / 2 &&
							index->x <= player->x + PLAYER_WIDTH / 2 &&
							index->y >= player->y - PLAYER_HEIGHT / 2 &&
							index->y <= player->y + PLAYER_HEIGHT / 2)
					{

						// O jogador foi atingido, faça as ações necessárias, por exemplo, decrementar vidas
						player->life--;
						was_shot = 1;
						// Remove o tiro do inimigo da lista
						bullet *previous = NULL;
						bullet *bullet_index = matrix->enemies[i][j].gun->shots;

						while (bullet_index != NULL)
						{
							if (bullet_index == index)
							{
								if (previous)
								{
									previous->next = bullet_index->next;
									bullet_destroy(bullet_index);
									index = (bullet *)previous->next;
								}
								else
								{
									matrix->enemies[i][j].gun->shots = (bullet *)bullet_index->next;
									bullet_destroy(bullet_index);
									index = matrix->enemies[i][j].gun->shots;
								}
								break;
							}
							previous = bullet_index;
							bullet_index = (bullet *)bullet_index->next;
						}

						break; // Se houve colisão, passa para o próximo tiro do inimigo
					}
				}
			}
		}
	}

	return was_shot;
}

// Função para verificar colisões entre tiros do jogador e tiros dos inimigos
void check_bullet_collision(player *player, enemyMatrix *matrix)
{
	bullet *player_bullet_prev = NULL;
	bullet *player_bullet_index = player->gun->shots;

	while (player_bullet_index != NULL)
	{
		int player_bullet_hit = 0;

		// Verifica colisão com tiros dos inimigos
		for (unsigned short i = 0; i < matrix->max_y; i++)
		{
			for (unsigned short j = 0; j < matrix->max_x; j++)
			{
				if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
				{
					bullet *enemy_bullet_prev = NULL;
					bullet *enemy_bullet_index = matrix->enemies[i][j].gun->shots;

					while (enemy_bullet_index != NULL)
					{
						// Verifica se há colisão entre o tiro do jogador e o tiro do inimigo
						if (player_bullet_index->x >= enemy_bullet_index->x &&
								player_bullet_index->x <= enemy_bullet_index->x + 2 &&
								player_bullet_index->y >= enemy_bullet_index->y &&
								player_bullet_index->y <= enemy_bullet_index->y + 10)
						{
							// Remove os tiros da lista
							if (player_bullet_prev)
							{
								player_bullet_prev->next = player_bullet_index->next;
								bullet_destroy(player_bullet_index);
								player_bullet_index = (bullet *)player_bullet_prev->next;
							}
							else
							{
								player->gun->shots = (bullet *)player_bullet_index->next;
								bullet_destroy(player_bullet_index);
								player_bullet_index = player->gun->shots;
							}

							if (enemy_bullet_prev)
							{
								enemy_bullet_prev->next = enemy_bullet_index->next;
								bullet_destroy(enemy_bullet_index);
								enemy_bullet_index = (bullet *)enemy_bullet_prev->next;
							}
							else
							{
								matrix->enemies[i][j].gun->shots = (bullet *)enemy_bullet_index->next;
								bullet_destroy(enemy_bullet_index);
								enemy_bullet_index = matrix->enemies[i][j].gun->shots;
							}

							player_bullet_hit = 1;
							break;
						}

						enemy_bullet_prev = enemy_bullet_index;
						enemy_bullet_index = (bullet *)enemy_bullet_index->next;
					}
				}

				if (player_bullet_hit)
					break;
			}

			if (player_bullet_hit)
				break;
		}

		// Move para o próximo tiro do jogador
		if (!player_bullet_hit)
		{
			player_bullet_prev = player_bullet_index;
			player_bullet_index = (bullet *)player_bullet_index->next;
		}
	}
}

// ================================== FUNÇÔES DE DESENHAR NA TELA ==================================
void draw_player(player *e, ALLEGRO_BITMAP *sprite, int was_shot)
{
	if (was_shot == 1)
	{
		al_draw_bitmap_region(sprite, 128, 238, PLAYER_WIDTH, PLAYER_HEIGHT, e->x - e->side / 2, e->y, 0);
		al_draw_bitmap_region(sprite, 91, 274, PLAYER_WIDTH, PLAYER_HEIGHT, e->x - e->side / 2, e->y, 0);
	}
	else
		al_draw_bitmap_region(sprite, 91, 238, PLAYER_WIDTH, PLAYER_HEIGHT, e->x - e->side / 2, e->y, 0);
}

void draw_obstacle(obstacle *o, ALLEGRO_BITMAP *sprite)
{
	if (o->life >= 9)
		al_draw_bitmap_region(sprite, 229, 118, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, o->x, o->y, 0);
	else if (o->life < 9 && o->life >= 7)
		al_draw_bitmap_region(sprite, 312, 118, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, o->x, o->y, 0);
	else if (o->life < 7 && o->life >= 5)
		al_draw_bitmap_region(sprite, 397, 118, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, o->x, o->y, 0);
	else if (o->life < 5 && o->life >= 3)
		al_draw_bitmap_region(sprite, 229, 202, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, o->x, o->y, 0);
	else if (o->life < 3 && o->life >= 1)
		al_draw_bitmap_region(sprite, 313, 202, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, o->x, o->y, 0);
}

void draw_enemy(enemy *e, ALLEGRO_BITMAP *sprite, int frame)
{
	int spriteX, spriteY;
	// Determina as coordenadas da região do sprite correspondente ao tipo de inimigo
	switch (e->type)
	{
	case ENEMY_WEAK:
		if (frame == 1)
		{
			spriteX = 90;
			spriteY = 115;
		}
		else
		{
			spriteX = 118;
			spriteY = 115;
		}
		break;
	case ENEMY_NORMAL:
		if (frame == 1)
		{
			spriteX = 90;
			spriteY = 144;
		}
		else
		{
			spriteX = 118;
			spriteY = 144;
		}
		break;
	case ENEMY_STRONG:
		if (frame == 1)
		{
			spriteX = 90;
			spriteY = 172;
		}
		else
		{
			spriteX = 118;
			spriteY = 172;
		}
		break;
	}
	// Desenha a região correspondente do sprite na posição do inimigo
	al_draw_bitmap_region(sprite, spriteX, spriteY, ENEMY_WIDTH, ENEMY_HEIGHT, e->x, e->y, 0);
}

void draw_enemy_bullets(enemyMatrix *matrix, ALLEGRO_BITMAP *sprite, int frame)
{
	int spriteX, spriteY, height, width;

	for (unsigned short i = 0; i < matrix->max_y; i++)
	{
		for (unsigned short j = 0; j < matrix->max_x; j++)
		{
			if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
			{
				for (bullet *index = matrix->enemies[i][j].gun->shots; index != NULL; index = (bullet *)index->next)
				{
					switch (matrix->enemies[i][j].type)
					{
					case ENEMY_WEAK:
						spriteX = 157;
						spriteY = 122;
						width = 3;
						height = 7;
						break;
					case ENEMY_NORMAL:
						spriteX = 156;
						spriteY = 154;
						width = 3;
						height = 3;
						break;
					case ENEMY_STRONG:
						if (frame == 1)
						{
							spriteX = 152;
							spriteY = 181;
							width = 3;
							height = 5;
						}
						else
						{
							spriteX = 159;
							spriteY = 181;
							width = 3;
							height = 5;
						}
						break;
					}
					al_draw_bitmap_region(sprite, spriteX, spriteY, width, height, index->x, index->y, 0);
				}
				if (matrix->enemies[i][j].gun->timer)
					matrix->enemies[i][j].gun->timer--;
			}
		}
	}
}
// ================================== FUNÇÔES DE AJUDA ==================================
int randomize_bonus_appearance(bonusEnemy *bonus)
{
	srand(time(NULL));

	int random_number = rand() % 101;

	if (random_number < 10)
	{
		return 1;
	}
	return 0;
}

int main()
{
	al_init();
	al_init_primitives_addon();
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
	ALLEGRO_TIMER *animation_times = al_create_timer(1.0);

	ALLEGRO_FONT *font = al_load_ttf_font("fonts/fonte.ttf", 10, 0);
	if (!font)
	{
		fprintf(stderr, "Falha ao criar font.\n");
		return -1;
	}

	ALLEGRO_FONT *font_large = al_load_ttf_font("fonts/fonte.ttf", 20, 0);
	if (!font_large)
	{
		fprintf(stderr, "Falha ao criar font_large.\n");
		return -1;
	}

	ALLEGRO_DISPLAY *disp = al_create_display(X_SCREEN, Y_SCREEN);
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(disp));
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_timer_event_source(animation_times));

	ALLEGRO_BITMAP *background = al_load_bitmap("background/background.png");
	if (!background)
	{
		fprintf(stderr, "Falha ao carregar a imagem de background.\n");
		return -1;
	}
	// Carregamento do sprite que contém todas as imagens
	ALLEGRO_BITMAP *sprite = al_load_bitmap("sprites/newSprite.png");
	if (!sprite)
	{
		fprintf(stderr, "Falha ao carregar o sprite.\n");
		return -1;
	}

	player *player = player_create(PLAYER_WIDTH, 0, X_SCREEN / 2, Y_SCREEN - PLAYER_HEIGHT, X_SCREEN, Y_SCREEN);
	if (!player)
		return 1;

	enemyMatrix *enemy_matrix = create_enemy_matrix(X_SCREEN, Y_SCREEN, MATRIX_COLUMNS, MATRIX_LINES, ENEMY_WIDTH, ENEMY_HEIGHT, SPACE_X, SPACE_Y);

	obstacle *obstacle_list = create_obstacle_list(X_SCREEN, Y_SCREEN, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, OBSTACLE_SPACE_X, OBSTACLE_AMOUNT);

	bonusEnemy *bonus = create_bonus_enemy(ENEMY_WIDTH, ENEMY_HEIGHT, 0, 30);

	ALLEGRO_EVENT event;
	al_start_timer(timer);
	al_start_timer(animation_times);

	int menu_active = 1;
	while (menu_active)
	{
		al_wait_for_event(queue, &event);
		if ((event.type == 10) && (event.keyboard.keycode == ALLEGRO_KEY_ENTER))
			menu_active = 0;

		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_draw_bitmap(background, 0, 0, 0);
		al_draw_text(font_large, al_map_rgb(0, 255, 0), (X_SCREEN - al_get_text_width(font_large, "SPACE INVADERS")) / 2, Y_SCREEN / 2 - 40, 0, "SPACE INVADERS");
		al_draw_text(font, al_map_rgb(255, 255, 255), (X_SCREEN - al_get_text_width(font, "Aperte enter para começar")) / 2, Y_SCREEN / 2, 0, "Aperte enter para começar");
		al_flip_display();
	}

	int frame = 1;
	int bonus_active = 0;
	int bonus_died = 0;
	while (1)
	{
		al_wait_for_event(queue, &event);
		int tempo_passado = al_get_timer_count(timer) / 30;
		int minutos = tempo_passado / 60;
		int segundos = tempo_passado % 60;

		if (event.type == ALLEGRO_EVENT_TIMER)
		{
			if (event.timer.source == animation_times)
			{
				// Trocar o estado do frame a cada 2 segundos
				frame *= -1;

				// Reiniciar o timer
				al_stop_timer(animation_times);
				al_start_timer(animation_times);
			}
		}

		while (player->life <= 0)
		{
			al_wait_for_event(queue, &event);
			if ((event.type == 10) && (event.keyboard.keycode == ALLEGRO_KEY_ENTER))
			{
				player->life = 3;

				enemy_matrix = create_enemy_matrix(X_SCREEN, Y_SCREEN, MATRIX_COLUMNS, MATRIX_LINES, ENEMY_WIDTH, ENEMY_HEIGHT, SPACE_X, SPACE_Y);

				destroy_obstacle_list(obstacle_list);
				obstacle_list = create_obstacle_list(X_SCREEN, Y_SCREEN, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, OBSTACLE_SPACE_X, OBSTACLE_AMOUNT);

				if (bonus != NULL)
					destroy_bonus_enemy(bonus);

				bonus_active = 0;
				bonus_died = 0;
			}
			else if (event.type == 42)
				break;

			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_bitmap(background, 0, 0, 0);
			al_draw_text(font_large, al_map_rgb(255, 0, 0), (X_SCREEN - al_get_text_width(font_large, "GAME OVER")) / 2, Y_SCREEN / 2 - 40, 0, "GAME OVER");
			al_draw_textf(font, al_map_rgb(255, 255, 255), (X_SCREEN - al_get_text_width(font, "SCORE:   ")) / 2, Y_SCREEN / 2, 0, "SCORE: %d", player->points);
			al_draw_text(font, al_map_rgb(255, 255, 255), (X_SCREEN - al_get_text_width(font, "Aperte enter para recomeçar")) / 2, Y_SCREEN / 2 + 20, 0, "Aperte enter para recomeçar");
			al_flip_display();
		}
		if (event.type == 30)
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_bitmap(background, 0, 0, 0);

			check_bullet_collision(player, enemy_matrix);
			check_player_enemy_collision(player, enemy_matrix, sprite);

			int enemy_alive = any_enemy_alive(enemy_matrix);
			if (!enemy_alive)
			{
				enemy_matrix = create_enemy_matrix(X_SCREEN, Y_SCREEN, MATRIX_COLUMNS, MATRIX_LINES, ENEMY_WIDTH, ENEMY_HEIGHT, SPACE_X, SPACE_Y);

				if (player->life < 5)
					player->life += 1;
			}

			check_enemy_obstacle_collision(enemy_matrix, obstacle_list, sprite);
			check_player_obstacle_collision(player, obstacle_list, sprite);
			int was_shot = check_enemy_player_collision(player, enemy_matrix);

			// Desenha os inimigos na tela
			for (unsigned short i = 0; i < enemy_matrix->max_y; i++)
			{
				for (unsigned short j = 0; j < enemy_matrix->max_x; j++)
				{
					if (enemy_matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
					{
						draw_enemy(&enemy_matrix->enemies[i][j], sprite, frame);
					}
				}
			}

			for (obstacle *current = obstacle_list; current != NULL; current = current->next)
			{
				if (current->life > 0)
					draw_obstacle(current, sprite);
			}
			// Insere as balas existentes disparadas pelo jogador na tela
			for (bullet *index = player->gun->shots; index != NULL; index = (bullet *)index->next)
				al_draw_filled_rectangle(index->x, index->y, index->x + BULLET_WIDTH, index->y + BULLET_HEIGHT, al_map_rgb(0, 255, 0));
			if (player->gun->timer)
				player->gun->timer--;

			if (bonus_died)
			{
				bonus = create_bonus_enemy(ENEMY_WIDTH, ENEMY_HEIGHT, 0, 30);
				bonus_died = 0;
				bonus_active = 0;
			}

			if (bonus_active)
			{
				move_bonus_enemy(bonus, X_SCREEN);
				al_draw_bitmap_region(sprite, 90, 200, ENEMY_WIDTH, ENEMY_HEIGHT, bonus->x, bonus->y, 0);

				bonus_died = check_player_bonus_collision(player, bonus, sprite);
			}
			else
				bonus_active = randomize_bonus_appearance(bonus);

			random_enemy_bullets(enemy_matrix, ENEMY_SHOOTS_AMOUNT);
			update_position(player);
			move_enemies(enemy_matrix, X_SCREEN, Y_SCREEN);

			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "SCORE: %d", player->points);
			al_draw_textf(font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 40, 10, 0, "TIME: %02d:%02d", minutos, segundos);
			al_draw_textf(font, al_map_rgb(255, 255, 255), X_SCREEN - 80, 10, 0, "LIFES: %d", player->life);

			draw_player(player, sprite, was_shot);

			draw_enemy_bullets(enemy_matrix, sprite, frame);

			al_flip_display();
		}
		// Verifica se o evento é de botão do teclado abaixado ou levantado
		else if ((event.type == 10) || (event.type == 12))
		{
			if (event.keyboard.keycode == 1)
				joystick_left(player->control);
			else if (event.keyboard.keycode == 4)
				joystick_right(player->control);
			else if (event.keyboard.keycode == 75)
				joystick_fire(player->control);
		}
		else if (event.type == 42)
			break; // Evento de clique no "X" de fechamento da tela. Encerra o programa graciosamente.
	}

	al_destroy_font(font);						 // Destrutor da fonte padrão
	al_destroy_font(font_large);			 // Destrutor da fonte grande
	al_destroy_display(disp);					 // Destrutor da tela
	al_destroy_timer(timer);					 // Destrutor do relógio
	al_destroy_timer(animation_times); // Destrutor do relógio do frame
	al_destroy_event_queue(queue);		 // Destrutor da fila

	al_destroy_bitmap(background); // Destrutor do background
	al_destroy_bitmap(sprite);		 // Destrutor do sprite

	if (bonus != NULL)
		destroy_bonus_enemy(bonus);
	destroy_obstacle_list(obstacle_list); // Destrutor dos obstaculos
	destroy_player(player);								// Destrutor do jogador

	return 0;
}