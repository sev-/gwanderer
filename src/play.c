#include <config.h>
#include <gnome.h>
#include "gwanderer.h"

extern int move_monsters();
extern void showname();
extern int jumpscreen();
extern int check();
extern void showpass();
extern void draw_symbol();
extern void display();
extern int fall();
extern void map();
extern void redraw_screen();
extern struct mon_rec *make_monster();

extern int debug_disp;
extern int edit_mode;
extern int saved_game;
extern int record_file;
extern char screen[NOOFROWS][ROWLEN+1];
extern char *edit_memory;
extern char *memory_end;

struct mon_rec start_of_list = {0,0,0,0,0,NULL,NULL};
struct mon_rec *last_of_list, *tail_of_list;

static struct {
  int x, y;     /* where you are */
  int diamonds; /* number of diamonds */
  int tx, ty;   /* teleport arrival */
  int sx, sy;   /* where the screen window on the playing area is */
  int mx, my;   /* where the monster is */
  int maxmoves; /* number of moves */
  int nf;       /* how many diamonds youve got so far */
} game_data;


/* process screen and initialise game data */
char *init_struct() {
  int x, y;
  struct mon_rec *monster;

  game_data.sx = -1;
  game_data.sy = -1;
  game_data.tx = -1;
  game_data.ty = -1;
  game_data.mx = -1;
  game_data.my = -1;
  game_data.diamonds = 0;
  game_data.nf = 0;

  tail_of_list = &start_of_list;

  for(x=0;x<=ROWLEN;x++)
    for(y=0;y<NOOFROWS;y++) {
      if((screen[y][x] == '*')||(screen[y][x] == '+'))
	game_data.diamonds++;
      if(screen[y][x] == 'A') {    /* note teleport arrival point &  */
	game_data.tx = x;                    /* replace with space */
	game_data.ty = y;
	screen[y][x] = ' ';
      }
      if(screen[y][x] == '@') {
	game_data.sx = x;
	game_data.sy = y;
      }
      if(screen[y][x] == 'M') {    /* Put megamonster in */
	game_data.mx = x;
	game_data.my = y;
      }
      if(screen[y][x] == 'S') {    /* link small monster to pointer chain */
	if((monster = make_monster(x,y)) == NULL) {
	  return ("running out of memory");
	}
	if(!viable(x,y-1)) {    /* make sure its running in the correct */
	  monster->mx = 1;      /* direction..                          */
	  monster->my = 0;
	} else if(!viable(x+1,y)) {
	  monster->mx = 0;
	  monster->my = 1;
	} else if(!viable(x,y+1)) {
	  monster->mx = -1;
	  monster->my = 0;
	} else if(!viable(x-1,y)) {
	  monster->mx = 0;
	  monster->my = -1;
	}
      }
      if(screen[y][x] == '-')
	screen[y][x] = ' ';
    };
  game_data.x=game_data.sx;
  game_data.y=game_data.sy;

  if((game_data.x == -1)&&(game_data.y == -1)) { /* no start position in screen ? */
    return("a screen design error");
  }

  if(game_data.maxmoves < 1) 
    game_data.maxmoves = -1;

  return (char *)NULL;
}


