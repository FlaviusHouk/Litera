#ifndef _CORE_TYPES_H_
#define _CORE_TYPES_H_

typedef struct {
	char* display_name;
	char* access_token;
} LiteraUser;

typedef struct {
    char* display_name;
	void* state;
} LiteraNotebook;

typedef struct {
	char* title;
	char* content; //Change
	void* state;
} LiteraNote;

#endif //_CORE_TYPES_H_
