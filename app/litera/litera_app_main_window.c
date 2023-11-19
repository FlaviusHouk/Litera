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
	LiteraSettingsPage* settingsPage;

	GSimpleActionGroup* defaultGroup;
	AppPage pageStore[4];
	int pageStoreIdx; 

	State* state;

	gulong loginDevHandlerId;
};

G_DEFINE_TYPE(LiteraAppMainWindow, litera_app_main_window, GTK_TYPE_APPLICATION_WINDOW);

static void litera_app_main_window_open_settings(GSimpleAction* action, GVariant* param, gpointer userData) {
	litera_app_main_window_set_page(LITERA_APP_MAIN_WINDOW(userData), SETTINGS); 
}

static void litera_app_main_window_close(GSimpleAction* action, GVariant* param, gpointer userData) {
	gtk_window_close(GTK_WINDOW(userData));
}

static void litera_app_main_window_go_back (GSimpleAction* action, GVariant* param, gpointer userData) {
	LiteraAppMainWindow* win = LITERA_APP_MAIN_WINDOW(userData);

	//TODO: disable
	if(win->pageStoreIdx < 0) {
		return;
	}

	litera_app_main_window_set_page(win, win->pageStore[win->pageStoreIdx--]);
}

const GActionEntry custom_entries[] = {
	{ "settings", litera_app_main_window_open_settings, NULL, NULL, NULL },
	{ "back",     litera_app_main_window_go_back,       NULL, NULL, NULL }
};

const GActionEntry window_entries[] = {
	{ "close", litera_app_main_window_close, NULL, NULL, NULL },
};

static void litera_app_main_window_on_select_notebook(GObject* obj, GParamSpec* spec, LiteraAppMainWindow* win) {
	LiteraNotepadPage* page = LITERA_NOTEPAD_PAGE(obj);
	LiteraNotebook* selected = litera_notepad_page_get_selected_notebook(page);
	LiteraNote** notes = core_state_get_notes(win->state, selected);
	litera_notepad_page_set_notes(page, notes);
}

static void litera_app_main_window_on_note_change(GObject* obj, GParamSpec* spec, LiteraAppMainWindow* win) {
	LiteraNotepadPage* page = LITERA_NOTEPAD_PAGE(obj);
	LiteraNote* selected = litera_notepad_page_get_selected_note(page);
	if(selected == NULL) {
		return;
	}

	core_state_refresh_content(win->state, selected);
	litera_notepad_page_refresh(page);
}

static void litera_app_main_window_refresh_note(GObject* page, LiteraNote* selectedNote, LiteraAppMainWindow* win) {
	core_state_refresh_content(win->state, selectedNote);
	litera_notepad_page_refresh(LITERA_NOTEPAD_PAGE(page));
}

static void litera_app_main_window_save_note(GObject* page, LiteraNote* note, LiteraAppMainWindow* win) {
	core_state_save_content(win->state, note);
}

static void litera_app_main_window_on_dev_login(GObject* page, gchar* token, LiteraAppMainWindow* win) {
	core_state_login_dev(win->state, token);
	g_signal_handler_disconnect(page, win->loginDevHandlerId);

	LiteraNotebook** notebooks = core_state_get_notebooks(win->state);
	g_signal_connect(G_OBJECT(win->notepadPage), "notify::selected-notebook", G_CALLBACK(litera_app_main_window_on_select_notebook), win);
	g_signal_connect(G_OBJECT(win->notepadPage), "notify::selected-note", G_CALLBACK(litera_app_main_window_on_note_change), win);
	g_signal_connect(G_OBJECT(win->notepadPage), "refresh-note", G_CALLBACK(litera_app_main_window_refresh_note), win);
	g_signal_connect(G_OBJECT(win->notepadPage), "save-note", G_CALLBACK(litera_app_main_window_save_note), win);

	litera_notepad_page_set_notebooks(win->notepadPage, notebooks);

	litera_app_main_window_set_page(win, NOTEPAD);
}

static void litera_app_main_window_init (LiteraAppMainWindow* win) {
	gtk_widget_init_template(GTK_WIDGET(win));

	g_action_map_add_action_entries(G_ACTION_MAP(win), window_entries, 1, win);

	win->loginDevHandlerId = g_signal_connect(win->loginPage, "login-dev", G_CALLBACK(litera_app_main_window_on_dev_login), win);
	
	win->defaultGroup = g_simple_action_group_new();
	g_action_map_add_action_entries(G_ACTION_MAP(win->defaultGroup), custom_entries, 2, win);
	gtk_widget_insert_action_group(GTK_WIDGET(win->menuButton), "litera", G_ACTION_GROUP(win->defaultGroup));

	litera_app_main_window_set_page(win, LOGIN);
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

void    litera_app_main_window_set_page(LiteraAppMainWindow* win, AppPage page) {
	GtkBuilder* b = NULL; 
	GMenuModel* menu = NULL;

	win->pageStore[win->pageStoreIdx++] = win->page;

	switch(page) {
		case SETTINGS:
			b = gtk_builder_new_from_resource("/litera/settings_menu.ui");
			menu = G_MENU_MODEL(gtk_builder_get_object(b, "settings-menu"));
			gtk_stack_set_visible_child_name(win->rootPanel, "SettigsPage");
			break;
		case LOGIN:
			b = gtk_builder_new_from_resource("/litera/login_menu.ui");
			menu = G_MENU_MODEL(gtk_builder_get_object(b, "login-menu"));
			gtk_stack_set_visible_child_name(win->rootPanel, "LoginPage");
			break;
		case NOTEPAD:
			b = gtk_builder_new_from_resource("/litera/notepad_menu.ui");
			menu = G_MENU_MODEL(gtk_builder_get_object(b, "notepad-menu"));
			gtk_stack_set_visible_child_name(win->rootPanel, "NotepadPage");
			break;
	}
	
	win->page = page;

	gtk_menu_button_set_menu_model(win->menuButton, menu);

	g_object_unref(G_OBJECT(b));
}
