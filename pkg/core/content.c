#include "content.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h> //TODO: proper error handling

struct litera_note_content_t {
	DataPiece* buffer;
	int len;
	int cap;
}; 

/* Iterator */
void       litera_note_content_iterator_reset(LiteraNoteContentIterator* iter) {
	assert(iter);
	assert(iter->content->len == iter->initialLen);

	iter->initialLen = 0;
}

DataPiece* litera_note_content_iterator_get_current(LiteraNoteContentIterator* iter) {
	assert(iter);
	assert(iter->content->len == iter->initialLen);
	assert(iter->currentIdx < iter->initialLen);

	return iter->content->buffer + iter->currentIdx;
}

bool       litera_note_content_iterator_move_next(LiteraNoteContentIterator* iter) {
	assert(iter);
	assert(iter->content->len == iter->initialLen);

	++iter->currentIdx;

	return iter->currentIdx < iter->initialLen;
}

static void init_iterartor(LiteraNoteContent* content, LiteraNoteContentIterator* iter) {
	iter->content = content;
	iter->initialLen = content->len;
	iter->currentIdx = 0;
}

/* Note/Content */
void litera_note_iterate(LiteraNote* note, LiteraNoteContentIterator* iter) {
	assert(note);
	assert(iter);

	init_iterartor(note->content, iter);
}

void litera_note_init_text_piece(DataPiece* piece, int len) {
	piece->type = DATA_PIECE_TEXT;
	piece->text.text = (char*) malloc(sizeof(char) * len);
	piece->text.len = len;
}

DataPiece*         litera_note_create_text_piece(int len) {
	DataPiece* piece = (DataPiece*)malloc(sizeof(DataPiece));

	litera_note_init_text_piece(piece, len);

	return piece;
}

LiteraNoteContent* litera_note_create_content(int capacity) {
	LiteraNoteContent* content = (LiteraNoteContent*) malloc(sizeof(LiteraNoteContent));

	content->buffer = (DataPiece*) malloc(sizeof(DataPiece) * capacity);
	content->len = 0;
	content->cap = capacity;

	return content;
}

static DataPiece*  litera_note_add_piece_internal(LiteraNoteContent* content, const DataPiece* piece) {
	if(content->len == content->cap) {
		void* newBuf = realloc(content->buffer, sizeof(DataPiece) * content->cap * 2);
		assert(newBuf);
		content->buffer = (DataPiece*)newBuf;
	}

    int curr = content->len++;
	//I'm not sure how bad is it.
	content->buffer[curr] = *piece;
    return content->buffer + curr;
}

void               litera_note_add_piece(LiteraNote* note, const DataPiece* piece) {
	assert(note);
	LiteraNoteContent* content = note->content;
	litera_note_add_piece_internal(content, piece);
}

DataPiece*         litera_note_add_text(LiteraNote* note, const char* text, int len) {
	DataPiece piece;
	litera_note_init_text_piece(&piece, len);
	
	//TODO: refactor
	if (text != NULL ) {
		memcpy(piece.text.text, text, len);
	}

	return litera_note_add_piece_internal(note->content, &piece);
}

void               litere_note_remove_piece(LiteraNote* note, DataPiece* piece) {
	LiteraNoteContentIterator iter;
	litera_note_iterate(note, &iter);
	int idx = -1;
	do {
		DataPiece* existing = litera_note_content_iterator_get_current(&iter);
		
		if(piece == existing) {
			idx = iter.currentIdx;
			break;
		}
	} while(litera_note_content_iterator_move_next(&iter));

	if(idx == -1) {
		return;
	}

	if(piece->type == DATA_PIECE_TEXT) {
		free(piece->text.text);
	}

	LiteraNoteContent* content = note->content;
	int lenToMove = content->len - idx + 1;
	if (lenToMove != 0) {
		memcpy(content->buffer + idx, content->buffer + idx + 1, sizeof(DataPiece) * lenToMove);
	}

	content->len--;
}

void litera_note_clear(LiteraNote* note) {
	LiteraNoteContentIterator iter;
	litera_note_iterate(note, &iter);
	do {
		DataPiece* piece = litera_note_content_iterator_get_current(&iter);
		if(piece->type == DATA_PIECE_TEXT) {
			free(piece->text.text);
		}

	} while(litera_note_content_iterator_move_next(&iter));

	note->content->len = 0;
}

void               litera_note_free_content(LiteraNoteContent* content) {
	LiteraNoteContentIterator iter;
	init_iterartor(content, &iter);
	do {
		DataPiece* piece = litera_note_content_iterator_get_current(&iter);
		if(piece->type == DATA_PIECE_TEXT) {
			free(piece->text.text);
		}

	} while(litera_note_content_iterator_move_next(&iter));

	free(content->buffer);
	free(content);
}
