#ifndef _CORE_BACKEND_H_
#define _CORE_BACKEND_H_

#include <stdbool.h>
#include <types.h>

typedef struct backend_t {
	const char* name;
	User* (*login)(void* state, char* userName, char* password);
	void (*logout)(void* state);
	void* state;
	void* user_data;
} Backend;

#define core_backend_login(backend, userName, password) backend.login(backend.state, userName, password)
#define core_backend_logout(backend) backend.logout(backend.state)

#endif //_CORE_BACKEND_H_
