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
/**********************************************************************/


/**********************************************************************/
/* File Static Variables                                              */
/**********************************************************************/
static gint      endlev_counter = 0;
static gint      current_screen = 0;
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
static void draw_graphics();
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
 * draw_graphics
 *
 * Description:
 * Draws all of the game objects
 **/
static void draw_graphics(
){
  gint i, j;

  for(i = 0; i < NOOFROWS; i++){
    for(j = 0; j < ROWLEN; j++){
      draw_object(j+1, i+1, symbol(screen[i][j]));
    }
  }

  display_updated = TRUE;
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
void start_new_game() {
  int aaa;

  current_screen = 1;
  score = 0;
  kills = 0;
  score_step = 0;

  edit_mode = 0;

  if (rscreen (3, &aaa)) {
    inform_me("Error", 0);
  }

  init_struct();

  clear_game_area();

  draw_graphics();

  game_state = STATE_PLAYING;

  gnobots_statusbar_set(score, current_screen, safe_teleports, 1, 2);
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

  res = playscreen (dx, dy);

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
      case KBD_NW:
        if(player_move(-1, -1)){
	}
        break;
      case KBD_N:
        if(player_move(0, -1)){
	}
        break;
      case KBD_NE:
        if(player_move(1, -1)){
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
      case KBD_SW:
        if(player_move(-1, 1)){
	}
        break;
      case KBD_S:
        if(player_move(0, 1)){
	}
        break;
      case KBD_SE:
        if(player_move(1, 1)){
	}
        break;
      case KBD_TELE:
        break;
      case KBD_RTEL:
        break;
      case KBD_WAIT:
        game_state = STATE_WAITING;
        break;
    }
  }

}

/**********************************************************************/
