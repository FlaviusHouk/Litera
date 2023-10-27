#ifndef _LITERA_APP_MAIN_WINDOW_H_
#define _LITERA_APP_MAIN_WINDOW_H_

#include "litera_app.h"
#include "state.h"

#define LITERA_APP_MAIN_WINDOW_TYPE (litera_app_main_window_get_type())
G_DECLARE_FINAL_TYPE (LiteraAppMainWindow, litera_app_main_window, LITERA, APP_MAIN_WINDOW, GtkApplicationWindow)

LiteraAppMainWindow* litera_app_main_window_new(LiteraApp* app, State* state);
AppPage              litera_app_main_window_get_page(LiteraAppMainWindow* win);

#endif //_LITERA_APP_MAIN_WINDOW_H_
