#include <config.h>
#include <gnome.h>

#include "gbdefs.h"
#include "menu.h"
#include "game.h"
#include "gwanderer.h"


/**********************************************************************/
/* Function Prototypes                                                */
/**********************************************************************/
static void new_cb(GtkWidget *widget,gpointer data);
static void goto_cb(GtkWidget *widget,gpointer data);
static void properties_cb(GtkWidget *widget,gpointer data);
static void scores_cb(GtkWidget *widget,gpointer data);
void exit_cb(GtkWidget *widget,gpointer  data);
static void about_cb(GtkWidget *widget, gpointer data);
/**********************************************************************/


/**********************************************************************/
/* Menu entries                                                       */
/**********************************************************************/

/**********************************************************************/
/* Game menu entries                                                  */
/**********************************************************************/
GnomeUIInfo gamemenu[] = {
  GNOMEUIINFO_MENU_NEW_GAME_ITEM(new_cb, NULL),
  GNOMEUIINFO_SEPARATOR,
  { GNOME_APP_UI_ITEM, N_("Change screen..."), N_("Jump to another screen"),
                (gpointer) goto_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK,
                GNOME_STOCK_MENU_JUMP_TO, 0, 0, NULL },
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_MENU_SCORES_ITEM(scores_cb, NULL),
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_MENU_EXIT_ITEM(exit_cb, NULL),
  GNOMEUIINFO_END
};
/**********************************************************************/


/**********************************************************************/
/* Preferences menu entries                                           */
/**********************************************************************/
GnomeUIInfo prefmenu[] = {
  GNOMEUIINFO_MENU_PREFERENCES_ITEM(properties_cb, NULL),
  GNOMEUIINFO_END
};
/**********************************************************************/


/**********************************************************************/
/* Help menu entries                                                  */
/**********************************************************************/
GnomeUIInfo helpmenu[] = {
  GNOMEUIINFO_HELP(GAME_NAME),
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_MENU_ABOUT_ITEM(about_cb, NULL),
  GNOMEUIINFO_END
};
/**********************************************************************/


/**********************************************************************/
/* Main menu                                                          */
/**********************************************************************/
GnomeUIInfo mainmenu[] = {
  GNOMEUIINFO_MENU_GAME_TREE(gamemenu),
  GNOMEUIINFO_MENU_SETTINGS_TREE(prefmenu),
  GNOMEUIINFO_MENU_HELP_TREE(helpmenu),
  GNOMEUIINFO_END
};
/**********************************************************************/

/**********************************************************************/


/**********************************************************************/
/* Function Definitions                                               */
/**********************************************************************/

/**
 * really_new_cb
 * @widget: Pointer to widget
 * @data: Callback data
 *
 * Description:
 * Callback to really start new game
 *
 * Returns:
 **/
static void really_new_cb(
GtkWidget *widget,
gpointer  data
){
  gint button = GPOINTER_TO_INT(data);
    
  if(button != 0) return;

  start_new_game(1);
}


/**
 * new_cb
 * @widget: Pointer to widget
 * @data: Callback data
 *
 * Description:
 * Callback for new menu entry
 *
 * Returns:
 **/
static void new_cb(
GtkWidget *widget,
gpointer  data
){
  GtkWidget *box;

  if(game_state != STATE_NOT_PLAYING){
    box = gnome_message_box_new(_("Do you really want to start a new game?"),
				GNOME_MESSAGE_BOX_QUESTION,
				GNOME_STOCK_BUTTON_YES,
				GNOME_STOCK_BUTTON_NO,
				NULL);
    gnome_dialog_set_parent (GNOME_DIALOG(box), GTK_WINDOW(app));
    gnome_dialog_set_default (GNOME_DIALOG(box), 0);
    gtk_window_set_modal (GTK_WINDOW(box), TRUE);
    gtk_signal_connect (GTK_OBJECT(box), "clicked",
			GTK_SIGNAL_FUNC(really_new_cb), NULL);
    gtk_widget_show(box);
  } else {
    really_new_cb(widget, (gpointer)0);
  }
}

static void exec_goto_screen(GtkWidget *widget, gpointer data) {
  gint sc = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data));

  if (sc != current_screen) {
    start_new_game (sc);
  }
}


void close_window(GtkWidget *widget, gpointer data)
{
  gtk_widget_destroy(GTK_WIDGET(data));
}

/**
 * really_goto_cb
 * @widget: Pointer to widget
 * @data: Callback data
 *
 * Description:
 * Callback to really start new game with specified level
 *
 * Returns:
 **/
