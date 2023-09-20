#ifndef _CORE_STATE_H_
#define _CORE_STATE_H_

#include "backend.h"

typedef struct state_t State;

typedef enum login_state_t {
	NO_USER,
	SAVED_USER
} LoginState;

State*     core_state_new();
void       core_state_set_backend(State* state, Backend backend);
LoginState core_state_get_login_state(State* state);
void       core_state_set_current_user(State* state, User* user);
User*      core_state_get_current_user(State* state);
void       core_state_free(State* state);

#endif //_CORE_STATE_H_
