#include "litera_app_main_window.h"
#include "litera_login_page.h"
#include "litera_settings_page.h"
#include "litera_notepad_page.h"

struct _LiteraAppMainWindow {
	GtkApplicationWindow parent;
	AppPage page;
	GtkStack* rootPanel;
	GtkMenuButton* menuButton;
	LiteraLoginPage* loginPage;
	LiteraNotepadPage* notepadPage;
	State* state;

	gulong loginDevHandlerId;
};

G_DEFINE_TYPE(LiteraAppMainWindow, litera_app_main_window, GTK_TYPE_APPLICATION_WINDOW);

static void litera_app_main_window_on_settings_button_click(GtkButton* button, gpointer userData) {
	LiteraAppMainWindow* win = (LiteraAppMainWindow*)userData;
}

static void litera_app_main_window_on_select_notebook(GObject* obj, GParamSpec* spec, LiteraAppMainWindow* win) {
	LiteraNotepadPage* page = LITERA_NOTEPAD_PAGE(obj);
	LiteraNotebook* selected = litera_notepad_page_get_selected_notebook(page);
    LiteraNote** notes = core_state_get_notes(win->state, selected);
	litera_notepad_page_set_notes(page, notes);
}

static void litera_app_main_window_on_note_change(GObject* obj, GParamSpec* spec, LiteraAppMainWindow* win) {
	LiteraNotepadPage* page = LITERA_NOTEPAD_PAGE(obj);
    LiteraNote* selected = litera_notepad_page_get_selected_note(page);
	gchar* content = core_state_get_content(win->state, selected);
	litera_notepad_page_set_content(page, content);
}

static void litera_app_main_window_on_dev_login(GObject* page, gchar* token, LiteraAppMainWindow* win) {
	LiteraUser* user = core_state_login_dev(win->state, token);
	g_signal_handler_disconnect(page, win->loginDevHandlerId);

    LiteraNotebook** notebooks = core_state_get_notebooks(win->state);
	litera_notepad_page_set_notebooks(win->notepadPage, notebooks);
	g_signal_connect(G_OBJECT(win->notepadPage), "notify::selected-notebook", G_CALLBACK(litera_app_main_window_on_select_notebook), win);
	g_signal_connect(G_OBJECT(win->notepadPage), "notify::selected-note", G_CALLBACK(litera_app_main_window_on_note_change), win);

	gtk_stack_set_visible_child_name(win->rootPanel, "NotepadPage");
}

static void litera_app_main_window_init (LiteraAppMainWindow* win) {
	gtk_widget_init_template(GTK_WIDGET(win));

	win->loginDevHandlerId = g_signal_connect(win->loginPage, "login-dev", G_CALLBACK(litera_app_main_window_on_dev_login), win);
	
	GtkBuilder* b = gtk_builder_new_from_resource("/litera/app_menu.ui");
	GMenuModel* menu = G_MENU_MODEL(gtk_builder_get_object(b, "app-menu"));

	gtk_menu_button_set_menu_model(win->menuButton, menu);
	g_object_unref(G_OBJECT(b));
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
	gtk_widget_class_bind_template_child (widget_class, LiteraAppMainWindow, loginPage);
	gtk_widget_class_bind_template_child (widget_class, LiteraAppMainWindow, notepadPage);
	gtk_widget_class_bind_template_child (widget_class, LiteraAppMainWindow, menuButton);
}

LiteraAppMainWindow* litera_app_main_window_new(LiteraApp* app, State* state) {
	//TODO: use property
	LiteraAppMainWindow* this = LITERA_APP_MAIN_WINDOW(g_object_new(LITERA_APP_MAIN_WINDOW_TYPE, "application", app, NULL));

	this->state = state;

	return this;
}

AppPage litera_app_main_window_get_page(LiteraAppMainWindow* app) {
	return app->page;
}
