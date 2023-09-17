#include <gtk/gtk.h>

static void activate_application(GtkApplication* app, gpointer userData) {
	GtkWidget *window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "Litera");
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
	gtk_widget_show (window);
}

int main (int argc, char** argv) {
	int status;

	GtkApplication* app = gtk_application_new("org.lazlo.litera", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate_application), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}
