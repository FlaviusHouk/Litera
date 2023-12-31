#ifndef _CORE_CONTENT_H_
#define _CORE_CONTENT_H_

#include "types.h"
#include "collections.h"

#include <stdbool.h>

LiteraNotebookCollection* litera_notebook_collection_new(int cap);
void                      litera_notebook_collection_add(LiteraNotebookCollection* c, LiteraNotebook notebook);
void                      litera_notebook_collection_iterate(LiteraNotebookCollection* c, LiteraNotebookCollectionIterator* iter); 
LiteraNotebook*           litera_notebook_collection_get(LiteraNotebookCollection* c, int idx);

void               litera_note_init_text_piece(DataPiece* piece, int len);

LiteraNoteContent* litera_note_create_content(int capacity);
void               litera_note_add_piece(LiteraNote* note, DataPiece piece);
DataPiece*         litera_note_add_text(LiteraNote* note, const char* text, int len);
void               litere_note_remove_piece(LiteraNote* note, DataPiece* piece);
void               litera_note_clear(LiteraNote* note);
void               litera_note_iterate(LiteraNote* note, LiteraNoteContentIterator* iter);
void               litera_note_free_content(LiteraNoteContent* content);

#endif //_CORE_CONTENT_H_
