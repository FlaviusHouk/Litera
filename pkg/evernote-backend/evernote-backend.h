#ifndef _EVERNOTE_BACKEND_H_
#define _EVERNOTE_BACKEND_H_

#include "backend.h"

typedef struct {
	char* baseUrl;
} EvernoteConfig;

Backend         evernote_get_backend();
EvernoteConfig* evernote_get_config();

#endif //_EVERNOTE_BACKEND_H_
