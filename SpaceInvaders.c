#include <stdlib.h>
#include <stdio.h>

#include <allegro5/allegro5.h>					 //Biblioteca base do Allegro
#include <allegro5/allegro_font.h>			 //Biblioteca de fontes do Allegro
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

// Definições dos inimigos
#define ENEMY_WIDTH 25		// Largura do inimigo
#define ENEMY_HEIGHT 25		// Altura do inimigo
#define SPACE_X 15				// Espaço entre os inimigos X
#define SPACE_Y 15				// Espaço entre os inimigos Y
#define MATRIX_LINES 5		// Número de linhas
#define MATRIX_COLUMNS 10 // Número de colunas

// Definições dos obstaculos
#define OBSTACLE_WIDTH 80		 // Largura do obstaculo
#define OBSTACLE_HEIGHT 80	 // Altura do obstaculo
#define OBSTACLE_AMOUNT 4		 // Número de obstaculos
#define OBSTACLE_SPACE_X 100 // Espaço entre os obstaculos X

// Implementação da função que atualiza o posicionamento de projéteis conforme o movimento dos mesmos (!)
void update_bullets(player *player)
{
	bullet *previous = NULL; // Variável auxiliar para salvar a posição imediatamente anterior na fila (!)
	for (bullet *index = player->gun->shots; index != NULL;)
	{ // Para cada projétil presente na lista de projéteis disparados (!)
		if (!index->trajectory)
			index->y -= BULLET_MOVE; // Se a trajetória for para a cima, atualiza a posição para a esquerda (!)
		else if (index->trajectory == 1)
			index->y -= BULLET_MOVE; // Se a trajetória for para a cima, atualiza a posição para a esquerda (!)

		if ((index->y < 0) || (index->y > Y_SCREEN))
		{ // Verifica se o projétil saiu das bordas da janela (!)
			if (previous)
			{																		// Verifica se não é o primeiro elemento da lista de projéteis (!)
				previous->next = index->next;			// Se não for, salva o próximo projétil (!)
				bullet_destroy(index);						// Chama o destrutor para o projétil atual (!)
				index = (bullet *)previous->next; // Atualiza para o próximo projétil (!)
			}
			else
			{																							// Se for o primeiro projétil da lista (!)
				player->gun->shots = (bullet *)index->next; // Atualiza o projétil no início da lista (!)
				bullet_destroy(index);											// Chama o destrutor para o projétil atual (!)
				index = player->gun->shots;									// Atualiza para o próximo projétil (!)
			}
		}
		else
		{																 // Se não saiu da tela (!)
			previous = index;							 // Atualiza o projétil anterior (para a próxima iteração) (!)
			index = (bullet *)index->next; // Atualiza para o próximo projétil (!)
		}
	}
}

