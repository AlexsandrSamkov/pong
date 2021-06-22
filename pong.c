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
#define ZERO  " # #\n#   #\n#   #\n#   #\n # #\n "
#define ONE   "  #\n# #\n  #\n  #\n  #\n         "
#define TWO   " ##\n#  #\n  #\n #\n####\n        "
#define THREE "# #\n   #\n #\n   #\n# #\n        "
#define FOUR  "#  #\n#  #\n####\n   #\n   #\n    "
#define FIVE  "####\n#\n ##\n   #\n###\n         "
#define SIX   " ###\n#\n####\n#   #\n ###\n      "
#define SEVEN " # # #\n    #\n   #\n  #\n #\n    "
#define EIGHT " # #\n#   #\n # #\n#   #\n # #\n  "
#define NINE  " # #\n#   #\n # ##\n    #\n # # \n"

typedef struct s_players {
  int p1_pos_x;
  int p1_pos_y;
  int p1_direction;
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

static struct termios stored_settings;

void set_keypress(void) {
  struct termios new_settings;
  tcgetattr(0,&stored_settings);
  new_settings = stored_settings;
  new_settings.c_lflag &= (~ICANON & ~ECHO);
  new_settings.c_cc[VTIME] = 0;
  new_settings.c_cc[VMIN] = 1;
  tcsetattr(0,TCSANOW,&new_settings);
  return;
}

void reset_keypress(void) {
  tcsetattr(0,TCSANOW,&stored_settings);
  return;
}

int get_pos(int x, int y) {
  return(y * 80 + x);
}

void draw_line(int x, int y, char *map) {
  map[get_pos(x, y - 1)] = '|' ;
  map[get_pos(x, y + 1)] = '|' ; 
  map[get_pos(x, y)] = '|';
}

char *get_numeric(int nbr)
{
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
  return (ZERO);                    
}

void print_numeric(char *nbr, char *map,int pos_x, int pos_y)
{
  int x = pos_x;
  int y = pos_y;
  int i = 0;
  int j = 0;
  while (i < 6)
  {
    while (j < 7)
    {
      map[get_pos(x + j, y + i)] = ' ';
      j++;
    }
    j = 0;
    i++;
  }
  i = 0;
  while (nbr[i]) {
    if (nbr[i] == '\n')
    {
      i++;
      x = pos_x;
      y++;
      continue;
    }
    x++;
    map[get_pos(x, y)] = nbr[i];
    i++;
  }
}

void print_points(int points[], char *map)
{
  print_numeric(get_numeric(points[0] / 10), map, 33, 0);
  print_numeric(get_numeric(points[0] % 10), map, 40, 0);
  print_numeric(get_numeric(points[1] / 10), map, 33, 20);
  print_numeric(get_numeric(points[1] % 10), map, 40, 20);  
}

int sim_ball(t_ball *ball, char *map) {
  if (map[get_pos(ball->x, ball->y)] != '|')
    map[get_pos(ball->x, ball->y)] = ' ';
  if (ball->x == 0)
    ball->dir_ud = 1;
  if (ball->x == MAX_X - 1)
    ball->dir_ud = -1; 
  if (ball->y == 0)
    ball->dir_lr = 1;
  if (ball->y == MAX_Y - 1)
    ball->dir_lr = -1;
  if (ball->x - 1 == 0 && ball->dir_ud == -1 
     && map[get_pos(ball->x - 1, ball->y)] == ' ')
    return(0);
  if (ball->x + 1 == MAX_X - 1 && ball->dir_ud == 1 
     && map[get_pos(ball->x + 1, ball->y)] == ' ')
    return(1);    
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

void init(t_players *pl, t_ball *ball, char *map) {
  ball->x = 39;
  ball->y = 12;
  ball->dir_lr = -1;
  ball->dir_ud = -1;
  pl->p1_pos_x = 0;
  pl->p1_pos_y = 12;
  pl->p2_pos_x = 79;
  pl->p2_pos_y = 12;
  pl->points[0] = 0;
  pl->points[1] = 0;
  for (int i = 0; i < MAP_SIZE; i++)
    map[i] = ' ';
  draw_line(pl->p1_pos_x, pl->p1_pos_y, map);
  draw_line(pl->p2_pos_x, pl->p2_pos_y, map);
  print_points(pl->points, map);
  
}

int main(void) {
  fd_set rfds;
  struct timeval tv;
  int key = 0;
  int ret = 2;
  char map[MAP_SIZE];
  t_players pl;
  t_ball ball;
  init(&pl, &ball, map);
  write(1,map,MAP_SIZE);
  set_keypress();
  while(1) {
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    key = select(2, &rfds, NULL, NULL, &tv);
    if (key)
    {
      key = getc(stdin);
        //printf("%d",key);
    }
    if (ret == 2)
    {
      ret = sim_ball(&ball,map);
      print_points(pl.points,map);
      if (map[get_pos(ball.x, ball.y)] != '|')
        map[get_pos(ball.x, ball.y)] = '#';
      control(&pl, map, key);
      write(1,map,MAP_SIZE);
    }
    else if (ret != 3)
    {
      pl.points[ret]++;
      ret = 3;
    }
    if (key == 32 && ret == 3)
    {
      map[get_pos(ball.x, ball.y)] = ' ';
      ret = 2;
      if (ball.dir_lr == 1)
        ball.dir_lr = -1; 
      else  
        ball.dir_lr = 1;
      if (ball.dir_ud == 1)
        ball.dir_ud = -1; 
      else  
        ball.dir_ud = 1;
      ball.x = 39;
      ball.y = 12;
    }
      usleep(100000);
  }
  reset_keypress();
  return(0);
}