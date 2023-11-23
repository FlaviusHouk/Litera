#include "litera_login_page.h"

struct _LiteraLoginPage {
	GtkWidget parent;

	GtkDropDown* loginTypeSelector;
	GtkStack* loginPresenter;
	GtkButton* loginButton;
	GtkPasswordEntry* devTokenEntry;
	gchar* devToken;
};

typedef enum {
	LITERA_LOGIN_PAGE_LOGIN_TYPE_PROP = 1,
	LITERA_LOGIN_PAGE_DEV_TOKEN_PROP,
	LITERA_LOGIN_PAGE_PROPS_COUNT
} LiteraLoginPageProps;

static GParamSpec* props[LITERA_LOGIN_PAGE_PROPS_COUNT] = { NULL, };

static guint login_dev_signal_id;
static guint login_signal_id;

G_DEFINE_TYPE(LiteraLoginPage, litera_login_page, GTK_TYPE_WIDGET);

static void litera_login_page_on_login(GtkButton* button, LiteraLoginPage* page) {
	const gchar* loginType = gtk_stack_get_visible_child_name(page->loginPresenter);

	if(g_strcmp0(loginType, "Developer") == 0) {
		g_signal_emit(page, login_dev_signal_id, 0, page->devToken);
	}
}

static void litera_login_page_init (LiteraLoginPage* page) {
	gtk_widget_init_template(GTK_WIDGET(page));

	g_object_bind_property(page->loginTypeSelector, "selected-item", page, "login-type", G_BINDING_DEFAULT);
	
	g_object_bind_property(page->devTokenEntry, "text", page, "dev-token", G_BINDING_DEFAULT);
	
	g_signal_connect(page->loginButton, "clicked", G_CALLBACK(litera_login_page_on_login), page);
}

static void litera_login_page_dispose (GObject* obj) {
	gtk_widget_dispose_template(GTK_WIDGET(obj), LITERA_LOGIN_PAGE_TYPE);

	GtkWidget* child = gtk_widget_get_first_child(GTK_WIDGET(obj));
	do {
		gtk_widget_unparent(child);
		child = gtk_widget_get_first_child(GTK_WIDGET(obj));
	} while(child != NULL);

	G_OBJECT_CLASS(litera_login_page_parent_class)->dispose(obj);
}

static void litera_login_page_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* spec) {
	LiteraLoginPage* this = LITERA_LOGIN_PAGE(obj);

	switch ((LiteraLoginPageProps)prop_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, spec);
			break;
	}
}

static void litera_login_page_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* spec) {
	LiteraLoginPage* this = LITERA_LOGIN_PAGE(obj);

	switch ((LiteraLoginPageProps)prop_id) {
		case LITERA_LOGIN_PAGE_LOGIN_TYPE_PROP:
			GtkStringObject* o = GTK_STRING_OBJECT(g_value_get_object(value));
			const gchar* loginType = gtk_string_object_get_string(o);

			gtk_stack_set_visible_child_name(this->loginPresenter, loginType);

			break;

		case LITERA_LOGIN_PAGE_DEV_TOKEN_PROP:
			g_free(this->devToken);
			this->devToken = g_value_dup_string(value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, spec);
			break;
	}
}

static void litera_login_page_class_init (LiteraLoginPageClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);
	
	object_class->dispose = litera_login_page_dispose;
	object_class->set_property = litera_login_page_set_property;
	object_class->get_property = litera_login_page_get_property;

	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

	gtk_widget_class_set_template_from_resource (widget_class, "/litera/login_page.ui");
	gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);

	gtk_widget_class_bind_template_child (widget_class, LiteraLoginPage, loginTypeSelector);
	gtk_widget_class_bind_template_child (widget_class, LiteraLoginPage, loginPresenter);
	gtk_widget_class_bind_template_child (widget_class, LiteraLoginPage, loginButton);
	gtk_widget_class_bind_template_child (widget_class, LiteraLoginPage, devTokenEntry);

	props[LITERA_LOGIN_PAGE_LOGIN_TYPE_PROP] = g_param_spec_object ("login-type", "Login type", "Defines how to perform login operation", G_TYPE_OBJECT, G_PARAM_WRITABLE);
	props[LITERA_LOGIN_PAGE_DEV_TOKEN_PROP] = g_param_spec_string("dev-token", "Developer token", "Developer token", NULL, G_PARAM_READWRITE);

	g_object_class_install_properties(object_class, LITERA_LOGIN_PAGE_PROPS_COUNT, props);

	login_dev_signal_id = g_signal_new("login-dev", G_TYPE_FROM_CLASS(class), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_STRING);
	login_signal_id = g_signal_new("login", G_TYPE_FROM_CLASS(class), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL, NULL, NULL, G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_STRING);
}

LiteraLoginPage* litera_login_page_new() {
	return g_object_new(LITERA_LOGIN_PAGE_TYPE, "hexpand", TRUE, NULL);
}
