#ifndef GRAPHICS_H
#define GRAPHICS_H


/**********************************************************************/
/* Exported functions                                                 */
/**********************************************************************/
gboolean  load_game_graphics();
gboolean  free_game_graphics();
gint      num_game_graphics();
gchar*    game_graphics_name(gint);
GdkColor  game_graphics_background(gint);
gint      current_game_graphics();
gint      set_game_graphics(gint);

void      draw_tile_pixmap(gint, gint, gint, gint, GtkWidget*);
void      draw_object(gint, gint, gint);
void      clear_game_area();
void      animate_game_graphics();
void      reset_player_animation();
void      player_animation_dead();
/**********************************************************************/

#define	ARRIVAL		0
#define	BACKSLIDE	1
#define	BALLOON		2
#define	BRICK		3
#define	CAGE		4
#define	DIAMOND		5
#define	DIRT		6
#define	FWDSLIDE	7
#define	LANDMINE	8
#define	LARROW		9
#define	MONSTER		10
#define	PLAYER		11
#define	RARROW		12
#define	ROCK		13
#define	SPACE		14
#define	SPRITE		15
#define	TELEPORT	16
#define	TIMECAPSULE	17
#define	WALL		18
#define	WAYOUT		19
#define	WHOOPS		20

#endif /* GRAPHICS_H */
