#include <config.h>
#include <gnome.h>

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>

#include "graphics.h"
#include "gbdefs.h"
#include "gwanderer.h"


/**********************************************************************/
/* GraphicInfo Structure Definition                                   */
/**********************************************************************/
typedef struct _GraphicInfo{
  GString   *name;
  GdkPixmap *pixmap;
  GdkColor   bgcolor;
}GraphicInfo;
/**********************************************************************/


/**********************************************************************/
/* File Static Variables                                              */
/**********************************************************************/
static gint          num_graphics     = -1;
static gint          current_graphics = -1;
static GraphicInfo **game_graphic     = NULL;
static GdkPixmap    *wanderer_pixmap     = NULL;
static GdkPixmap    *wanderer_mask       = NULL;

static gint          robot_animation  = 0;
static gint          player_animation = 0;
static gint          player_num_waves = 0;
static gint          player_wave_wait = 0;
static gint          player_wave_dir  = 1;

static gint          bubble_xpos = 0;
static gint          bubble_ypos = 0;
static gint          bubble_xo   = 0;
static gint          bubble_yo   = 0;
static gint          bubble_type = BUBBLE_NONE;
/**********************************************************************/


/**********************************************************************/
/* Function Prototypes                                                */
/**********************************************************************/
static gboolean load_misc_graphic(gchar*, GdkPixmap**, GdkPixmap**);
static gboolean load_misc_graphics();
/**********************************************************************/


/**********************************************************************/
/* Function Definitions                                               */
/**********************************************************************/

/**
 * load_misc_graphic
 * @fname: pixmap filename
 * @pixmap: pointer to pixmap pointer
 * @mask: pointer to pixmap mask pointer
 *
 * Description:
 *
 * Returns:
 * TRUE on success FALSE otherwise
 **/
static gboolean load_misc_graphic(
gchar *fname,
GdkPixmap **pixmap,
GdkPixmap **mask
){
  GdkImlibImage *image;

  if(!g_file_exists(fname)){
    printf(_("Could not find \'%s\' pixmap file for gwanderer\n"), fname);
    return FALSE;
  }

  image = gdk_imlib_load_image(fname);
  gdk_imlib_render(image, image->rgb_width, image->rgb_height);
  *pixmap = gdk_imlib_move_image(image);
  *mask = gdk_imlib_move_mask(image);
  gdk_imlib_destroy_image(image);
  
  return TRUE;
}


/**
 * load_misc_graphics
 *
 * Description:
 * Loads all of the 'speech bubble' graphics
 *
 * Returns:
 * TRUE on success FALSE otherwise
 **/
static gboolean load_misc_graphics(
){
  gchar buffer[PATH_MAX];
  gchar *dname = gnome_unconditional_pixmap_file(GAME_NAME);

  strcpy(buffer, dname);
  strcat(buffer, "/");
  strcat(buffer, "wanderer.png");
  if(!load_misc_graphic(buffer, &wanderer_pixmap, &wanderer_mask)) return FALSE;

  return TRUE;
}


/**
 * load_game_graphics
 *
 * Description:
 * Loads all of the game graphics
 *
 * Returns:
 * TRUE on success FALSE otherwise
 **/