// Função de atualização das posições dos quadrados conforme os comandos do controle
void update_position(player *player_1)
{
	if (player_1->control->left)
		player_move(player_1, 1, 0, X_SCREEN, Y_SCREEN);

	if (player_1->control->right)
		player_move(player_1, 1, 1, X_SCREEN, Y_SCREEN);

	// Verifica se o primeiro jogador está atirando (!)
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

void draw_enemy(enemy *e)
{
	if (e->type == ENEMY_WEAK)
		al_draw_filled_rectangle(e->x, e->y, e->x + e->sizeX, e->y + e->sizeY, al_map_rgb(255, 255, 255));
	if (e->type == ENEMY_NORMAL)
		al_draw_filled_rectangle(e->x, e->y, e->x + e->sizeX, e->y + e->sizeY, al_map_rgb(255, 0, 0));
	if (e->type == ENEMY_STRONG)
		al_draw_filled_rectangle(e->x, e->y, e->x + e->sizeX, e->y + e->sizeY, al_map_rgb(0, 0, 255));
}

// Função para verificar colisões entre tiros e inimigos
void check_player_enemy_collision(player *player, enemyMatrix *matrix)
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

void draw_enemy_bullets(enemyMatrix *matrix)
{
	for (unsigned short i = 0; i < matrix->max_y; i++)
	{
		for (unsigned short j = 0; j < matrix->max_x; j++)
		{
			if (matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
			{
				for (bullet *index = matrix->enemies[i][j].gun->shots; index != NULL; index = (bullet *)index->next)
				{
					al_draw_filled_rectangle(index->x, index->y, index->x + 2, index->y + 10, al_map_rgb(255, 0, 0));
				}
				if (matrix->enemies[i][j].gun->timer)
					matrix->enemies[i][j].gun->timer--;
			}
		}
	}
}

void draw_obstacle(obstacle *o)
{
	al_draw_filled_rectangle(o->x, o->y, o->x + o->sizeX, o->y + o->sizeY, al_map_rgb(0, 255, 0));
}

void check_player_obstacle_collision(player *player, obstacle *obstacle_list)
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

void check_enemy_obstacle_collision(enemyMatrix *matrix, obstacle *obstacle_list)
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

void check_enemy_player_collision(player *player, enemyMatrix *matrix)
{
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
}

int main()
{

	al_init();									// Faz a preparação de requisitos da biblioteca Allegro
	al_init_primitives_addon(); // Faz a inicialização dos addons das imagens básicas
	al_install_keyboard();			// Habilita a entrada via teclado (eventos de teclado), no programa
	al_init_image_addon();

	ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);						 // Cria o relógio do jogo; isso indica quantas atualizações serão realizadas por segundo (30, neste caso)
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();					 // Cria a fila de eventos; todos os eventos (programação orientada a eventos)
	ALLEGRO_FONT *font = al_create_builtin_font();								 // Carrega uma fonte padrão para escrever na tela (é bitmap, mas também suporta adicionar fontes ttf)
	ALLEGRO_DISPLAY *disp = al_create_display(X_SCREEN, Y_SCREEN); // Cria uma janela para o programa, define a largura (x) e a altura (y) da tela em píxeis (320x320, neste caso)

	al_register_event_source(queue, al_get_keyboard_event_source());		// Indica que eventos de teclado serão inseridos na nossa fila de eventos
	al_register_event_source(queue, al_get_display_event_source(disp)); // Indica que eventos de tela serão inseridos na nossa fila de eventos
	al_register_event_source(queue, al_get_timer_event_source(timer));	// Indica que eventos de relógio serão inseridos na nossa fila de eventos

	player *player = player_create(PLAYER_WIDTH, 0, X_SCREEN / 2, Y_SCREEN - PLAYER_HEIGHT, X_SCREEN, Y_SCREEN);
	if (!player)
		return 1;

	enemyMatrix *enemy_matrix = create_enemy_matrix(X_SCREEN, Y_SCREEN, MATRIX_COLUMNS, MATRIX_LINES, ENEMY_WIDTH, ENEMY_HEIGHT, SPACE_X, SPACE_Y);

	obstacle *obstacle_list = createObstacleList(X_SCREEN, Y_SCREEN, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, OBSTACLE_SPACE_X, OBSTACLE_AMOUNT);

	ALLEGRO_EVENT event;
	al_start_timer(timer);

	int menu_active = 1;
	while (menu_active)
	{
		al_wait_for_event(queue, &event);

		if ((event.type == 10) && (event.keyboard.keycode == ALLEGRO_KEY_ENTER))
			menu_active = 0;

		al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpe a tela atual para um fundo preto
		al_draw_text(font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 60, Y_SCREEN / 2 - 40, 0, "SPACE INVADERS");
		al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 110, Y_SCREEN / 2 - 15, 0, "Aperte enter para começar");
		al_flip_display();
	}

	ALLEGRO_BITMAP *player_bitmap = al_load_bitmap("space_ship/space_ship_1.png");
	if (!player_bitmap)
	{
		fprintf(stderr, "Falha ao carregar a imagem do jogador.\n");
		return -1;
	}

	while (1)
	{																		// Laço servidor do jogo
		al_wait_for_event(queue, &event); // Função que captura eventos da fila, inserindo os mesmos na variável de eventos
		int tempo_passado = al_get_timer_count(timer) / 30;
		// Calcule os minutos e segundos
		int minutos = tempo_passado / 60;
		int segundos = tempo_passado % 60;

		while (player->life <= 0)
		{
			al_wait_for_event(queue, &event);

			if ((event.type == 10) && (event.keyboard.keycode == ALLEGRO_KEY_ENTER))
			{
				player->life = 3;

				destroy_enemy_matrix(enemy_matrix);
				enemy_matrix = create_enemy_matrix(X_SCREEN, Y_SCREEN, MATRIX_COLUMNS, MATRIX_LINES, ENEMY_WIDTH, ENEMY_HEIGHT, SPACE_X, SPACE_Y);
			}
			else if (event.type == 42)
				break;

			al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpe a tela atual para um fundo preto
			al_draw_text(font, al_map_rgb(255, 0, 0), X_SCREEN / 2 - 60, Y_SCREEN / 2 - 45, 0, "GAME OVER");
			al_draw_textf(font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 60, Y_SCREEN / 2 - 30, 0, "SCORE: %d", player->points);
			al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 200, Y_SCREEN / 2 - 15, 0, "Aperte enter para recomeçar ou esc para sair");

			al_flip_display();
		}
		if (event.type == 30)
		{
			al_clear_to_color(al_map_rgb(0, 0, 0)); // Substitui tudo que estava desenhado na tela por um fundo preto
			check_player_enemy_collision(player, enemy_matrix);

			int enemy_alive = any_enemy_alive(enemy_matrix);
			if (!enemy_alive)
			{
				destroy_enemy_matrix(enemy_matrix);
				enemy_matrix = create_enemy_matrix(X_SCREEN, Y_SCREEN, MATRIX_COLUMNS, MATRIX_LINES, ENEMY_WIDTH, ENEMY_HEIGHT, SPACE_X, SPACE_Y);

				if (player->life < 5)
					player->life += 1;
			}

			check_enemy_obstacle_collision(enemy_matrix, obstacle_list);
			check_player_obstacle_collision(player, obstacle_list);
			check_enemy_player_collision(player, enemy_matrix);

			// Desenha os inimigos na tela
			for (unsigned short i = 0; i < enemy_matrix->max_y; i++)
			{
				for (unsigned short j = 0; j < enemy_matrix->max_x; j++)
				{
					if (enemy_matrix->enemies[i][j].aliveOrDead == IS_ALIVE)
					{
						draw_enemy(&enemy_matrix->enemies[i][j]);
					}
				}
			}

			for (obstacle *current = obstacle_list; current != NULL; current = current->next)
			{
				if (current->life > 0)
					draw_obstacle(current);
			}
			// Insere as balas existentes disparadas pelo primeiro jogador na tela (!)
			for (bullet *index = player->gun->shots; index != NULL; index = (bullet *)index->next)
				al_draw_filled_rectangle(index->x, index->y, index->x + 2, index->y + 10, al_map_rgb(0, 255, 0));

			if (player->gun->timer)
				player->gun->timer--; // Atualiza o cooldown da arma do primeiro jogador (!)

			random_enemy_bullets(enemy_matrix, ENEMY_SHOOTS_AMOUNT);
			update_position(player);
			move_enemies(enemy_matrix, X_SCREEN, Y_SCREEN);

			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "SCORE: %d", player->points);
			al_draw_textf(font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 40, 10, 0, "TIME: %02d:%02d", minutos, segundos);
			al_draw_textf(font, al_map_rgb(255, 255, 255), X_SCREEN - 80, 10, 0, "LIFES: %d", player->life);

			// Insere a img do jogador na tela
			al_draw_bitmap(player_bitmap, player->x - al_get_bitmap_width(player_bitmap) / 2, player->y - al_get_bitmap_height(player_bitmap) / 2, 0);

			draw_enemy_bullets(enemy_matrix);

			al_flip_display(); // Insere as modificações realizadas nos buffers de tela
		}
		// Verifica se o evento é de botão do teclado abaixado ou levantado
		else if ((event.type == 10) || (event.type == 12))
		{
			if (event.keyboard.keycode == 1)
				joystick_left(player->control); // Indica o evento correspondente no controle do primeiro jogador (botão de movimentação à esquerda)
			else if (event.keyboard.keycode == 4)
				joystick_right(player->control); // Indica o evento correspondente no controle do primeiro jogador (botão de movimentação à direita)
			else if (event.keyboard.keycode == 75)
				joystick_fire(player->control); // Indica o evento correspondente no controle do primeiro joagdor (botão de disparo - c) (!)
		}
		else if (event.type == 42)
			break; // Evento de clique no "X" de fechamento da tela. Encerra o programa graciosamente.
	}

	al_destroy_font(font);								// Destrutor da fonte padrão
	al_destroy_display(disp);							// Destrutor da tela
	al_destroy_timer(timer);							// Destrutor do relógio
	al_destroy_event_queue(queue);				// Destrutor da fila
	destroy_enemy_matrix(enemy_matrix);		// Destrutor dos inimigos
	destroy_obstacle_list(obstacle_list); // Destrutor dos obstaculos
	player_destroy(player);								// Destrutor do jogador

	return 0;
}