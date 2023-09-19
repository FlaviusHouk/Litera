#ifndef _LITERA_APP_H_
#define _LITERA_APP_H_

#include <gtk/gtk.h>

#define LITERA_APP_TYPE (litera_app_get_type())
G_DECLARE_FINAL_TYPE (LiteraApp, litera_app, LITERA, APP, GtkApplication)

LiteraApp*  litera_app_new();

#endif //_LITERA_APP_H_
