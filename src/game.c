#include <config.h>
#include <gnome.h>
#include <stdlib.h>

#include "gbdefs.h"
#include "gameconfig.h"
#include "keyboard.h"
#include "game.h"
#include "gwanderer.h"
#include "sound.h"

/**********************************************************************/
/* Exported Variables                                                 */
/**********************************************************************/
gint         game_state = STATE_NOT_PLAYING;
gint         score      = 0;
gint         current_screen = 0;
/**********************************************************************/


/**********************************************************************/
/* File Static Variables                                              */
/**********************************************************************/
static gint      endlev_counter = 0;
static gint      kills          = 0;
static gint      score_step     = 0;
static gint      safe_teleports = 0;
static gboolean  display_updated = 0;
static gint      player_xpos    = 0;
static gint      player_ypos    = 0;
static gint      push_xpos      = -1;
static gint      push_ypos      = -1;
static gint      game_timer_id  = -1;
static gint      arena[GAME_WIDTH][GAME_HEIGHT];
static gint      old_arena[GAME_WIDTH][GAME_HEIGHT];
static gint      temp_arena[GAME_WIDTH][GAME_HEIGHT];
/**********************************************************************/

int edit_mode;
char *edit_screen;
char screen[NOOFROWS][ROWLEN+1];

char screen_name[61];



/**********************************************************************/
/* Function Prototypes                                                */
/**********************************************************************/
static guint log_score(gint sc);
static void add_kill(gint type);
static void clear_arena();
static void add_object(gint x, gint y, gint type);
static gint check_location(gint x, gint y);
static gboolean check_heap(gint x, gint y);
static gint timeout_cb(void *data);
static void destroy_game_timer();
static void create_game_timer();
static void signal_bad_move();
static gboolean push_heap(gint x, gint y, gint dx, gint dy);
static gboolean try_player_move(gint dx, gint dy);
static gboolean safe_move_available();
static gboolean player_move(gint dx, gint dy);
static gboolean random_teleport();
/**********************************************************************/


/**********************************************************************/
/* Function Definitions                                               */
/**********************************************************************/

/**
 * message_box
 * @msg: message
 *
 * Description:
 * Displays a modal dialog box with a given message
 **/
void message_box(gchar *msg) {
  GtkWidget *box;

  box = gnome_message_box_new(msg,
			      GNOME_MESSAGE_BOX_INFO, 
			      GNOME_STOCK_BUTTON_OK, NULL);

  gtk_window_set_modal(GTK_WINDOW(box), TRUE);
  gtk_widget_show(box);
}


/**
 * show_scores
 * @pos: score-table position
 *
 * Description:
 * Displays the high-score table
 **/
void show_scores(
guint pos
){
  gchar sbuf[256];
  gchar nbuf[256];

  if(properties_super_safe_moves()){
    sprintf(sbuf, "%s-super-safe", game_config_filename(current_game_config()));
  } else if(properties_safe_moves()){
    sprintf(sbuf, "%s-safe", game_config_filename(current_game_config()));
  } else {
    sprintf(sbuf, "%s", game_config_filename(current_game_config()));
  }

  if(properties_super_safe_moves()){
    sprintf(nbuf, _("'%s' with super-safe moves"), 
	    _(game_config_name(current_game_config())));
  } else if(properties_safe_moves()){
    sprintf(nbuf, _("'%s' with safe moves"), 
	    _(game_config_name(current_game_config())));
  } else {
    sprintf(nbuf, "'%s'", _(game_config_name(current_game_config())));
  }

  gnome_scores_display(nbuf, GAME_NAME, sbuf, pos);
}


/**
 * log_score
 * @sc: score
 *
 * Description:
 * Enters a score in the high-score table
 *
 * Returns:
 * the position in the high-score table
 **/
static guint log_score(
gint sc
){
  guint pos;
  gchar sbuf[256];

  if(properties_safe_moves()){
    sprintf(sbuf, "%s-safe", game_config_filename(current_game_config()));
  } else if(properties_super_safe_moves()){
    sprintf(sbuf, "%s-super-safe", game_config_filename(current_game_config()));
  } else {
    sprintf(sbuf, "%s", game_config_filename(current_game_config()));
  }

  pos = gnome_score_log((gfloat)sc, sbuf, TRUE);

  return pos;
}


/**
 * timeout_cb
 * @data: callback data
 *
 * Description:
 * Game timer callback function
 **/
static gint timeout_cb(
void *data
){
  gint sp;

  draw_graphics();

  return TRUE;
}


