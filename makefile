CC = gcc
CFLAGS = $(shell pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 --cflags)
LIBS = $(shell pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 --libs)

SRCS = SpaceInvaders.c Player.c Enemy.c Obstacle.c Joystick.c Bullet.c Pistol.c
OBJS = $(SRCS:.c=.o)

TARGET = AS

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)