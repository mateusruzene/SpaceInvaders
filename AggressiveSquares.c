// Compilação: gcc AggressiveSquares.c Square.c Joystick.c Bullet.c Pistol.c -o AS $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 --libs --cflags)
#include <stdlib.h>
#include <stdio.h>

#include <allegro5/allegro5.h>					 //Biblioteca base do Allegro
#include <allegro5/allegro_font.h>			 //Biblioteca de fontes do Allegro
#include <allegro5/allegro_primitives.h> //Biblioteca de figuras básicas
#include <allegro5/allegro_image.h>

#include "Player.h" //Inclusão da biblioteca de quadrados

#define X_SCREEN 1000 // Definição do tamanho da tela em pixels no eixo x
#define Y_SCREEN 650	// Definição do tamanho da tela em pixels no eixo y

// Implementação da função de verificação de colisão entre dois quadrados
unsigned char collision_2D(player *element_first, player *element_second)
{
	if ((((element_second->y - element_second->side / 2 >= element_first->y - element_first->side / 2) && (element_first->y + element_first->side / 2 >= element_second->y - element_second->side / 2)) ||	//				//Verifica se o primeiro elemento colidiu com o segundo no eixo X
			 ((element_first->y - element_first->side / 2 >= element_second->y - element_second->side / 2) && (element_second->y + element_second->side / 2 >= element_first->y - element_first->side / 2))) && //				//Verifica se o segundo elemento colidiu com o primeiro no eixo X
			(((element_second->x - element_second->side / 2 >= element_first->x - element_first->side / 2) && (element_first->x + element_first->side / 2 >= element_second->x - element_second->side / 2)) ||	//				//Verifica se o primeiro elemento colidiu com o segundo no eixo Y
			 ((element_first->x - element_first->side / 2 >= element_second->x - element_second->side / 2) && (element_second->x + element_second->side / 2 >= element_first->x - element_first->side / 2))))
		return 1; // Verifica se o segundo elemento colidiu com o primeiro no eixo Y
	else
		return 0; // Se as condições não forem satisfeita, não houve colisão
}

