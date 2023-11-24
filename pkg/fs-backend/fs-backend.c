#include "fs-backend.h"
#include <stddef.h>

static FsConfig config =
{
	.pathLen = 0
};

static Backend* get_backend_local();

static void fs_init(void* state) {
	Backend* backend = get_backend_local();
	if (backend->is_initialized) {
		return;
	}

	backend->is_initialized = true;
}

static LiteraUser* fs_login_dev(void* state, const char* token) {
	return NULL;
}

static LiteraNotebookCollection* fs_get_notebooks (void* state, LiteraUser* currentUser) {
	return NULL;
}

static LiteraNote** fs_get_notes (void* state, LiteraUser* currentUser, LiteraNotebook* notebook) {
	return NULL;
}

static void fs_get_note_content (void* state, LiteraUser* currentUser,  LiteraNote* note) {
}

static void fs_refresh_note_content (void* state, LiteraUser* currentUser,  LiteraNote* note) {
}

static void fs_save_content (void* state, LiteraUser* user, LiteraNote* note) {
}

static Backend fs_backend =
{
	.name =              "File system",
	.is_initialized =    false,
	.init =              fs_init,
	.login_dev =         fs_login_dev,
	.get_notebooks =     fs_get_notebooks,
	.get_notes =         fs_get_notes,
	.get_content =       fs_get_note_content,
	.refresh_content =   fs_refresh_note_content,
	.save_content =      fs_save_content,
};

Backend     fs_get_backend() {
	return fs_backend;
}

static Backend* get_backend_local() {
	return &fs_backend;
}

FsConfig*   fs_get_config()
{
	return &config;
}
