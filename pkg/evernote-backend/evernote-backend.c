#include "evernote-backend.h"
#include "http-transport.h"
#include <thrift/c_glib/thrift.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol.h>
#include <stdio.h>
#include <glib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "user_store.h"
#include "note_store.h"

typedef struct _evernote_backend_state {
	UserStoreClient* userClient;
	NoteStoreClient* noteStore;
} EvernoteState;

static void evernote_init(void* state) {
	LIBXML_TEST_VERSION

	GError* err = NULL;
	EvernoteState* this = (EvernoteState*)state;

	EvernoteHttpTransport* transport = evernote_http_transport_new("https://sandbox.evernote.com/edam/user");
	ThriftProtocol* protocol =  g_object_new (THRIFT_TYPE_BINARY_PROTOCOL, "transport", transport, NULL);
	this->userClient = g_object_new(TYPE_USER_STORE_CLIENT, "input_protocol", protocol, "output_protocol", protocol, NULL);

	gboolean checkValue = FALSE;
    if(!user_store_client_check_version(USER_STORE_IF(this->userClient), &checkValue, "Litera/0.0.1; Linux/Fedora 38", EDAM_VERSION_MAJOR, EDAM_VERSION_MINOR, &err)) {
		printf("%s. %s\n", "Could not check version", err->message);
	} else if (!checkValue) {
		printf("%s\n", "Error during check");
		return;
	}	
}

static LiteraUser* evernote_login_dev(void* state, const char* token) {
	GError* err = NULL;
	EvernoteState* this = (EvernoteState*)state;

	UserUrls* urls = g_object_new(TYPE_USER_URLS, NULL);
	EDAMUserException* userException = NULL;
	EDAMSystemException* systemException = NULL; 
	if(!user_store_client_get_user_urls(USER_STORE_IF(this->userClient), &urls, token, &userException, &systemException, &err)) {
		printf("%s\n", "Could not get user urls");
		return NULL;
	}

    gchar* noteStoreUrl = urls->noteStoreUrl;
	EvernoteHttpTransport* storeTransport = evernote_http_transport_new(noteStoreUrl);
	ThriftProtocol* storeProtocol =  g_object_new (THRIFT_TYPE_BINARY_PROTOCOL, "transport", storeTransport, NULL);
	this->noteStore = g_object_new(TYPE_NOTE_STORE_CLIENT, "input_protocol", storeProtocol, "output_protocol", storeProtocol, NULL);

	LiteraUser* literaUser = g_new(LiteraUser, 1);
	literaUser->access_token = g_strdup(token);

	User* user = g_object_new(TYPE_USER, NULL);
	if(!user_store_if_get_user (USER_STORE_IF(this->userClient), &user, token, &userException, &systemException, &err)) {
		printf("%s\n", "cannot get user info");
		return NULL;
	}

	literaUser->display_name = g_strdup(user->name);
	g_object_unref(user);

	return literaUser;
}

static LiteraNotebook** evernote_get_notebooks(void* state, LiteraUser* user) {
	EvernoteState* this = (EvernoteState*)state;
	GError* err = NULL;
	EDAMUserException* userException = NULL;
	EDAMSystemException* systemException = NULL;
	GPtrArray* notebooks = g_ptr_array_new();

	if(!note_store_if_list_notebooks (NOTE_STORE_IF(this->noteStore), &notebooks, user->access_token, &userException, &systemException, &err)) {
		printf("%s\n", "Cannot read notebooks");
	}

	LiteraNotebook** returnValue = (LiteraNotebook**)malloc(sizeof(LiteraNotebook*) * notebooks->len + 1);
	returnValue[notebooks->len] = NULL;
    for(int i = 0; i < notebooks->len; i++) {
		returnValue[i] = (LiteraNotebook*)malloc(sizeof(LiteraNotebook));
		Notebook* evNotebook = (Notebook*)notebooks->pdata[i];

		returnValue[i]->display_name = strdup(evNotebook->name);
		returnValue[i]->state = g_object_ref(G_OBJECT(evNotebook));
	}

    g_ptr_array_unref(notebooks);

	return returnValue;
}

static LiteraNote** evernote_get_notes(void* state, LiteraUser* user, LiteraNotebook* notebook) {
    EvernoteState* this = (EvernoteState*)state;
	GError* err = NULL;
	EDAMUserException* userException = NULL;
	EDAMSystemException* systemException = NULL;
	EDAMNotFoundException * notFoundException = NULL;
	Notebook* evNotebook = NOTEBOOK(notebook->state);

    NoteFilter* filter = g_object_new(TYPE_NOTE_FILTER, NULL);
	filter->notebookGuid = g_strdup(evNotebook->guid); 
	filter->__isset_notebookGuid = TRUE;

    NotesMetadataList* list = g_object_new(TYPE_NOTES_METADATA_LIST, NULL);
	NotesMetadataResultSpec* spec = g_object_new(TYPE_NOTES_METADATA_RESULT_SPEC, NULL);
    spec->includeTitle = TRUE;
	spec->__isset_includeTitle = TRUE;
	spec->includeContentLength = TRUE;
	spec->__isset_includeContentLength = TRUE;

    if(!note_store_if_find_notes_metadata (NOTE_STORE_IF(this->noteStore), &list, user->access_token, filter, 0, 100, spec, &userException, &systemException, &notFoundException, &err)) {
		printf("%s: %s\n", "cannot get notes for notebook", err->message);
		return NULL;
	}

	GPtrArray* obtainedNotes = list->notes;
    LiteraNote** notes = (LiteraNote**)malloc(sizeof(LiteraNote*) * (obtainedNotes->len + 1));
	notes[obtainedNotes->len] = NULL;
	for(gint i = 0; i < obtainedNotes->len; ++i) {
		LiteraNote* note = g_new(LiteraNote, 1);
		NoteMetadata* evNote = (NoteMetadata*)obtainedNotes->pdata[i];
		note->title = g_strdup(evNote->title);
		note->state = g_object_ref(G_OBJECT(evNote));
		note->content = NULL;
		notes[i] = note;
	}

	g_object_unref(G_OBJECT(filter));
	g_object_unref(G_OBJECT(spec));
	g_object_unref(G_OBJECT(list));

	return notes;
}

