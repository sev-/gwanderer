#ifndef GWANDERER_H
#define GWANDERER_H


/**********************************************************************/
/* Exported Variables                                                 */
/**********************************************************************/
extern GtkWidget *app;
extern GtkWidget *game_area;
/**********************************************************************/


#define SCREENPATH "../screens"

/* I wouldnt change these if I were you - it wont give you a bigger screen */
#define ROWLEN 40
#define NOOFROWS 16

extern int edit_mode;
extern char *edit_screen;
extern char screen[NOOFROWS][ROWLEN+1];

extern char screen_name[61];

extern gint score;

char *playscreen(gint dx, gint dy);
void message_box(gchar *msg);


/* Save and Restore game additions (M002) by Gregory H. Margo	*/
/* mon_rec structure needed by save.c */
struct mon_rec
    {
    int x,y,mx,my;
    char under;
    struct mon_rec *next,*prev;
    };

struct	save_vars	{
	int	z_x, z_y,
		z_sx, z_sy,
		z_tx, z_ty,
		z_mx, z_my,
		z_diamonds,
		z_nf;
};

/* for monster movement */

#define viable(x,y) (((screen[y][x] == ' ') || (screen[y][x] == ':') ||\
	(screen[y][x] == '@') || (screen[y][x] == '+') ||\
	(screen[y][x] == 'S')) && (y >= 0) &&\
	(x >= 0) && (y < NOOFROWS) && (x < ROWLEN))

#endif /* GWANDERER_H */
