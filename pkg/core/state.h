#ifndef _CORE_STATE_H_
#define _CORE_STATE_H_

#include "backend.h"

typedef struct state_t State;

typedef enum login_state_t {
	NO_USER,
	SAVED_USER
} LoginState;

State*           core_state_new();
void             core_state_set_backend(State* state, Backend backend);
Backend          core_state_get_backend(State* state);
LoginState       core_state_get_login_state(State* state);
LiteraUser*      core_state_get_current_user(State* state);
void             core_state_free(State* state);

void             core_state_init_backend(State* state);
LiteraUser*      core_state_login(State* state, char* userName, char* password);
LiteraUser*      core_state_login_dev(State* state, char* token);
void             core_state_logout(State* state);
LiteraNotebook** core_state_get_notebooks(State* state);
LiteraNote**     core_state_get_notes(State* state, LiteraNotebook* notebook);
DataPiece*       core_state_get_content(State* state, LiteraNote* note);
void             core_state_set_content(State* state, LiteraNote* note, DataPiece* content);

#endif //_CORE_STATE_H_