static void evernote_parse_div(xmlNodePtr node, DataPiece* data) {
	TextPiece* text = &(data->text);
	text->type = DATA_PIECE_TEXT;
	text->len = 0;

	xmlNodePtr child = node->children;
	int currIdx = 0;

	//Calculate buffer for storing text
	while(child != NULL) {
		if(child->type == XML_TEXT_NODE) {
			xmlChar* content = child->content;
			int currLen = xmlStrlen(content);	
            if (content[currLen - 1] == '\0') {
				currLen--;
			}

			text->len += currLen;
		} else if (xmlStrcmp(child->name, (const xmlChar*)"br") == 0) {
			text->len++;
		}

		child = child->next;
	}

	text->text = g_new(gchar, text->len);
	child = node->children;
    while(child != NULL) {
		if(child->type == XML_TEXT_NODE) {
			xmlChar* content = child->content;
			gchar* buffStart = text->text + currIdx;
            int currLen = xmlStrlen(content);
            if (content[currLen - 1] == '\0') {
				currLen--;
			}

			memcpy(buffStart, content, currLen);
			currIdx += currLen;
		} else if (xmlStrcmp(child->name, (const xmlChar*)"br") == 0) {
			text->text[currIdx++] = '\n';
		}

		child = child->next;
	}
}

static GArray* evernote_parse_xhtml_content(gchar* xhtml, gint len) {
	xmlDocPtr doc = xmlReadMemory(xhtml, len, NULL, NULL, 0);

	g_assert(doc);
    
	GArray* arr = g_array_new(FALSE, FALSE, sizeof(DataPiece));
	g_array_set_size(arr, 1);
    len = 0;

	xmlNodePtr root = xmlDocGetRootElement(doc);
	if(root == NULL || xmlStrEqual(root->name, (xmlChar*)"en-note") != 1) {
		printf("%s\n", "cannot get root element");
	    DataPiece* p = &g_array_index(arr, DataPiece, 0);
		p->type = DATA_PIECE_END;
		
		return arr;
	}

	unsigned long childrenCount = xmlChildElementCount(root);
	g_array_set_size(arr, childrenCount);

	xmlNodePtr child = root->children;
	while(child != NULL) {
		DataPiece* cur = &g_array_index(arr, DataPiece, len++);
		
		/* Paragraph */
		if (xmlStrEqual(child->name, (const xmlChar*)"div") == 1) {
			evernote_parse_div(child, cur);
		}

		child = child->next;
	}

	if(arr->len >= len) {
		g_array_set_size(arr, arr->len + 1);
	}
	
	DataPiece* end = &g_array_index(arr, DataPiece, len++);
	end->type = DATA_PIECE_END;

	return arr;
}

static DataPiece* evernote_refresh_note_content(void* state, LiteraUser* user, LiteraNote* note) {
	EvernoteState* this = (EvernoteState*)state;
	GError* err = NULL;
	EDAMUserException* userException = NULL;
	EDAMSystemException* systemException = NULL;
	EDAMNotFoundException * notFoundException = NULL;
	NoteMetadata* evNote = (NoteMetadata*)note->state;
	printf("Note id is %s\n", evNote->guid);

    gchar* content = g_new(gchar, evNote->contentLength);

    if(!note_store_if_get_note_content (NOTE_STORE_IF(this->noteStore), &content, user->access_token, evNote->guid, &userException, &systemException, &notFoundException, &err)) {
		printf("%s: %s\n", "cannot get note content", err->message);
		if(userException != NULL) {
			printf("User exception, code: %d\n", userException->errorCode);
		}
		return NULL;
	}

	GArray* p = evernote_parse_xhtml_content(content, evNote->contentLength);
	g_free(content);

	if(note->content != NULL) {
		for(int i = 0; note->content[i].type != DATA_PIECE_END; ++i) {
			DataPiece p = note->content[i];
			
			if(p.type == DATA_PIECE_TEXT) {
				TextPiece t = p.text;

				g_free(t.text);
			}
		}

		g_free(note->content);
	}

	note->content = (DataPiece*)p->data;
	
	return note->content;
}

static DataPiece* evernote_get_note_content(void* state, LiteraUser* user, LiteraNote* note) {
	if (note->content == NULL) {
		return evernote_refresh_note_content(state, user, note);
	}

	return note->content;
}

static EvernoteState state =
{
	.userClient = NULL,
	.noteStore = NULL
};

static Backend backend = 
{
	.name =              "Evernote",
	.is_initialized =    false,
    .init =              evernote_init,
	.login_dev =         evernote_login_dev,
	.get_notebooks =     evernote_get_notebooks,
	.get_notes =         evernote_get_notes,
	.get_content =       evernote_get_note_content,
	.refresh_content =   evernote_refresh_note_content,
	.state =             &state
};

Backend evernote_get_backend() {
	return backend;
}
