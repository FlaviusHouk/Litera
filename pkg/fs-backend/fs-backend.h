#ifndef _FS_BACKEND_H_
#define _FS_BACKEND_H_

#include "backend.h"

typedef struct {
	char* baseFolderPath;
	int pathLen;
} FsConfig;

Backend     fs_get_backend();
FsConfig*   fs_get_config();

#endif //_FS_BACKEND_H_
