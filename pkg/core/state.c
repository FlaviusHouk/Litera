#include "state.h"
#include <stdlib.h>

struct user_t {
	char* displayName;
};

struct state_t {
	Backend backend;
	User* currentUser;
};

static void state_try_load_user(State* state);

State*     core_state_new() {
	State* state = (State*)malloc(sizeof(State));

    state->backend.name = NULL;
	state->currentUser = NULL;

    state_try_load_user(state);

	return state;
}

void       core_state_set_backend(State* state, Backend backend) {
	state->backend = backend;
}

LoginState core_state_get_login_state(State* state) {
    return state->currentUser == NULL ? NO_USER : SAVED_USER;
}

void       core_state_set_current_user(State* state, User* user) {
}

User*      core_state_get_current_user(State* state) {
	return state->currentUser;
}

void       core_state_free(State* state) {
	free(state);
}

static void state_try_load_user(State* state) {
}
