#ifndef _LITERA_SETTINGS_PAGE_H_
#define _LITERA_SETTINGS_PAGE_H_

#include "litera_app.h"

#define LITERA_SETTINGS_PAGE_TYPE (litera_settings_page_get_type())
G_DECLARE_FINAL_TYPE (LiteraSettingsPage, litera_settings_page, LITERA, SETTINGS_PAGE, GtkWidget)

LiteraSettingsPage* litera_settings_page_new();
gchar*              litera_settings_page_get_dev_token(LiteraSettingsPage* page);

#endif //_LITERA_SETTINGS_PAGE_H_
