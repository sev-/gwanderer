/* gwanderer.c
 *
 * Copyright (C) 2001 Eugene Sandulenko
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <config.h>
#include <gnome.h>

#include "app.h"


static gint session_die(GnomeClient* client, gpointer client_data);

static gint save_session(GnomeClient *client, gint phase, 
                         GnomeSaveStyle save_style,
                         gint is_shutdown, GnomeInteractStyle interact_style,
                         gint is_fast, gpointer client_data);


/* poptable */
static int greet_mode = FALSE;
static char* message  = NULL;
static char* geometry = NULL;

struct poptOption options[] = {
  { 
    "geometry",
    '\0',
    POPT_ARG_STRING,
    &geometry,
    0,
    N_("Specify the geometry of the main window"),
    N_("GEOMETRY")
  },
  {
    NULL,
    '\0',
    0,
    NULL,
    0,
    NULL,
    NULL
  }
};

int 
main(int argc, char* argv[])
{
  GtkWidget* app;
  
  poptContext pctx;

  char** args;
  int i;

  GSList* greet = NULL;

  GnomeClient* client;

  bindtextdomain(PACKAGE, GNOMELOCALEDIR);  
  textdomain(PACKAGE);

  gnome_init_with_popt_table(PACKAGE, VERSION, argc, argv, 
                             options, 0, &pctx);  

  /* Argument parsing */

  args = poptGetArgs(pctx);

  if (greet_mode && args) {
    i = 0;
    while (args[i] != NULL) {
      greet = g_slist_prepend (greet, args[i]);
      i++;
    }
    /* Put them in order */
    greet = g_slist_reverse(greet); 
  } else if (greet_mode && args == NULL) {
    g_error(_("You must specify someone to greet."));
  } else if (args != NULL) {
    g_error(_("Command line arguments are only allowed with --greet."));
  } else { 
    g_assert(!greet_mode && args == NULL);
  }

  poptFreeContext(pctx);

  /* Session Management */
  client = gnome_master_client ();
  gtk_signal_connect (GTK_OBJECT (client), "save_yourself",
                      GTK_SIGNAL_FUNC (save_session), argv[0]);
  gtk_signal_connect (GTK_OBJECT (client), "die",
                      GTK_SIGNAL_FUNC (session_die), NULL);

  
  /* Main app */
  app = gwanderer_app_new(message, geometry, greet);

  g_slist_free(greet);

  gtk_widget_show_all(app);

  gtk_main();

  return 0;
}

static gint
save_session (GnomeClient *client, gint phase, GnomeSaveStyle save_style,
              gint is_shutdown, GnomeInteractStyle interact_style,
              gint is_fast, gpointer client_data)
{
  gchar** argv;
  guint argc;

  /* allocate 0-filled, so it will be NULL-terminated */
  argv = g_malloc0(sizeof(gchar*)*4);
  argc = 1;

  argv[0] = client_data;

  if (message) {
    argv[1] = "--message";
    argv[2] = message;
    argc = 3;
  }
  
  gnome_client_set_clone_command (client, argc, argv);
  gnome_client_set_restart_command (client, argc, argv);

  return TRUE;
}

static gint 
session_die(GnomeClient* client, gpointer client_data)
{
  gtk_main_quit ();
  return TRUE;
}