gboolean load_game_graphics(
){
  gint             i;
  struct dirent  *dent;
  DIR            *dir;
  gchar           buffer[PATH_MAX];
  gchar          *bptr;
  GdkImlibImage  *image;
  GdkImage       *tmpimage;
  GdkVisual      *visual;
  GdkPixmap      *pixmap;
  gchar          *dname = gnome_unconditional_pixmap_file(GAME_NAME);

  if(game_graphic != NULL){
    free_game_graphics();
  }

  dir = opendir(dname);
  if(!dir) return FALSE;

  num_graphics = 0;
  while((dent = readdir(dir)) != NULL){
    if(!strstr(dent->d_name, ".png")){
      continue;
    }
    num_graphics++;
  }

  game_graphic = g_new(GraphicInfo*, num_graphics);
  for(i = 0; i < num_graphics; ++i){
    game_graphic[i] = NULL;
  }

  rewinddir(dir);

  num_graphics = 0;
  while((dent = readdir(dir)) != NULL){
    if(!strstr(dent->d_name, ".png")){
      continue;
    }
    if(!strcmp(dent->d_name, "wanderer.png")){
      continue;
    }
    if(!strcmp(dent->d_name, "gwanderer-logo.png")){
      continue;
    }

    strcpy(buffer, dent->d_name);
    bptr = buffer;
    while(bptr){
      if(*bptr == '.'){
	*bptr = 0;
	break;
      }
      bptr++;
    }

    game_graphic[num_graphics] = g_new(GraphicInfo, 1);
    game_graphic[num_graphics]->name = g_string_new(buffer);

    
    strcpy(buffer, dname);
    strcat(buffer, "/");
    strcat(buffer, dent->d_name);

    image = gdk_imlib_load_image(buffer);
    visual = gdk_imlib_get_visual();
    if(visual->type != GDK_VISUAL_TRUE_COLOR){
        gdk_imlib_set_render_type(RT_PLAIN_PALETTE);
    }
    gdk_imlib_render(image, image->rgb_width, image->rgb_height);
    pixmap = gdk_imlib_move_image(image);
    tmpimage = gdk_image_get(pixmap, 0, 0, 1, 1);
    game_graphic[num_graphics]->bgcolor.pixel = gdk_image_get_pixel(tmpimage, 0, 0);
    gdk_image_destroy(tmpimage);
    gdk_imlib_destroy_image(image);

    game_graphic[num_graphics]->pixmap = pixmap;

    num_graphics++;
  }

  closedir(dir);

  current_graphics = 0;

  if(!load_misc_graphics()) return FALSE;

  return TRUE;
}


/**
 * free_game_graphics
 *
 * Description:
 * Frees all of the resources used by the game graphics
 *
 * Returns:
 * TRUE on success FALSE otherwise
 **/
gboolean free_game_graphics(
){
  gint i;

  if(game_graphic == NULL){
    return FALSE;
  }

  for(i = 0; i < num_graphics; ++i){
    g_free(game_graphic[i]);
  }
  g_free(game_graphic);

  game_graphic = NULL;
  num_graphics = -1;
  current_graphics = -1;

  if(wanderer_pixmap) gdk_pixmap_unref(wanderer_pixmap);
  wanderer_pixmap = NULL;
  if(wanderer_mask) gdk_pixmap_unref(wanderer_mask);
  wanderer_mask = NULL;

  return TRUE;
}


/**
 * num_game_graphics
 *
 * Description:
 * Returns the number of different graphics scenarios available
 *
 * Returns:
 * number of graphic types
 **/
gint num_game_graphics(
){
  if(game_graphic == NULL) return -1;

  return num_graphics;
}


/**
 * ganme_graphics_name
 * @n: game graphics number
 *
 * Description:
 * The descriptive name of game graphics number @n
 *
 * Returns:
 * a string containing the graphics name
 **/
gchar* game_graphics_name(
gint n
){
  if(game_graphic == NULL) return NULL;

  if((n < 0) || (n >= num_graphics)) return NULL;

  return game_graphic[n]->name->str;
}


/**
 * game_graphics_background
 * @n: game graphics number
 *
 * Description:
 * Returns the background colour for game graphics specified by @n
 *
 * Returns:
 * background colour
 **/
GdkColor game_graphics_background(
gint n
){
  static GdkColor nocol = {0, 0, 0, 0};

  if(game_graphic == NULL) return nocol;

  if((n < 0) || (n >= num_graphics)) return nocol;

  return game_graphic[n]->bgcolor;
}


/**
 * current_game_graphics
 *
 * Description:
 * returns the currently selected graphics
 *
 * Returns:
 * game graphics number
 **/
gint current_game_graphics(
){
  return current_graphics;
}


/**
 * set_game_graphics
 * @ng: Game graphics number
 *
 * Description:
 * Sets the game graphics to use
 **/
gint set_game_graphics(
gint ng
){
  if((ng < 0) || (ng >= num_graphics)) return -1;

  current_graphics = ng;

  if(game_area != NULL){
    gdk_window_set_background(game_area->window, &game_graphic[current_graphics]->bgcolor);
  }

  return current_graphics;
}


/**
 * draw_tile_pixmap
 * @tileno: Graphics tile number
 * @pno: Number of graphics set
 * @x: x position in pixels
 * @y: y position in pixels
 * @area: Pointer to drawing area widget
 *
 * Description:
 * Draws tile pixmap @tileno form graphics set @pno at (@x, @y) in
 * a widget @area
 **/
