#include "state.h"
#include <stdlib.h>

struct user_t {
	char* displayName;
};

struct state_t {
	Backend backend;
	LiteraUser* currentUser;
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

    backend.init(backend.state);
}

Backend          core_state_get_backend(State* state) {
	return state->backend;
}

LoginState core_state_get_login_state(State* state) {
    return state->currentUser == NULL ? NO_USER : SAVED_USER;
}

LiteraUser*      core_state_get_current_user(State* state) {
	return state->currentUser;
}

void core_state_free(State* state) {
	free(state);
}

void core_state_init_backend(State* state) {
	state->backend.init(state->backend.state);
}

LiteraUser* core_state_login(State* state, char* userName, char* password) {
	//TODO: handle login state
    void* backendState = state->backend.state;
	state->currentUser = state->backend.login(backendState, userName, password);

	return state->currentUser;
}

LiteraUser* core_state_login_dev(State* state, char* token) {
    void* backendState = state->backend.state;
	state->currentUser = state->backend.login_dev(backendState, token);

	return state->currentUser;
}

void core_state_logout(State* state) {
    void* backendState = state->backend.state;
	state->backend.logout(backendState, state->currentUser);

	state->currentUser = NULL;
}

LiteraNotebook** core_state_get_notebooks(State* state) {
	void* backendState = state->backend.state;
	return state->backend.get_notebooks(backendState, state->currentUser);
}

LiteraNote**     core_state_get_notes(State* state, LiteraNotebook* notebook) {
    void* backendState = state->backend.state;
	return state->backend.get_notes(backendState, state->currentUser, notebook);
}

DataPiece*           core_state_get_content(State* state, LiteraNote* note) {
    void* backendState = state->backend.state;
	return state->backend.get_content(backendState, state->currentUser, note);
}

static void state_try_load_user(State* state) {
}
