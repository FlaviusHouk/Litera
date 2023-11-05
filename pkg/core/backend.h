#ifndef _CORE_BACKEND_H_
#define _CORE_BACKEND_H_

#include <stdbool.h>
#include <types.h>

typedef struct backend_t {
	const char* name;
	bool is_initialized;
	void (*init)(void* state);
	LiteraUser* (*login)(void* state, char* userName, char* password);
	LiteraUser* (*login_dev)(void* state, const char* token);
	void (*logout)(void* state, LiteraUser* currentUser);
	LiteraNotebook** (*get_notebooks)(void* state, LiteraUser* currentUser);
	LiteraNote** (*get_notes)(void* state, LiteraUser* currentUser, LiteraNotebook* notebook);
	DataPiece* (*get_content)(void* state, LiteraUser* currentUser,  LiteraNote* note);
	DataPiece* (*refresh_content)(void* state, LiteraUser* currentUser,  LiteraNote* note);
	void (*save_content)(void* state, LiteraUser* user, LiteraNote* note, DataPiece* content);
	void* state;
} Backend;

#endif //_CORE_BACKEND_H_