/**
 * destroy_game_timer
 *
 * Description:
 * Destroys the game timer
 **/
static void destroy_game_timer(
){
  if(game_timer_id != -1){
    gtk_timeout_remove(game_timer_id);
    game_timer_id = -1;
  }
}


/**
 * create_game_timer
 *
 * Description:
 * Creates the game timer
 **/
static void create_game_timer(
){
  if(game_timer_id != -1){
    destroy_game_timer();
  }

  game_timer_id = gtk_timeout_add(ANIMATION_DELAY, timeout_cb, 0);
}


/**
 * init_game
 *
 * Description:
 * Initialises everything when game first starts up
 **/
void init_game() {
  clear_game_area();

  /* Now there is no animation, but it is a TODO, also use of
     game timer eliminates need of redraw control */
  create_game_timer(); 

  gtk_signal_connect(GTK_OBJECT(app), "key_press_event",
		     GTK_SIGNAL_FUNC(keyboard_cb), 0);  
}


/**
 * cleanup_game
 *
 * Description:
 * cleans up all of game resources
 **/
void cleanup_game() {
  destroy_game_timer();
}


/**
 * start_new_game
 *
 * Description:
 * Initialises everything needed to start a new game
 **/
void start_new_game(int screen) {
  int maxmoves;

  current_screen = screen;
  score = 0;
  kills = 0;
  score_step = 0;

  edit_mode = 0;

  if (rscreen (current_screen, &maxmoves)) {
    message_box("Error reading screen");
  }

  init_struct(maxmoves);

  clear_game_area();

  draw_graphics();

  game_state = STATE_PLAYING;
}


/**
 * signal_bad_move
 *
 * Description:
 * Makes a sound when a bad move is made
 **/
static void signal_bad_move() {
  play_sound(SOUND_BAD);
}



/**
 * player_move
 * @dx: x direction
 * @dy: y direction
 *
 * Description:
 * moves the player in a given direction
 *
 * Returns:
 * TRUE if the player can move, FALSE otherwise
 **/
static gboolean player_move(gint dx, gint dy) {
  char *res;
  int maxmoves;

  res = playscreen (dx, dy);

  if (res == NULL) {
    current_screen++;

    if (rscreen (current_screen, &maxmoves)) {
      message_box("Error reading screen");
    }

    init_struct(maxmoves);

    clear_game_area();

    draw_graphics();
    return TRUE;
  }

  if (*res != '\0') {
    message_box (res);
    game_state = STATE_DEAD;
  }

  return TRUE;
}

/**
 * game_keypress
 * @key: keycode
 *
 * Description:
 * handles keyboard commands
 **/
void game_keypress(gint key) {
  if(!display_updated) return;

  if(game_state == STATE_PLAYING){
    switch(key){
      case KBD_N:
        if(player_move(0, -1)){
	}
        break;
      case KBD_W:
        if(player_move(-1, 0)){
	}
        break;
      case KBD_STAY:
        if(player_move(0, 0)){
	}
        break;
      case KBD_E:
        if(player_move(1, 0)){
	}
        break;
      case KBD_S:
        if(player_move(0, 1)){
	}
        break;
    }
  }

}

/**********************************************************************/

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


/**
 * draw_graphics
 *
 * Description:
 * Draws all of the game objects
 **/
void draw_graphics(
){
  gint i, j;

  for(i = 0; i < NOOFROWS; i++){
    for(j = 0; j < ROWLEN; j++){
      draw_object(j+1, i+1, symbol(screen[i][j]));
    }
  }

  gwanderer_statusbar_set(score, current_screen, game_data.nf, 
			  game_data.diamonds, game_data.maxmoves);

  display_updated = TRUE;
}


/* process screen and initialise game data */
char *init_struct(int maxmoves) {
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
  game_data.maxmoves = maxmoves;

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

  *howdead = '\0';

  mx = game_data.mx;
  my = game_data.my;
  sx = game_data.sx;
  sy = game_data.sy;
  x = game_data.x;
  y = game_data.y;
  nx = x + dx;
  ny = y + dy;

  if (nx >= ROWLEN || nx < 0 || ny >= NOOFROWS || ny < 0)
    return "";


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

  deadyet += move_monsters(&mx,&my,&score,howdead,sx,sy,game_data.nf,x,y,game_data.diamonds);

  game_data.x = x;
  game_data.y = y;
  game_data.mx = mx;
  game_data.my = my;
  game_data.sx = sx;
  game_data.sy = sy;
  

  return(howdead);
}