/* Actual game function - Calls fall() to move boulders and arrows
	recursively.
   Variable explaination :
	All the var names make sense to ME, but some people think them a bit confusing... :-) So heres an explanation.
   nx,ny : where you're trying to move to
   bx,by : baby monster position
   nbx,nby : where it wants to be
   lx,ly : the place you left when teleporting
   new_disp : the vector the baby monster is trying
*/
char *playscreen(gint dx, gint dy) {
  int  x,y,nx,ny,deadyet =0,
    lx = 0,ly = 0, mx, my, sx, sy;
  static char     howdead[25];	/* M001 can't use auto var for return value */

  mx = game_data.mx;
  my = game_data.my;
  sx = game_data.sx;
  sy = game_data.sy;
  x = game_data.x;
  y = game_data.y;
  nx = x + dx;
  ny = y + dy;

  if (nx >= ROWLEN || nx < 0 || ny >= NOOFROWS || ny < 0)
    return (char *)NULL;


  if(screen[ny][nx] == 'C') {
    screen[ny][nx] = ':';
    score+=4;
    if(game_data.maxmoves != -1)
      game_data.maxmoves+=250;
  }


  switch(screen[ny][nx]) {
    case '@': 
      break;
    case '*': 
      score+=9;
      game_data.nf++;
    case ':': 
      score+=1;
    case ' ':
      screen[y][x] = ' ';
      screen[ny][nx] = '@';
      deadyet += check(&mx,&my,x,y,nx-x,ny-y,sx,sy,howdead);
      y = ny;
      x = nx;
      break;

    case 'O':
      if((nx == 0)||(nx == ROWLEN)) break;
      if(screen[y][nx*2-x] == 'M') {
	screen[y][nx*2-x] = ' ';
	game_data.mx = game_data.my = -1;
	score+=100;
      }
      if(screen[y][nx*2-x] == ' ') {
	screen[y][nx*2-x] = 'O';
	screen[y][x] = ' ';
	screen[ny][nx] = '@';
	deadyet += fall(&mx,&my,nx*2-x,y+1,sx,sy,howdead);
	deadyet += fall(&mx,&my,x*2-nx,y,sx,sy,howdead);
	deadyet += fall(&mx,&my,x,y,sx,sy,howdead);
	deadyet += fall(&mx,&my,x,y-1,sx,sy,howdead);
	deadyet += fall(&mx,&my,x,y+1,sx,sy,howdead);
	y = ny;
	x = nx;
      }
      break;
    case '^':
      if((nx == 0 )||(nx == ROWLEN)) break;
      if(screen[y][nx*2-x] == ' ') {
	screen[y][nx*2-x] = '^';
	screen[y][x] = ' ';
	screen[ny][nx] = '@';
	deadyet += fall(&mx,&my,nx*2-x,y-1,sx,sy,howdead);
	deadyet += fall(&mx,&my,x*2-nx,y,sx,sy,howdead);
	deadyet += fall(&mx,&my,x,y,sx,sy,howdead);
	deadyet += fall(&mx,&my,x,y+1,sx,sy,howdead);
	deadyet += fall(&mx,&my,x,y-1,sx,sy,howdead);
	y = ny;
	x = nx;
      }
      break;
    case '<':
    case '>':
      if((ny == 0) || ( ny == NOOFROWS)) break;
      if(screen[ny*2-y][x] == 'M') {
	screen[ny*2-y][x] = ' ';
	game_data.mx = game_data.my = -1;
	score+=100;
      }
      if(screen[ny*2-y][x] == ' ') {
	screen[ny*2-y][x] = screen[ny][nx];
	screen[y][x] = ' ';
	screen[ny][nx] = '@';
	deadyet += fall(&mx,&my,x,y,sx,sy,howdead);
	deadyet += fall(&mx,&my,x-1,(ny>y)?y:(y-1),sx,sy,howdead);
	deadyet += fall(&mx,&my,x+1,(ny>y)?y:(y-1),sx,sy,howdead);
	deadyet += fall(&mx,&my,x-1,ny*2-y,sx,sy,howdead);
	deadyet += fall(&mx,&my,x+1,ny*2-y,sx,sy,howdead);
	y = ny;
	x = nx;
      }
      break;
    case '~':
      if(((2*nx-x) < 0) ||((ny*2-y) > NOOFROWS)||((ny*2-y) < 0)||((2*nx-x) > ROWLEN)) break;
      if(screen[ny*2-y][nx*2 -x] == 'M') {
	screen[ny*2-y][nx*2-x] = ' ';
	mx = my = -1;
	score+=100;
      }
      if(screen[ny*2-y][nx*2-x] == ' ') {
	screen[ny*2-y][nx*2-x] = '~';
	screen[y][x] = ' ';
	screen[ny][nx] = '@';
	deadyet += check(&mx,&my,x,y,nx-x,ny-y,sx,sy,howdead);
	y = ny;
	x = nx;
      }
      break;
    case '!':
      strcpy(howdead,"an exploding landmine");
      deadyet = 1;
      break;
    case 'X':
      if(game_data.nf == game_data.diamonds) {
	score+=250;
	return NULL;
      }
      break;
    case 'T':
      if(game_data.tx > -1) {
	screen[ny][nx] = ' ';
	screen[y][x] = ' ';
	lx = x;
	ly = y;
	y = game_data.ty;
	x = game_data.tx;
	screen[y][x] = '@';
	sx = x;
	sy = y;
	score += 20;
	deadyet += fall(&mx,&my,nx,ny,sx,sy,howdead);
	if(deadyet == 0)
	  deadyet = fall(&mx,&my,lx,ly,sx,sy,howdead);
	if(deadyet == 0)
	  deadyet = fall(&mx,&my,lx+1,ly-1,sx,sy,howdead);
	if(deadyet == 0)
	  deadyet = fall(&mx,&my,lx+1,ly+1,sx,sy,howdead);
	if(deadyet == 0)
	  deadyet = fall(&mx,&my,lx-1,ly+1,sx,sy,howdead);
	if(deadyet == 0)
	  deadyet = fall(&mx,&my,lx-1,ly-1,sx,sy,howdead);
      } else {
	screen[ny][nx] = ' ';
	message_box("Teleport out of order");
      }
      break;
    case 'M':
      strcpy(howdead,"a hungry monster");
      deadyet = 1;
      break;
    case 'S':
      strcpy(howdead,"walking into a monster");
      deadyet = 1;
      break;
    default:
      break;
  }

  if((y == ny) && (x == nx) && (game_data.maxmoves>0))
    game_data.maxmoves--;

  if(game_data.maxmoves == 0) {
    strcpy(howdead,"running out of time");
    deadyet = 1;
    if(edit_mode) game_data.maxmoves = -1;
  }
  
  if ((x<(sx-3))&& (deadyet ==0)) {    /* screen scrolling if necessary */
    sx-=6;
    if(sx < 4)
      sx = 4;
  }
  if ((y<(sy-2))&& (deadyet == 0)) {
    sy-=5;
    if(sy < 2)
      sy = 2;
  }
  if ((x>(sx+3)) && (!deadyet)) {
    sx+=6;
    if(sx>(ROWLEN -5))
      sx = ROWLEN -5;
  }
  if ((y>(sy+2))&& (!deadyet)) {
    sy+=5;
    if(sy > (NOOFROWS-3))
      sy = NOOFROWS -3;
  }

  deadyet += move_monsters(&mx,&my,score,howdead,sx,sy,game_data.nf,x,y,game_data.diamonds);

  if((y == ny) && (x == nx)) {
    game_data.x = nx;
    game_data.y = ny;
  }

  game_data.mx = mx;
  game_data.my = my;
  game_data.sx = sx;
  game_data.sy = sy;
  

  return(howdead);
}
