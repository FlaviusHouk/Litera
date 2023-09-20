#ifndef _LITERA_LOGIN_PAGE_H_
#define _LITERA_LOGIN_PAGE_H_

#include <gtk/gtk.h>

#define LITERA_LOGIN_PAGE_TYPE (litera_login_page_get_type())
G_DECLARE_FINAL_TYPE (LiteraLoginPage, litera_login_page, LITERA, LOGIN_PAGE, GtkWidget)

LiteraLoginPage* litera_login_page_new();

#endif //_LITERA_LOGIN_PAGE_H_
