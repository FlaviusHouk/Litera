#ifndef _LITERA_NOTEPAD_PAGE_H_
#define _LITERA_NOTEPAD_PAGE_H_

#include "litera_app.h"
#include "types.h"

#define LITERA_NOTEPAD_PAGE_TYPE (litera_notepad_page_get_type())
G_DECLARE_FINAL_TYPE (LiteraNotepadPage, litera_notepad_page, LITERA, NOTEPAD_PAGE, GtkWidget)

LiteraNotepadPage*  litera_notepad_page_new();
void                litera_notepad_page_set_notebooks(LiteraNotepadPage* page, LiteraNotebook** notebooks);
LiteraNotebook*     litera_notepad_page_get_selected_notebook(LiteraNotepadPage* page);
void                litera_notepad_page_set_notes(LiteraNotepadPage* page, LiteraNote** notebooks);
LiteraNote*         litera_notepad_page_get_selected_note(LiteraNotepadPage* page);
void                litera_notepad_page_set_content(LiteraNotepadPage* page, DataPiece* content);

#endif //_LITERA_NOTEPAD_PAGE_H_
