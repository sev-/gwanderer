#include <config.h>
#include <gnome.h>

#include "statusbar.h"


/**********************************************************************/
/* File Static Variables                                              */
/**********************************************************************/
static GtkWidget *statusbar        = NULL;
static GtkWidget *sbtbl            = NULL;
static GtkWidget *score_area       = NULL;
static GtkWidget *score_label      = NULL;
static GtkWidget *found_label      = NULL;
static GtkWidget *total_label      = NULL;
static GtkWidget *level_label      = NULL;
static GtkWidget *remaining_label  = NULL;
static gboolean   show_both        = TRUE;
/**********************************************************************/


/**********************************************************************/
/* Function Definitions                                               */
/**********************************************************************/

/**
 * gwanderer_statusbar_new
 *
 * Description:
 * creates a new statusbar
 *
 * Returns:
 * a pointer to the statusbar or NULL on failure
 **/
GtkWidget* gwanderer_statusbar_new(
){
  GtkWidget *label;

  if(statusbar != NULL){
    return statusbar;
  }

  sbtbl = gtk_table_new(1, 14, FALSE);

  label = gtk_label_new(_("Score:"));
  gtk_table_attach(GTK_TABLE(sbtbl), label, 0, 1, 0, 1, 0, 0, 3, 3);
  gtk_widget_show(label);
    
  score_label = gtk_label_new("0");
  gtk_table_attach(GTK_TABLE(sbtbl), score_label, 1, 2, 0, 1, 0, 0, 3, 3);
  gtk_widget_show(score_label);

  gtk_table_set_col_spacing(GTK_TABLE(sbtbl), 2, 32);

  label = gtk_label_new(_("Found:"));
  gtk_table_attach(GTK_TABLE(sbtbl), label, 3, 4, 0, 1, 0, 0, 3, 3);
  gtk_widget_show(label);
    
  found_label = gtk_label_new("0");
  gtk_table_attach(GTK_TABLE(sbtbl), found_label, 4, 5, 0, 1, 0, 0, 3, 3);
  gtk_widget_show(found_label);
    
  gtk_table_set_col_spacing(GTK_TABLE(sbtbl), 5, 32);

  label = gtk_label_new(_("Total:"));
  gtk_table_attach(GTK_TABLE(sbtbl), label, 6, 7, 0, 1, 0, 0, 3, 3);
  gtk_widget_show(label);
    
  total_label = gtk_label_new("0");
  gtk_table_attach(GTK_TABLE(sbtbl), total_label, 7, 8, 0, 1, 0, 0, 3, 3);
  gtk_widget_show(total_label);
    
  gtk_table_set_col_spacing(GTK_TABLE(sbtbl), 8, 32);
    
  label = gtk_label_new(_("Level:"));
  gtk_table_attach(GTK_TABLE(sbtbl), label, 9, 10, 0, 1, 0, 0, 3, 3);
  gtk_widget_show(label);
    
  level_label = gtk_label_new("0");
  gtk_table_attach(GTK_TABLE(sbtbl), level_label, 10, 11, 0, 1, 0, 0, 3, 3);
  gtk_widget_show(level_label);

  gtk_table_set_col_spacing(GTK_TABLE(sbtbl), 11, 32);
    
  label = gtk_label_new(_("Moves:"));
  gtk_table_attach(GTK_TABLE(sbtbl), label, 12, 13, 0, 1, 0, 0, 3, 3);
  gtk_widget_show(label);
    
  remaining_label = gtk_label_new("0");
  gtk_table_attach(GTK_TABLE(sbtbl), remaining_label, 13, 14, 0, 1, 0, 0, 3, 3);
  gtk_widget_show(remaining_label);
    
  gtk_widget_show(sbtbl);


  statusbar = gnome_appbar_new(FALSE, TRUE, GNOME_PREFERENCES_USER);
  gtk_box_pack_start(GTK_BOX(statusbar), sbtbl, FALSE, FALSE, 0);

  show_both = TRUE;

  gwanderer_statusbar_reset();

  return statusbar;
}


/**
 * gwanderer_statusbar_delete
 *
 * Description:
 * destroys an existing statusbar
 *
 * Returns:
 * TRUE on success, FALSE otherwise
 **/