static void really_goto_cb(
GtkWidget *widget,
gpointer  data
){
  GtkWidget       *hbox;
  GtkWidget       *frame;
  GtkWidget       *label;
  GtkWidget       *spinbutton;
  GtkObject       *adjustment;
  GtkWidget       *level_window = NULL;

  gint button = GPOINTER_TO_INT(data);
    
  if(button != 0) return;

  level_window = gnome_dialog_new(_("Choose level"),
	      GNOME_STOCK_BUTTON_OK, GNOME_STOCK_BUTTON_CANCEL, NULL);
  gnome_dialog_set_parent(GNOME_DIALOG(level_window), GTK_WINDOW(app));
  gnome_dialog_set_default(GNOME_DIALOG(level_window), 0);

  frame = gtk_frame_new(NULL);
  gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(level_window)->vbox), frame,
                TRUE, TRUE, GNOME_PAD_SMALL);
  gtk_widget_show(frame);
        
  hbox = gtk_hbox_new(FALSE, GNOME_PAD);
  gtk_container_border_width(GTK_CONTAINER(hbox), GNOME_PAD_SMALL);
  gtk_container_add(GTK_CONTAINER(frame), hbox);
  gtk_widget_show(hbox);
        
  label = gtk_label_new(_("Screen:"));
  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, GNOME_PAD);
  gtk_widget_show(label);
        
  adjustment = gtk_adjustment_new(current_screen, 1, 60, 1, 2, 3);
  spinbutton = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1.0, 0);
  gtk_box_pack_start(GTK_BOX(hbox), spinbutton, FALSE, FALSE, GNOME_PAD);
  gtk_widget_show(spinbutton);

  gnome_dialog_button_connect(GNOME_DIALOG(level_window), 0,
			      exec_goto_screen, spinbutton);
  gnome_dialog_button_connect(GNOME_DIALOG(level_window), 1,
			      close_window, (gpointer) level_window);
  gtk_widget_show(level_window);
}


/**
 * goto_cb
 * @widget: Pointer to widget
 * @data: Callback data
 *
 * Description:
 * Callback for goto menu entry
 *
 * Returns:
 **/
static void goto_cb(
GtkWidget *widget,
gpointer  data
){
  GtkWidget *box;

  if(game_state != STATE_NOT_PLAYING){
    box = gnome_message_box_new(_("Do you really want to start a new game?"),
				GNOME_MESSAGE_BOX_QUESTION,
				GNOME_STOCK_BUTTON_YES,
				GNOME_STOCK_BUTTON_NO,
				NULL);
    gnome_dialog_set_parent (GNOME_DIALOG(box), GTK_WINDOW(app));
    gnome_dialog_set_default (GNOME_DIALOG(box), 0);
    gtk_window_set_modal (GTK_WINDOW(box), TRUE);
    gtk_signal_connect (GTK_OBJECT(box), "clicked",
			GTK_SIGNAL_FUNC(really_goto_cb), NULL);
    gtk_widget_show(box);
  } else {
    really_goto_cb(widget, (gpointer)0);
  }
}


/**
 * properties_cb
 * @widget: Pointer to widget
 * @data: Callback data
 *
 * Description:
 * Callback for properties menu entry
 *
 * Returns:
 **/
static void properties_cb(
GtkWidget *widget,
gpointer  data
){
  show_properties_dialog();
}


/**
 * scores_cb
 * @widget: Pointer to widget
 * @data: Callback data
 *
 * Description:
 * Callback for scores menu entry
 *
 * Returns:
 **/
static void scores_cb(
GtkWidget *widget,
gpointer  data
){
  show_scores(0);
}


/**
 * really_exit_cb
 * @widget: Pointer to widget
 * @data: Callback data
 *
 * Description:
 * Callback to really exit game
 *
 * Returns:
 **/
void really_exit_cb(
GtkWidget *widget,
gpointer  data
){
  gint button = GPOINTER_TO_INT(data);
    
  if(button != 0) return;

  gtk_main_quit();
}


/**
 * exit_cb
 * @widget: Pointer to widget
 * @data: Callback data
 *
 * Description:
 * Callback for exit menu entry
 *
 * Returns:
 **/
void exit_cb(
GtkWidget *widget,
gpointer  data
){
  GtkWidget *box;

  if(game_state != STATE_NOT_PLAYING){
    box = gnome_message_box_new(_("Do you really want to quit the game?"),
				GNOME_MESSAGE_BOX_QUESTION,
				GNOME_STOCK_BUTTON_YES,
				GNOME_STOCK_BUTTON_NO,
				NULL);
    gnome_dialog_set_parent(GNOME_DIALOG(box), GTK_WINDOW(app));
    gnome_dialog_set_default(GNOME_DIALOG(box), 0);
    gtk_window_set_modal(GTK_WINDOW(box), TRUE);
    gtk_signal_connect(GTK_OBJECT(box), "clicked",
		       GTK_SIGNAL_FUNC(really_exit_cb), NULL);
    gtk_widget_show(box);
  } else {
    really_exit_cb(widget, (gpointer)0);
  }
}


/**
 * about_cb
 * @widget: Pointer to widget
 * @data: Callback data
 *
 * Description:
 * Callback for about menu entry
 *
 * Returns:
 **/
static void about_cb(
GtkWidget *widget, 
gpointer data
){
  GtkWidget *about = NULL;
  
  const gchar *authors[]= {
    "Eugene Sandulenko <sev@interhack.net>",
    NULL
  };
  
  about = gnome_about_new(_("gwanderer"), VERSION,
                          "(C) 2001 Eugene Sandulenko",
                          authors,
                          _("Gnome Wanderer Game"),
                          NULL);
  gnome_dialog_set_parent(GNOME_DIALOG(about), GTK_WINDOW(app));
  gtk_window_set_modal(GTK_WINDOW(about), TRUE);

  gtk_widget_show(about);
}


/**
 * create_game_menus
 * @ap: application pointer
 *
 * Description:
 * Creates the menus for application @ap
 *
 * Returns:
 * %TRUE if successful, %FALSE otherwise
 **/
gboolean create_game_menus(
){
  gnome_app_create_menus(GNOME_APP(app), mainmenu);
  gnome_app_install_menu_hints(GNOME_APP(app), mainmenu);

  return TRUE;
}

/**********************************************************************/
