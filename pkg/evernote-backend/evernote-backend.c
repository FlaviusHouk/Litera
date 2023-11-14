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
#include "content.h"

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

static DataPiece* evernote_parse_div(xmlNodePtr node) {
	gint len = 0;

	xmlNodePtr child = node->children;
	int currIdx = 0;

	//Calculate buffer for storing text
	while(child != NULL) {
		if(child->type == XML_TEXT_NODE) {
			xmlChar* content = child->content;
			int currLen = xmlStrlen(content);	
			
			len += currLen;
		} else if (xmlStrcmp(child->name, BAD_CAST"br") == 0) {
			len++;
		}

		child = child->next;
	}
    
	DataPiece* piece = litera_note_create_text_piece(len);
	TextPiece* text = &piece->text;

	child = node->children;
    while(child != NULL) {
		if(child->type == XML_TEXT_NODE) {
			xmlChar* content = child->content;
			gchar* buffStart = text->text + currIdx;
            int currLen = xmlStrlen(content);

			memcpy(buffStart, content, currLen);
			currIdx += currLen;
		} else if (xmlStrcmp(child->name, BAD_CAST"br") == 0) {
			text->text[currIdx++] = '\n';
		}

		child = child->next;
	}

	return piece;
}

static void evernote_parse_xhtml_content(gchar* xhtml, gint len, LiteraNote* note) {
	xmlDocPtr doc = xmlReadMemory(xhtml, len, NULL, NULL, 0);

	g_assert(doc);

	g_print("%s\n", xhtml);
    len = 0;

	xmlNodePtr root = xmlDocGetRootElement(doc);
	if(root == NULL || xmlStrEqual(root->name, (xmlChar*)"en-note") != 1) {
		printf("%s\n", "cannot get root element");
		
		return;
	}

	xmlNodePtr child = root->children;
	while(child != NULL) {
		/* Paragraph */
		if (xmlStrEqual(child->name, (const xmlChar*)"div") == 1) {
			DataPiece* paragraphPiece = evernote_parse_div(child);
			litera_note_add_piece(note, paragraphPiece);
		}

		child = child->next;
	}
}

static void evernote_refresh_note_content(void* state, LiteraUser* user, LiteraNote* note) {
	EvernoteState* this = (EvernoteState*)state;
	GError* err = NULL;
	EDAMUserException* userException = NULL;
	EDAMSystemException* systemException = NULL;
	EDAMNotFoundException * notFoundException = NULL;
	NoteMetadata* evNote = (NoteMetadata*)note->state;
	printf("Note id is %s\n", evNote->guid);

    gchar* content = g_new(gchar, evNote->contentLength);
	if(note->content != NULL) {
		litera_note_clear(note);
	} else {
		note->content = litera_note_create_content(/*capacity*/4);
	}

    if(!note_store_if_get_note_content (NOTE_STORE_IF(this->noteStore), &content, user->access_token, evNote->guid, &userException, &systemException, &notFoundException, &err)) {
		printf("%s: %s\n", "cannot get note content", err->message);
		if(userException != NULL) {
			printf("User exception, code: %d\n", userException->errorCode);
		}

		return;
	}

	evernote_parse_xhtml_content(content, evNote->contentLength, note);

	g_free(content);
}

static void evernote_get_note_content(void* state, LiteraUser* user, LiteraNote* note) {
	if (note->content != NULL) {
		return;
	}

	evernote_refresh_note_content(state, user, note);
}

static xmlDocPtr evernote_format_enml(LiteraNote* note) {
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0"); /*xmlVersion*/

    xmlNodePtr root = xmlNewNode(/*ns*/ NULL, BAD_CAST "en-note");
	xmlDocSetRootElement(doc, root);

    xmlDtdPtr dtd = xmlCreateIntSubset(doc, BAD_CAST "en-note", NULL, BAD_CAST "http://xml.evernote.com/pub/enml2.dtd");

	LiteraNoteContentIterator iter;
	litera_note_iterate(note, &iter);
	do {
		DataPiece* piece = litera_note_content_iterator_get_current(&iter);

		if(piece->type == DATA_PIECE_TEXT) {
			xmlNodePtr div = xmlNewNode(/*ns*/NULL, BAD_CAST "div");
			char* pos = strchr(piece->text.text, '\n');
			
			xmlChar* textContent;
			gboolean haveNewline = pos != NULL;
			if (!haveNewline) {
				textContent = xmlCharStrdup(piece->text.text);
			} else {
				int len = pos - piece->text.text;
				textContent = xmlCharStrndup(piece->text.text, len);
			}

			xmlNodeAddContent(div, textContent);
   
            if(haveNewline) {
            	xmlNodePtr br = xmlNewNode(/*ns*/NULL, BAD_CAST "br");
				xmlSetProp(br, BAD_CAST "clear", BAD_CAST "none");

				xmlAddChild(div, br);
			}

			xmlFree(textContent);

			xmlAddChild(root, div);
		}
	} while(litera_note_content_iterator_move_next(&iter));
		
	return doc;
}

static void evernote_save_content(void* state, LiteraUser* user, LiteraNote* note) {
	xmlChar* rawXml;
	gint xmlContentLen;
    GError* err = NULL;
	EDAMUserException* userException = NULL;
	EDAMSystemException* systemException = NULL;
	EDAMNotFoundException* notFoundException = NULL;
	EvernoteState* this = (EvernoteState*)state;

	xmlDocPtr xmlContent = evernote_format_enml(note);
	xmlDocDumpFormatMemoryEnc(xmlContent, &rawXml, &xmlContentLen, "UTF-8", /*formating spaces*/0);

	NoteMetadata* metadata = NOTE_METADATA(note->state);
	Note* returnNote = g_object_new(TYPE_NOTE, NULL);
    returnNote->guid = metadata->guid;
	returnNote->__isset_guid = TRUE;
	returnNote->title = metadata->title;
	returnNote->__isset_title = TRUE;
    returnNote->content = (gchar*)rawXml;
    returnNote->__isset_content = TRUE;

    Note* sendNote = g_object_new(TYPE_NOTE, NULL);
    sendNote->guid = metadata->guid;
	sendNote->__isset_guid = TRUE;
	sendNote->title = metadata->title;
	sendNote->__isset_title = TRUE;
    sendNote->content = (gchar*)rawXml;
    sendNote->__isset_content = TRUE;

    if(!note_store_client_update_note(NOTE_STORE_IF(this->noteStore), &returnNote, user->access_token, sendNote, &userException, &systemException, &notFoundException, &err)) {
		g_print("%s: %s\n", "cannot update note", err->message);
		return;
	}

	xmlFree(rawXml);
	xmlFree(xmlContent);

    returnNote->guid = NULL;
	returnNote->__isset_guid = FALSE;
	returnNote->title = NULL;
	returnNote->__isset_title = FALSE;
    returnNote->content = NULL;
    returnNote->__isset_content = FALSE;
	g_object_unref(G_OBJECT(returnNote));

    sendNote->guid = NULL;
	sendNote->__isset_guid = FALSE;
	sendNote->title = NULL;
	sendNote->__isset_title = FALSE;
	sendNote->content = NULL;
	sendNote->__isset_content = FALSE;
	g_object_unref(G_OBJECT(sendNote));

    //TODO: set note content
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
	.save_content =      evernote_save_content,
	.state =             &state
};

Backend evernote_get_backend() {
	return backend;
}