gboolean gwanderer_statusbar_delete(
){
  if(statusbar == NULL) return FALSE;

  gtk_widget_unref(statusbar);

  statusbar = FALSE;

  return TRUE;
}


/**
 * gwanderer_statusbar_set_score
 * @score: score
 *
 * Description:
 * sets the score on the statusbar
 *
 * Returns:
 * TRUE on success, FALSE otherwise
 **/
gboolean gwanderer_statusbar_set_score(
gint score				    
){
  gchar buffer[32];

  if(statusbar == NULL) return FALSE;

  sprintf(buffer, "%d", score);
  gtk_label_set(GTK_LABEL(score_label), buffer);
}


/**
 * gwanderer_statusbar_set_level
 * @level: level
 *
 * Description:
 * sets the level on the statusbar
 *
 * Returns:
 * TRUE on success, FALSE otherwise
 **/
gboolean gwanderer_statusbar_set_level(
gint level
){
  gchar buffer[32];

  if(statusbar == NULL) return FALSE;

  sprintf(buffer, "%d", level);
  gtk_label_set(GTK_LABEL(level_label), buffer);
}


/**
 * gwanderer_statusbar_set_found_diamonds
 * @fdim: found diamonds
 *
 * Description:
 * sets the number of found diamonds on the statusbar
 *
 * Returns:
 * TRUE on success, FALSE otherwise
 **/
gboolean gwanderer_statusbar_set_found_diamonds(
gint fdim
){
  gchar buffer[32];

  if(statusbar == NULL) return FALSE;

  sprintf(buffer, "%d", fdim);
  gtk_label_set(GTK_LABEL(found_label), buffer);
}


/**
 * gwanderer_statusbar_set_total_diamonds
 * @tdim: total diamonds
 *
 * Description:
 * sets the total number of diamonds on the statusbar
 *
 * Returns:
 * TRUE on success, FALSE otherwise
 **/
gboolean gwanderer_statusbar_set_total_diamonds(gint tdim) {
  gchar buffer[32];

  if(statusbar == NULL) return FALSE;

  sprintf(buffer, "%d", tdim);
  gtk_label_set(GTK_LABEL(total_label), buffer);
}


/**
 * gwanderer_statusbar_set_remaining
 * @rem1: remaining type1
 *
 * Description:
 * sets the number of remaining robots on the statusbar
 *
 * Returns:
 * TRUE on success, FALSE otherwise
 **/
gboolean gwanderer_statusbar_set_remaining(gint rem) {
  gchar buffer[32];

  if(statusbar == NULL) return FALSE;

  if (rem < 0) {
    strcpy (buffer, "Unlimited");
  } else {
    sprintf(buffer, "%d", rem);
  }

  gtk_label_set(GTK_LABEL(remaining_label), buffer);
}


/**
 * gwanderer_statusbar_set
 * @score: score
 * @level: level
 * @fdim: number of found diamonds
 * @tdim: total number of diamonds
 * @rem: remaining moves
 *
 * Description:
 * sets all of the values on the statusbar
 *
 * Returns:
 * TRUE on success, FALSE otherwise
 **/
gboolean gwanderer_statusbar_set(
gint score,
gint level,
gint fdim,
gint tdim,
gint rem
){
  if(statusbar == NULL) return FALSE;

  gwanderer_statusbar_set_score(score);
  gwanderer_statusbar_set_level(level);
  gwanderer_statusbar_set_found_diamonds(fdim);
  gwanderer_statusbar_set_total_diamonds(tdim);
  gwanderer_statusbar_set_remaining(rem);

  return TRUE;
}


/**
 * gwanderer_statusbar_reset
 *
 * Description:
 * resets all the values on the statusbar to 0
 *
 * Returns:
 * TRUE on success, FALSE otherwise
 **/
gboolean gwanderer_statusbar_reset(
){
  return gwanderer_statusbar_set(0, 0, 0, 0, 0);
}


/**
 * gwanderer_statusbar_show_both
 * @show: show remaining
 *
 * Description:
 * selects whether the statusbar should indicate different types of robots
 *
 * Returns:
 * TRUE on success, FALSE otherwise
 **/
gboolean gwanderer_statusbar_show_both(
gboolean show
){
  if(statusbar == NULL) return FALSE;

  show_both = show;

  return TRUE;
}

/**********************************************************************/
