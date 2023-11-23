#include "content.h"
#include <stdlib.h>
#include <assert.h>

struct litera_notebook_collection_t {
	LiteraNotebook* buffer;
	int len;
	int cap;
};

void       litera_notebook_collection_iterator_reset(LiteraNotebookCollectionIterator* iter) {
	assert(iter);
	assert(iter->collection->len == iter->initialLen);

	iter->initialLen = 0;
}

LiteraNotebook litera_notebook_collection_iterator_get_current(LiteraNotebookCollectionIterator* iter) {
	assert(iter);
	assert(iter->collection->len == iter->initialLen);
	assert(iter->currentIdx < iter->initialLen);

	return iter->collection->buffer[iter->currentIdx];
}

bool       litera_notebook_collection_iterator_move_next(LiteraNotebookCollectionIterator* iter) {
	assert(iter);
	assert(iter->collection->len == iter->initialLen);

	++iter->currentIdx;

	return iter->currentIdx < iter->initialLen;
}

LiteraNotebookCollection* litera_notebook_collection_new(int cap) {
	LiteraNotebookCollection* collection = (LiteraNotebookCollection*)malloc(sizeof(LiteraNotebookCollection));

	if (cap < 1) {
		cap = 4;
	}

	collection->buffer = (LiteraNotebook*)malloc(sizeof(LiteraNotebook) * cap);
	collection->len = 0;
	collection->cap = cap;

	return collection;
}

void litera_notebook_collection_add(LiteraNotebookCollection* c, LiteraNotebook notebook) {
	if(c->len == c->cap) {
		void* newBuf = realloc(c->buffer, sizeof(LiteraNotebook) * c->cap * 2);
		assert(newBuf);
		c->buffer = (LiteraNotebook*)newBuf;
	}

	int curr = c->len++;
	c->buffer[curr] = notebook;
}

void litera_notebook_collection_iterate(LiteraNotebookCollection* c, LiteraNotebookCollectionIterator* iter) {
	iter->collection = c;
	iter->currentIdx = 0;
	iter->initialLen = c->len;
}