// Implementação da função que verifica se um projétil acertou um jogador (!)
unsigned char check_kill(player *killer, player *victim)
{
	for (bullet *index = killer->gun->shots; index != NULL; index = (bullet *)index->next)
	{																																																	// Para todos os projéteis do atirador (!)
		if ((index->x >= victim->x - victim->side / 2) && (index->x <= victim->x + victim->side / 2) && //																										//Verique se houve colisão com a vítima no eixo X (!)
				(index->y >= victim->y - victim->side / 2) && (index->y <= victim->y + victim->side / 2))
		{						// Verifique se houve colisão com a vítima no eixo Y (!)
			return 1; // Se sim, retorne que o quadrado vítima foi atingido
		}
	}
	return 0; // Se não houver nenhum projétil que acertou a vítima, retorne falso (!)
}

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
void update_position(player *player_1, player *player_2)
{
	// Se o botão de movimentação para esquerda do controle do primeiro jogador está ativado...
	if (player_1->control->left)
	{
		player_move(player_1, 1, 0, X_SCREEN, Y_SCREEN); // Move o quadrado do primeiro jogador para a esquerda
		if (collision_2D(player_1, player_2))
			player_move(player_1, -1, 0, X_SCREEN, Y_SCREEN); // Se o movimento causou uma colisão entre quadrados, desfaça o mesmo
	}

	// Se o botão de movimentação para direita do controle do primeir ojogador está ativado...
	if (player_1->control->right)
	{
		player_move(player_1, 1, 1, X_SCREEN, Y_SCREEN); // Move o quadrado do primeiro jogador para a direta
		if (collision_2D(player_1, player_2))
			player_move(player_1, -1, 1, X_SCREEN, Y_SCREEN); // Se o movimento causou uma colisão entre quadrados, desfaça o mesmo
	}

	// Verifica se o primeiro jogador está atirando (!)
	if (player_1->control->fire)
	{
		if (!player_1->gun->timer)
		{																					// Verifica se a arma do primeiro jogador não está em cooldown (!)
			player_shot(player_1);									// Se não estiver, faz um disparo (!)
			player_1->gun->timer = PISTOL_COOLDOWN; // Inicia o cooldown da arma (!)
		}
	}

	update_bullets(player_1); // Atualiza os disparos do primeiro jogador (!)
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

	player *player_1 = player_create(20, 0, 10, Y_SCREEN / 2, X_SCREEN, Y_SCREEN); // Cria o quadrado do primeiro jogador
	if (!player_1)
		return 1;																																								// Verificação de erro na criação do quadrado do primeiro jogador
	player *player_2 = player_create(20, 1, X_SCREEN - 10, Y_SCREEN / 2, X_SCREEN, Y_SCREEN); // Cria o quadrado do segundo jogador
	if (!player_2)
		return 2; // Verificação de erro na criação do quadrado do segundo jogador

	ALLEGRO_EVENT event;	 // Variável que guarda um evento capturado, sua estrutura é definida em: https:		//www.allegro.cc/manual/5/ALLEGRO_EVENT
	al_start_timer(timer); // Função que inicializa o relógio do programa

	int menu_active = 1;
	while (menu_active)
	{
		al_wait_for_event(queue, &event);

		if ((event.type == 10) && (event.keyboard.keycode == 75))
			menu_active = 0;

		al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpe a tela atual para um fundo preto
		al_draw_text(font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 60, Y_SCREEN / 2 - 40, 0, "SPACE INVADERS");
		al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 110, Y_SCREEN / 2 - 15, 0, "Aperte espaço para começar");
		al_flip_display();
	}

	ALLEGRO_BITMAP *player_bitmap = al_load_bitmap("space_ship/space_ship_1.png");
	if (!player_bitmap)
	{
		fprintf(stderr, "Falha ao carregar a imagem do jogador.\n");
		return -1;
	}

	unsigned char p1k = 0, p2k = 0; // Variáveis de controle de vida dos quadrados (jogadores) (!)
	while (1)
	{																		// Laço servidor do jogo
		al_wait_for_event(queue, &event); // Função que captura eventos da fila, inserindo os mesmos na variável de eventos
		int tempo_passado = al_get_timer_count(timer) / 30;
		// Calcule os minutos e segundos
		int minutos = tempo_passado / 60;
		int segundos = tempo_passado % 60;

		if (p1k || p2k)
		{																					// Verifica se algum jogador foi morto
			al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpe a tela atual para um fundo preto
			if (p2k && p1k)
				al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 40, Y_SCREEN / 2 - 15, 0, "EMPATE!"); // Se ambos foram mortos, declare um empate
			else if (p2k)
				al_draw_text(font, al_map_rgb(255, 0, 0), X_SCREEN / 2 - 75, Y_SCREEN / 2 - 15, 0, "JOGADOR 1 GANHOU!"); // Se o segundo jogador morreu, declare o primeiro jogador vencedor
			else if (p1k)
				al_draw_text(font, al_map_rgb(0, 0, 255), X_SCREEN / 2 - 75, Y_SCREEN / 2 - 15, 0, "JOGADOR 2 GANHOU!");						// Se o primeiro jogador morreu, declare o segundo jogador vencedor
			al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 110, Y_SCREEN / 2 + 5, 0, "PRESSIONE ESPAÇO PARA SAIR"); // Indique o modo de conclusão do programa
			al_flip_display();																																																		// Atualiza a tela

			if ((event.type == 10) && (event.keyboard.keycode == 75))
				break; // Espera por um evento de teclado, de clique da tecla de espaço
			else if (event.type == 42)
				break; // Finaliza o jogo
		}
		else
		{ // Se nenhum quadrado morreu
			if (event.type == 30)
			{																					// O evento tipo 30 indica um evento de relógio, ou seja, verificação se a tela deve ser atualizada (conceito de FPS)
				update_position(player_1, player_2);		// Atualiza a posição dos jogadores
				p1k = check_kill(player_2, player_1);		// Verifica se o primeiro jogador matou o segundo jogador (!)
				p2k = check_kill(player_1, player_2);		// Verifica se o segundo jogador matou o primeiro jogador (!)
				al_clear_to_color(al_map_rgb(0, 0, 0)); // Substitui tudo que estava desenhado na tela por um fundo preto

				al_draw_textf(font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 50, 10, 0, "Tempo: %02d:%02d", minutos, segundos);

				// Insere a img do jogador na tela
				al_draw_bitmap(player_bitmap, player_1->x - al_get_bitmap_width(player_bitmap) / 2, player_1->y - al_get_bitmap_height(player_bitmap) / 2, 0);

				// Insere as balas existentes disparadas pelo primeiro jogador na tela (!)
				for (bullet *index = player_1->gun->shots; index != NULL; index = (bullet *)index->next)
					al_draw_filled_circle(index->x, index->y, 2, al_map_rgb(255, 0, 0));
				if (player_1->gun->timer)
					player_1->gun->timer--; // Atualiza o cooldown da arma do primeiro jogador (!)
				al_flip_display();				// Insere as modificações realizadas nos buffers de tela
			}
			// Verifica se o evento é de botão do teclado abaixado ou levantado
			else if ((event.type == 10) || (event.type == 12))
			{
				if (event.keyboard.keycode == 1)
					joystick_left(player_1->control); // Indica o evento correspondente no controle do primeiro jogador (botão de movimentação à esquerda)
				else if (event.keyboard.keycode == 4)
					joystick_right(player_1->control); // Indica o evento correspondente no controle do primeiro jogador (botão de movimentação à direita)
				else if (event.keyboard.keycode == 3)
					joystick_fire(player_1->control); // Indica o evento correspondente no controle do primeiro joagdor (botão de disparo - c) (!)
			}
			else if (event.type == 42)
				break; // Evento de clique no "X" de fechamento da tela. Encerra o programa graciosamente.
		}
	}

	al_destroy_font(font);				 // Destrutor da fonte padrão
	al_destroy_display(disp);			 // Destrutor da tela
	al_destroy_timer(timer);			 // Destrutor do relógio
	al_destroy_event_queue(queue); // Destrutor da fila
	player_destroy(player_1);			 // Destrutor do quadrado do primeiro jogador
	player_destroy(player_2);			 // Destrutor do quadrado do segundo jogador

	return 0;
}