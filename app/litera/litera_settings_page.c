#include "litera_settings_page.h"

#include "evernote-backend.h"

struct _LiteraSettingsPage {
	GtkApplicationWindow parent;
	
	GtkStack* stack;
	GtkEntry* evenoteBaseUrl;
	GtkButton* saveButton;

	State* state;
};

typedef enum {
	LITERA_SETTINGS_PAGE_PROPS_COUNT = 0
} LiteraSettingsPageProps;

static GParamSpec* props[LITERA_SETTINGS_PAGE_PROPS_COUNT] = { NULL, };

G_DEFINE_TYPE(LiteraSettingsPage, litera_settings_page, GTK_TYPE_WIDGET);

static void litera_settings_page_dispose(GObject* obj) {
	gtk_widget_dispose_template(GTK_WIDGET(obj), LITERA_SETTINGS_PAGE_TYPE);

	GtkWidget* child = gtk_widget_get_first_child(GTK_WIDGET(obj));
	do {
		gtk_widget_unparent(child);
		child = gtk_widget_get_first_child(GTK_WIDGET(obj));
	} while(child != NULL);

	G_OBJECT_CLASS(litera_settings_page_parent_class)->dispose(obj);
}

static void litera_settings_page_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* spec) {
	LiteraSettingsPage* this = LITERA_SETTINGS_PAGE(obj);

	switch ((LiteraSettingsPageProps)prop_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, spec);
			break;
	}
}

static void litera_settings_page_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* spec) {
	LiteraSettingsPage* this = LITERA_SETTINGS_PAGE(obj);

	switch ((LiteraSettingsPageProps)prop_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, spec);
			break;
	}
}

static void litera_settings_page_class_init(LiteraSettingsPageClass* class) {
	GObjectClass* object_class =  G_OBJECT_CLASS(class);
	
	object_class->dispose = litera_settings_page_dispose;
	object_class->set_property = litera_settings_page_set_property;
	object_class->get_property = litera_settings_page_get_property;

	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

	gtk_widget_class_set_template_from_resource (widget_class, "/litera/settings.ui");
	gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);

	gtk_widget_class_bind_template_child (widget_class, LiteraSettingsPage, stack);
	gtk_widget_class_bind_template_child (widget_class, LiteraSettingsPage, evenoteBaseUrl);
	gtk_widget_class_bind_template_child (widget_class, LiteraSettingsPage, saveButton);

	//g_object_class_install_properties(object_class, LITERA_SETTINGS_PAGE_PROPS_COUNT, props);
}

static void litera_settings_page_on_group_selection (GObject* obj, GParamSpec* spec, LiteraSettingsPage* page) {
	const gchar* name = gtk_stack_get_visible_child_name(page->stack);
	if(strcmp(name, "evernote_settings") == 0) {
		EvernoteConfig* config = evernote_get_config();
		GtkEntryBuffer* buffer = gtk_entry_get_buffer(page->evenoteBaseUrl);
		gtk_entry_buffer_set_text(buffer, config->baseUrl, -1);
	}
}

static void litera_settings_page_on_save (GtkButton* button, LiteraSettingsPage* page) {
	const gchar* name = gtk_stack_get_visible_child_name(page->stack);
	if(strcmp(name, "evernote_settings") == 0) {
		EvernoteConfig* config = evernote_get_config();
		GtkEntryBuffer* buffer = gtk_entry_get_buffer(page->evenoteBaseUrl);
		gchar* newBaseUrl = gtk_entry_buffer_get_text(buffer);
		strcpy(config->baseUrl, newBaseUrl);
	}
}

static void litera_settings_page_init(LiteraSettingsPage* widget) {
	gtk_widget_init_template(GTK_WIDGET(widget));

	widget->state = NULL;

	g_signal_connect(widget->stack, "notify::visible-child-name", G_CALLBACK(litera_settings_page_on_group_selection), widget);
	g_signal_connect(widget->saveButton, "clicked", G_CALLBACK(litera_settings_page_on_save), widget);
}

LiteraSettingsPage* litera_settings_page_new() {
	return g_object_new(LITERA_SETTINGS_PAGE_TYPE, NULL);
}

void litera_settings_page_set_state(LiteraSettingsPage* page, State* state) {
	g_assert(page->state == NULL);

	page->state = state;
}
