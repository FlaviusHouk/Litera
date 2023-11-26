#ifndef _CORE_BACKEND_H_
#define _CORE_BACKEND_H_

#include <stdbool.h>
#include <types.h>

typedef struct backend_t {
	const char* name;
	bool is_initialized;
	void (*init)(void* state);
	bool (*login)(void* state, char* userName, char* password, LiteraUser* user);
	bool (*login_dev)(void* state, const char* token, LiteraUser* user);
	void (*logout)(void* state, LiteraUser* currentUser);
	LiteraNotebookCollection* (*get_notebooks)(void* state, LiteraUser* currentUser);
	LiteraNote** (*get_notes)(void* state, LiteraUser* currentUser, LiteraNotebook* notebook);
	void (*get_content)(void* state, LiteraUser* currentUser,  LiteraNote* note);
	void (*refresh_content)(void* state, LiteraUser* currentUser,  LiteraNote* note);
	void (*save_content)(void* state, LiteraUser* user, LiteraNote* note);
	void* state;
} Backend;

#endif //_CORE_BACKEND_H_