void draw_tile_pixmap(
gint         tileno,
gint         pno,
gint         x,
gint         y,
GtkWidget  *area
){
  gdk_draw_pixmap(area->window, area->style->black_gc,
		  game_graphic[pno]->pixmap, tileno*TILE_WIDTH, 
		  0, x, y, TILE_WIDTH, TILE_HEIGHT);
}


/**
 * draw_object
 * @x: x position 
 * @y: y position 
 * @type: object type
 *
 * Description:
 * Draws graphics for an object at specified location
 **/
void draw_object(
gint x,
gint y,
gint type
){
  gint xpos = x * TILE_WIDTH;
  gint ypos = y * TILE_HEIGHT;

  if((game_area == NULL) || (game_graphic == NULL)) return;

  draw_tile_pixmap(type, current_graphics, xpos, ypos, game_area);
}


/**
 * clear_game_area
 *
 * Description:
 * clears the whole of the game area
 **/
void clear_game_area(
){
  if((game_area == NULL) || (game_graphic == NULL)) return;
  
  gdk_window_clear_area(game_area->window, 0, 0, 
			GAME_WIDTH*TILE_WIDTH, GAME_HEIGHT*TILE_HEIGHT);
}


/**
 * clear_bubble_area
 *
 * Description:
 * clears the area underneath a bubble
 **/
static void clear_bubble_area(
){
  if((game_area == NULL) || (game_graphic == NULL)) return;
  
  gdk_window_clear_area(game_area->window, bubble_xpos, bubble_ypos, 
			BUBBLE_WIDTH, BUBBLE_HEIGHT);
}


/**
 * reset_player_animation
 *
 * Description:
 * resets player animation to standing position
 **/
void reset_player_animation(
){
  player_wave_wait = 0;
  player_num_waves = 0;
  player_wave_dir  = 1;
  player_animation = 0;
}


/**
 * player_animation_dead
 *
 * Description:
 * sets player animation to be dead
 **/
void player_animation_dead(
){
  player_wave_wait = 0;
  player_num_waves = 0;
  player_wave_dir  = 1;
  player_animation = NUM_PLAYER_ANIMATIONS;
}


/**
 * animate_game_graphics
 *
 * Description:
 * updates animation for object graphics
 **/
void animate_game_graphics(
){
  ++robot_animation;
  if(robot_animation >= NUM_ROBOT_ANIMATIONS){
    robot_animation = 0;
  }

  if(player_animation == NUM_PLAYER_ANIMATIONS){
    /* do nothing */
  } else if(player_wave_wait < PLAYER_WAVE_WAIT){
    ++player_wave_wait;
    player_animation = 0;
  } else {
    player_animation += player_wave_dir;
    if(player_animation >= NUM_PLAYER_ANIMATIONS){
      player_wave_dir = -1;
      player_animation -= 2;
    } else if(player_animation < 0){
      player_wave_dir = 1;
      player_animation = 1;
      ++player_num_waves;
      if(player_num_waves >= PLAYER_NUM_WAVES){
	reset_player_animation();
      }
    }
  }
}

gint inform_me(char *msg, gint qable) {
  printf ("%s\n", msg);

  return 0;
}

int symbol (char ch)
{
  switch (ch)
  {
    case ' ':			  /* space  */
      return (SPACE);
    case '#':			  /* rock  */
      return (WALL);
    case '<':			  /* arrows  */
      return (LARROW);
    case '>':
      return (RARROW);
    case 'O':			  /* boulder  */
      return (ROCK);
    case ':':			  /* earth  */
      return (DIRT);
    case '/':			  /* slopes */
      return (FWDSLIDE);
    case '\\':
      return (BACKSLIDE);
    case '*':			  /* diamond  */
      return (DIAMOND);
    case '=':			  /* brick  */
      return (BRICK);
    case '@':			  /* YOU!!! */
      return (PLAYER);
    case 'T':			  /* teleport  */
      return (TELEPORT);
    case 'X':			  /* exits  */
      return (WAYOUT);
    case '!':			  /* landmine  */
      return (LANDMINE);
    case 'M':			  /* big monster  */
      return (MONSTER);
    case 'S':			  /* baby monster */
      return (SPRITE);
    case '^':			  /* balloon */
      return (BALLOON);
    case 'C':			  /* time capsule */
      return (TIMECAPSULE);
    case '+':			  /* cage */
      return (CAGE);
    case 'A':			  /* teleport arrival */
      return (ARRIVAL);
  }

  /* this is what it uses if it doesn't */
  /* recognise the character  */
  return (WHOOPS);
}
