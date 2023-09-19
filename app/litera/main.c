#include "litera_app.h"

int main (int argc, char** argv) {
	int status;

	LiteraApp* app = litera_app_new();
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}
