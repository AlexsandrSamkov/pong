/*Copyright 2021 mwatson lgriffin kmorton 21school Novosibirsk*/
#ifndef PONG_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#define MAP_SIZE 2000
#define MAX_X 80
#define MAX_Y 25
#define P1_UP 97
#define P1_DOWN 122
#define P2_UP 107
#define P2_DOWN 109
#define SPACE 32
#define GAME 2
#define RESTART 3
#define ZERO  " # #\n#   #\n#   #\n#   #\n # #\n"
#define ONE   "  #\n# #\n  #\n  #\n  #\n"
#define TWO   " ##\n#  #\n  #\n #\n####\n"
#define THREE "# #\n   #\n #\n   #\n# #\n"
#define FOUR  "#  #\n#  #\n####\n   #\n   #\n"
#define FIVE  "####\n#\n ##\n   #\n###\n"
#define SIX   " ###\n#\n####\n#   #\n ###\n"
#define SEVEN " # # #\n    #\n   #\n  #\n #\n"
#define EIGHT " # #\n#   #\n # #\n#   #\n # #\n"
#define NINE  " # #\n#   #\n # ##\n    #\n # # \n"
#endif
typedef struct s_players {
  int p1_pos_x;
  int p1_pos_y;
  int p2_pos_x;
  int p2_pos_y;
  int points[2];
}             t_players;

typedef struct s_ball {
  int x;
  int y;
  int dir_ud;
  int dir_lr;
}             t_ball;

typedef struct s_all {
    t_players  *pl;
    t_ball     *ball;
    int key;
    int status;
    char *map;
}             t_all;

void set_keypress(struct termios *stored_settings);
void reset_keypress(struct termios *stored_settings);
int get_pos(int x, int y);
void draw_line(int x, int y, char *map);
char *get_numeric(int nbr);
void print_numeric(char *nbr, char *map, int pos_x, int pos_y);
void print_points(int points[], char *map);
int sim_ball(t_ball *ball, char *map);
void control(t_players *pl, char *map, int key);
void init(t_all *all);
void restart(t_all *all);
void game(t_all *all);

int main(void) {
  struct termios stored_settings;
  fd_set rfds;
  struct timeval tv;
  char map[MAP_SIZE];
  t_players pl;
  t_ball ball;
  t_all all;
  all.pl = &pl;
  all.ball = &ball;
  all.map = map;
  init(&all);
  set_keypress(&stored_settings);
  while (1) {
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    all.key = select(2, &rfds, NULL, NULL, &tv);
    if (all.key)
      all.key = getc(stdin);
    if (all.status == GAME)
        game(&all);
    else if (all.status != RESTART) {
      all.pl->points[all.status]++;
      if (all.pl->points[0] == 21 || all.pl->points[1] == 21) {
          print_points(all.pl->points, all.map);
          write(1, all.map, MAP_SIZE);
          break;
      }
        all.status = RESTART;
    }
    if (all.key == SPACE && all.status == RESTART)
      restart(&all);
    usleep(100000);
  }
  reset_keypress(&stored_settings);
  if (all.pl->points[1] == 21)
    printf("                                   Winner 1\n");
  else
    printf("                                   Winner 2\n");
  printf("                                   GAME OVER\n");
  return(0);
}

void reset_keypress(struct termios *stored_settings) {
  tcsetattr(0, TCSANOW, stored_settings);
  return;
}

int get_pos(int x, int y) {
  return(y * 80 + x);
}

void draw_line(int x, int y, char *map) {
  map[get_pos(x, y - 1)] = '|';
  map[get_pos(x, y + 1)] = '|';
  map[get_pos(x, y)] = '|';
}

char *get_numeric(int nbr) {
  if (nbr == 1)
    return(ONE);
  if (nbr == 2)
    return(TWO);
  if (nbr == 3)
    return(THREE);
  if (nbr == 4)
    return(FOUR);
  if (nbr == 5)
    return(FIVE);
  if (nbr == 6)
    return(SIX);
  if (nbr == 7)
    return(SEVEN);
  if (nbr == 8)
    return(EIGHT);
  if (nbr == 9)
    return(NINE);
  return(ZERO);
}

void print_numeric(char *nbr, char *map, int pos_x, int pos_y) {
  int x = pos_x;
  int y = pos_y;
  int i = 0;
  int j = 0;
  while (i < 6) {
    while (j < 7) {
      if (x + j < MAX_X && y + i < MAX_Y)
        map[get_pos(x + j, y + i)] = ' ';
      j++;
    }
    j = 0;
    i++;
  }
  i = 0;
  while (nbr[i]) {
    if (nbr[i] == '\n') {
      i++;
      x = pos_x;
      y++;
      continue;
    }
    x++;
      if (x < MAX_X && y < MAX_Y && x >= 0 && y >= 0)
          map[get_pos(x, y)] = nbr[i];
    i++;
  }
}

void print_points(int points[], char *map) {
  print_numeric(get_numeric(points[0] / 10), map, 33, 0);
  print_numeric(get_numeric(points[0] % 10), map, 40, 0);
  print_numeric(get_numeric(points[1] / 10), map, 33, 20);
  print_numeric(get_numeric(points[1] % 10), map, 40, 20);
}

