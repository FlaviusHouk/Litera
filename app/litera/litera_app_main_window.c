#include "litera_app_main_window.h"

struct _LiteraAppMainWindow {
	GtkApplicationWindow parent;
};

G_DEFINE_TYPE(LiteraAppMainWindow, litera_app_main_window, GTK_TYPE_APPLICATION_WINDOW);

static void litera_app_main_window_init (LiteraAppMainWindow* win) {
	gtk_widget_init_template(GTK_WIDGET(win));
}

static void litera_app_main_window_class_init (LiteraAppMainWindowClass* class) {
	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

	gtk_widget_class_set_template_from_resource (widget_class, "/litera/main_window.ui");
}

LiteraAppMainWindow* litera_app_main_window_new(LiteraApp* app) {
	return g_object_new(LITERA_APP_MAIN_WINDOW_TYPE, "application", app, NULL);
}
