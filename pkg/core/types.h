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
    int type;
	char* text;
	int len;
	/*Text props*/
} TextPiece;

typedef struct {
	int type;
} ImagePiece;

#define DATA_PIECE_END   -1
#define DATA_PIECE_BREAK 0
#define DATA_PIECE_TEXT  1
#define DATA_PIECE_IMAGE 2

typedef union {
	int type;
    TextPiece text;
	ImagePiece image;
} DataPiece;

typedef struct {
	char* title;
	void* state;
	DataPiece* content;
} LiteraNote;

#endif //_CORE_TYPES_H_
