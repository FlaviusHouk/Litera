#include "litera_login_page.h"

struct _LiteraLoginPage {
	GtkWidget parent;
};

G_DEFINE_TYPE(LiteraLoginPage, litera_login_page, GTK_TYPE_WIDGET);

static void litera_login_page_init (LiteraLoginPage* page) {
	gtk_widget_init_template(GTK_WIDGET(page));
}

static void litera_login_page_dispose (GObject* obj) {
	gtk_widget_dispose_template(GTK_WIDGET(obj), LITERA_LOGIN_PAGE_TYPE);

	G_OBJECT_CLASS(litera_login_page_parent_class)->dispose(obj);
}

static void litera_login_page_class_init (LiteraLoginPageClass* class) {
	G_OBJECT_CLASS(class)->dispose = litera_login_page_dispose;

	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

	gtk_widget_class_set_template_from_resource (widget_class, "/litera/login_page.ui");
	gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

LiteraLoginPage* litera_login_page_new() {
	return g_object_new(LITERA_LOGIN_PAGE_TYPE, "hexpand", TRUE, NULL);
}
