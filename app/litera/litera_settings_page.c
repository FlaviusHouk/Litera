#include "litera_settings_page.h"

struct _LiteraSettingsPage {
	GtkApplicationWindow parent;
	
	/*Developer settings*/
	GtkEntry* devEntry;
	gchar* dev_token;
	GBinding* devTokenBinding;
};

typedef enum {
	LITERA_SETTINGS_PAGE_DEV_TOKEN_PROP = 1,
	LITERA_SETTINGS_PAGE_PROPS_COUNT
} LiteraSettingsPageProps;

static GParamSpec* props[LITERA_SETTINGS_PAGE_PROPS_COUNT] = { NULL, };

G_DEFINE_TYPE(LiteraSettingsPage, litera_settings_page, GTK_TYPE_WIDGET);

static void litera_settings_page_dispose(GObject* obj) {
	gtk_widget_dispose_template(GTK_WIDGET(obj), LITERA_SETTINGS_PAGE_TYPE);

	LiteraSettingsPage* this = LITERA_SETTINGS_PAGE(obj);
	g_object_unref(G_OBJECT(this->devTokenBinding));

	G_OBJECT_CLASS(litera_settings_page_parent_class)->dispose(obj);
}

static void litera_settings_page_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* spec) {
	LiteraSettingsPage* this = LITERA_SETTINGS_PAGE(obj);

	switch ((LiteraSettingsPageProps)prop_id) {
		case LITERA_SETTINGS_PAGE_DEV_TOKEN_PROP:
		    g_value_set_string(value, this->dev_token);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, spec);
			break;
	}
}

static void litera_settings_page_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* spec) {
	LiteraSettingsPage* this = LITERA_SETTINGS_PAGE(obj);

	switch ((LiteraSettingsPageProps)prop_id) {
		case LITERA_SETTINGS_PAGE_DEV_TOKEN_PROP:
			g_free(this->dev_token);
			this->dev_token = g_value_dup_string(value);
			break;

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

	gtk_widget_class_bind_template_child (widget_class, LiteraSettingsPage, devEntry);

	props[LITERA_SETTINGS_PAGE_DEV_TOKEN_PROP] = g_param_spec_string ("dev-token", "Developer token", "Developer token that might be used for the authentication", NULL, G_PARAM_READWRITE);

	g_object_class_install_properties(object_class, LITERA_SETTINGS_PAGE_PROPS_COUNT, props);
}

static void litera_settings_page_init(LiteraSettingsPage* widget) {
	gtk_widget_init_template(GTK_WIDGET(widget));

	GtkEntryBuffer* entryBuffer = gtk_entry_get_buffer(widget->devEntry);
	g_object_bind_property(entryBuffer, "text", widget, "dev-token", G_BINDING_DEFAULT);
}

gchar* litera_settings_page_get_dev_token(LiteraSettingsPage* page) {
	return page->dev_token;
}

LiteraSettingsPage* litera_settings_page_new() {
	return g_object_new(LITERA_SETTINGS_PAGE_TYPE, NULL);
}
