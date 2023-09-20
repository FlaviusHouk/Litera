#include "litera_app_main_window.h"
#include "state.h"

struct _LiteraApp {
	GtkApplication parent;
	State* state;
};

G_DEFINE_TYPE(LiteraApp, litera_app, GTK_TYPE_APPLICATION);

static void litera_app_init (LiteraApp* app) {
	app->state = core_state_new();
}

static void litera_app_activate (GApplication* app) {
	LiteraAppMainWindow* win = litera_app_main_window_new(LITERA_APP(app));
    litera_app_main_window_set_page (win, LOGIN);
	gtk_window_present(GTK_WINDOW(win));
}

static void litera_app_class_init(LiteraAppClass* class) {
	G_APPLICATION_CLASS(class)->activate = litera_app_activate;
}

LiteraApp*  litera_app_new() {
	return g_object_new(LITERA_APP_TYPE, "application-id", "org.lazlo.litera", NULL);
}
