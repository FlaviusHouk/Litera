#include "litera_app_main_window.h"
#include "state.h"
#include "evernote-backend.h"

#define GETTEXT_PACKAGE "gtk4"
#include <glib/gi18n-lib.h>

struct _LiteraApp {
	GtkApplication parent;
	State* state;
};

#define GPL_LICENCE "This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>." 

G_DEFINE_TYPE(LiteraApp, litera_app, GTK_TYPE_APPLICATION);

static void litera_app_about(GSimpleAction* action, GVariant* param, gpointer userData) {
	char *authors[2];
	authors[0] = "Vladyslav Stovmanenko";
	authors[1] = NULL;

	gtk_show_about_dialog (NULL, "program-name", "Litera", "license", GPL_LICENCE, "wrap-license", TRUE, "authors", authors, "comments", _("Application for managing notes stored in Evernote."), NULL);
}

const GActionEntry app_entries[] = {
	{ "about", litera_app_about, NULL, NULL, NULL },
};

static void litera_app_init (LiteraApp* app) {
	g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, 1, app);

	app->state = core_state_new();
}

static void litera_app_activate (GApplication* app) {	
	LiteraApp* this = LITERA_APP(app);
	Backend b = evernote_get_backend();
	core_state_set_backend(this->state, b);

	LiteraAppMainWindow* win = litera_app_main_window_new(LITERA_APP(app), this->state);
	gtk_window_present(GTK_WINDOW(win));
}

static void litera_app_class_init(LiteraAppClass* class) {
	G_APPLICATION_CLASS(class)->activate = litera_app_activate;
}

LiteraApp*  litera_app_new() {
	return g_object_new(LITERA_APP_TYPE, "application-id", "org.lazlo.litera", NULL);
}
