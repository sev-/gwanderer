#ifndef STATUSBAR_H
#define STATUSBAR_H


/**********************************************************************/
/* Exported functions                                                 */
/**********************************************************************/
GtkWidget* gwanderer_statusbar_new();
gboolean   gwanderer_statusbar_delete();
gboolean   gwanderer_statusbar_set_score(gint);
gboolean   gwanderer_statusbar_set_level(gint);
gboolean   gwanderer_statusbar_set_found_diamonds(gint);
gboolean   gwanderer_statusbar_set_total_diamonds(gint);
gboolean   gwanderer_statusbar_set_remaining(gint);
gboolean   gwanderer_statusbar_set(gint, gint, gint, gint, gint);
gboolean   gwanderer_statusbar_reset();
gboolean   gwanderer_statusbar_show_both(gboolean);
/**********************************************************************/


#endif /* STATUSBAR_H */
