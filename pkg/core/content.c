#include "types.h"

#include <stdlib.h>

void litera_note_set_content(LiteraNote* note, DataPiece* content) {
	if(note->content != NULL) {
		for(int i = 0; note->content[i].type != DATA_PIECE_END; ++i) {
			DataPiece p = note->content[i];
			
			if(p.type == DATA_PIECE_TEXT) {
				TextPiece t = p.text;

				free(t.text);
			}
		}

		free(note->content);
	}

	note->content = content;
}
