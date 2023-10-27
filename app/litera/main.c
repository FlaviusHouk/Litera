#include "litera_app.h"
#include "litera_login_page.h"
#include "litera_settings_page.h"
#include "litera_notepad_page.h"

int main (int argc, char** argv) {
	int status;

	LiteraApp* app = litera_app_new();
	
	g_type_ensure(LITERA_LOGIN_PAGE_TYPE);
	g_type_ensure(LITERA_SETTINGS_PAGE_TYPE);
	g_type_ensure(LITERA_NOTEPAD_PAGE_TYPE);

	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}
