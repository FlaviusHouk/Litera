#include "litera_app_main_window.h"
#include "litera_login_page.h"

struct _LiteraAppMainWindow {
	GtkApplicationWindow parent;
	AppPage page;
	GtkBox* rootPanel;
	GtkWidget* currentPageView;
};

G_DEFINE_TYPE(LiteraAppMainWindow, litera_app_main_window, GTK_TYPE_APPLICATION_WINDOW);

static void litera_app_main_window_init (LiteraAppMainWindow* win) {
	gtk_widget_init_template(GTK_WIDGET(win));

	win->currentPageView = NULL;
}

static void litera_app_main_window_dispose (GObject* obj) {
	gtk_widget_dispose_template(GTK_WIDGET(obj), LITERA_APP_MAIN_WINDOW_TYPE);

	G_OBJECT_CLASS(litera_app_main_window_parent_class)->dispose(obj);
}

static void litera_app_main_window_class_init (LiteraAppMainWindowClass* class) {
	G_OBJECT_CLASS(class)->dispose = litera_app_main_window_dispose;

	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

	gtk_widget_class_set_template_from_resource (widget_class, "/litera/main_window.ui");

	gtk_widget_class_bind_template_child (widget_class, LiteraAppMainWindow, rootPanel);
}

LiteraAppMainWindow* litera_app_main_window_new(LiteraApp* app) {
	return g_object_new(LITERA_APP_MAIN_WINDOW_TYPE, "application", app, NULL);
}

void litera_app_main_window_set_page(LiteraAppMainWindow* win, AppPage page) {
	if(win->page == page && win->currentPageView != NULL) {
		return;
	}

	win->page = page;
	if(win->currentPageView) {
		gtk_box_remove(win->rootPanel, GTK_WIDGET(win->currentPageView));
	}

	if(page == LOGIN) {
		win->currentPageView = GTK_WIDGET(litera_login_page_new());
	}

	gtk_box_append(win->rootPanel, win->currentPageView);
}

AppPage litera_app_main_window_get_page(LiteraAppMainWindow* app) {
	return app->page;
}
