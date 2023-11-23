#ifndef _CORE_COLLECTIONS_H_
#define _CORE_COLLECTIONS_H_

#include "types.h"
#include <stdbool.h>


#define DECLARE_ITERATOR(snakeCaseName, PascalCaseName, collectionType, dataType) \
typedef struct { \
	collectionType collection; \
	int currentIdx; \
	int initialLen; \
} PascalCaseName##Iterator; \
void           snakeCaseName##_iterator_reset (PascalCaseName##Iterator* iter); \
dataType       snakeCaseName##_iterator_get_current (PascalCaseName##Iterator* iter); \
bool           snakeCaseName##_iterator_move_next (PascalCaseName##Iterator* iter);

DECLARE_ITERATOR(litera_notebook_collection, LiteraNotebookCollection, LiteraNotebookCollection*, LiteraNotebook);
DECLARE_ITERATOR(litera_note_content, LiteraNoteContent, LiteraNoteContent*, DataPiece*);

#endif //_CORE_COLLECTIONS_H_