int sim_ball(t_ball *ball, char *map) {
  int pl_1 = 0;
  int pl_2 = 0;
  if (map[get_pos(ball->x, ball->y)] != '|')
    map[get_pos(ball->x, ball->y)] = ' ';
  if (ball->x == 0)
    ball->dir_ud = 1;
  if (ball->x == MAX_X - 1)
    ball->dir_ud = - 1;
  if (ball->y == 0)
    ball->dir_lr = 1;
  if (ball->y == MAX_Y - 1)
    ball->dir_lr = -1;
  if (ball->x - 1 == 0 && ball->dir_ud == - 1
     && map[get_pos(ball->x - 1, ball->y)] == ' ')
      pl_1++;
  if ((ball->y + 1 == MAX_Y
     || !(map[get_pos(ball->x - 1, ball->y + 1)] == '|'))
     && (ball->y - 1 == - 1
     || !(map[get_pos(ball->x - 1, ball->y - 1)] == '|')))
      pl_1++;
  if (ball->x + 1 == MAX_X - 1 && ball->dir_ud == 1
    && map[get_pos(ball->x + 1, ball->y)] == ' ')
    pl_2++;
  if ((ball->y + 1 == MAX_Y
    || !(map[get_pos(ball->x + 1, ball->y + 1)] == '|'))
    && (ball->y - 1 == -1  || !(map[get_pos(ball->x + 1, ball->y - 1)] == '|')))
      pl_2++;
  if (pl_1 == 2)
      return (0);
  if (pl_2 == 2)
    return (1);
  ball->x += ball->dir_ud;
  ball->y += ball->dir_lr;
  return (2);
}

void control(t_players *pl, char *map, int key) {
    if (key == P1_DOWN && pl->p1_pos_y != MAX_Y - 2) {
    map[get_pos(pl->p1_pos_x, pl->p1_pos_y - 1)] = ' ';
    pl->p1_pos_y++;
    draw_line(pl->p1_pos_x, pl->p1_pos_y, map);
  }
  if (key == P1_UP && pl->p1_pos_y != 1) {
    map[get_pos(pl->p1_pos_x, pl->p1_pos_y + 1)] = ' ';
    pl->p1_pos_y--;
    draw_line(pl->p1_pos_x, pl->p1_pos_y, map);
  }
  if (key == P2_DOWN && pl->p2_pos_y != MAX_Y - 2) {
    map[get_pos(pl->p2_pos_x, pl->p2_pos_y - 1)] = ' ';
    pl->p2_pos_y++;
    draw_line(pl->p2_pos_x, pl->p2_pos_y, map);
  }
  if (key == P2_UP && pl->p2_pos_y != 1) {
    map[get_pos(pl->p2_pos_x, pl->p2_pos_y + 1)] = ' ';
    pl->p2_pos_y--;
    draw_line(pl->p2_pos_x, pl->p2_pos_y, map);
  }
}

void init(t_all *all) {
  all->key = 0;
  all->status = RESTART;
  all->ball->x = 39;
  all->ball->y = 12;
  all->ball->dir_lr = -1;
  all->ball->dir_ud = -1;
  all->pl->p1_pos_x = 0;
  all->pl->p1_pos_y = 12;
  all->pl->p2_pos_x = 79;
  all->pl->p2_pos_y = 12;
  all->pl->points[0] = 0;
  all->pl->points[1] = 0;
  for (int i = 0; i < MAP_SIZE; i++)
    all->map[i] = ' ';
  draw_line(all->pl->p1_pos_x, all->pl->p1_pos_y, all->map);
  draw_line(all->pl->p2_pos_x, all->pl->p2_pos_y, all->map);
  print_points(all->pl->points, all->map);
  write(1, all->map, MAP_SIZE);
}

void restart(t_all *all) {
  all->map[get_pos(all->ball->x, all->ball->y)] = ' ';
  all->status = 2;
  if (all->ball->dir_lr == 1)
    all->ball->dir_lr = -1;
  else
    all->ball->dir_lr = 1;
  if (all->ball->dir_ud == 1)
    all->ball->dir_ud = -1;
  else
    all->ball->dir_ud = 1;
  all->ball->x = 39;
  all->ball->y = 12;
}

void game(t_all *all) {
  all->status = sim_ball(all->ball, all->map);
  print_points(all->pl->points, all->map);
  if (all->map[get_pos(all->ball->x, all->ball->y)] != '|')
    all->map[get_pos(all->ball->x, all->ball->y)] = '#';
  control(all->pl, all->map, all->key);
  write(1, all->map, MAP_SIZE);
}

void set_keypress(struct termios *stored_settings) {
  struct termios new_settings;
  tcgetattr(0, stored_settings);
  new_settings = *stored_settings;
  new_settings.c_lflag &= (~ICANON & ~ECHO);
  new_settings.c_cc[VTIME] = 0;
  new_settings.c_cc[VMIN] = 1;
  tcsetattr(0, TCSANOW, &new_settings);
  return;
}
