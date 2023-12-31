#include "litera_notepad_page.h"
#include "content.h"

struct _LiteraNotepadPage {
	GtkWidget parent;

	GtkDropDown* notepadSelector;
	GtkTextView* textView; 
	GtkListBox* notesList;

	LiteraNotebook* selectedNotebook;
	LiteraNotebookCollection* notebooks;

	LiteraNote* selectedNote;
	LiteraNote** notes;

	GtkButton* refreshButton;
	GtkButton* saveButton;
};

static guint litera_notepad_page_refresh_signal_id;
static guint litera_notepad_page_save_signal_id;

typedef enum {
	LITERA_NOTEPAD_PAGE_SELECTED_NOTEBOOK = 1,
	LITERA_NOTEPAD_PAGE_SELECTED_NOTE = 2,
	LITERA_NOTEPAD_PAGE_PROPS_COUNT
} LiteraNotepadPageProps;

static GParamSpec* props[LITERA_NOTEPAD_PAGE_PROPS_COUNT] = { NULL, };

G_DEFINE_TYPE(LiteraNotepadPage, litera_notepad_page, GTK_TYPE_WIDGET);

static void litera_notepad_page_dispose(GObject* obj) {
	gtk_widget_dispose_template(GTK_WIDGET(obj), LITERA_NOTEPAD_PAGE_TYPE);

	GtkWidget* child = gtk_widget_get_first_child(GTK_WIDGET(obj));
	do {
		gtk_widget_unparent(child);
		child = gtk_widget_get_first_child(GTK_WIDGET(obj));
	} while(child != NULL);

	G_OBJECT_CLASS(litera_notepad_page_parent_class)->dispose(obj);
}

static void litera_notepad_page_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* spec) {
	LiteraNotepadPage* this = LITERA_NOTEPAD_PAGE(obj);

	switch ((LiteraNotepadPageProps)prop_id) {
		case LITERA_NOTEPAD_PAGE_SELECTED_NOTEBOOK:
			g_value_set_pointer(value, this->selectedNotebook);
			break;

		case LITERA_NOTEPAD_PAGE_SELECTED_NOTE:
			g_value_set_pointer(value, this->selectedNote);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, spec);
			break;
	}
}

static void litera_notepad_page_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* spec) {
	LiteraNotepadPage* this = LITERA_NOTEPAD_PAGE(obj);

	switch ((LiteraNotepadPageProps)prop_id) {
		case LITERA_NOTEPAD_PAGE_SELECTED_NOTEBOOK:
			this->selectedNotebook = (LiteraNotebook*)g_value_get_pointer(value);

			if(this->notes != NULL) {
				gint i = 0;
				while(this->notes[i] != NULL) {
					GtkListBoxRow* toRemove = gtk_list_box_get_row_at_index(this->notesList, i);
					gtk_list_box_remove(this->notesList, GTK_WIDGET(toRemove));
				}
			}
			this->notes = NULL;
			break;

		case LITERA_NOTEPAD_PAGE_SELECTED_NOTE:
			this->selectedNote = (LiteraNote*)g_value_get_pointer(value);
			gboolean noteRelatedOperationsEnabled = this->selectedNote != NULL;

			gtk_widget_set_sensitive(GTK_WIDGET(this->refreshButton), noteRelatedOperationsEnabled);
			gtk_widget_set_sensitive(GTK_WIDGET(this->saveButton), noteRelatedOperationsEnabled);

			if(this->selectedNote == NULL) {
				GtkTextIter start, end;
				GtkTextBuffer* buffer = gtk_text_view_get_buffer(this->textView);
	
				gtk_text_buffer_get_iter_at_offset(buffer, &start, 0);
				gtk_text_buffer_get_iter_at_offset(buffer, &end, -1);
				gtk_text_buffer_delete(buffer, &start, &end);
			}
	
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, spec);
			break;
	}
}

static void litera_notepad_page_class_init(LiteraNotepadPageClass* class) {
	GObjectClass* object_class =  G_OBJECT_CLASS(class);
	
	object_class->dispose = litera_notepad_page_dispose;
	object_class->set_property = litera_notepad_page_set_property;
	object_class->get_property = litera_notepad_page_get_property;

	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

	gtk_widget_class_set_template_from_resource (widget_class, "/litera/notepad_page.ui");
	gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
	
	gtk_widget_class_bind_template_child (widget_class, LiteraNotepadPage, notepadSelector);
	gtk_widget_class_bind_template_child (widget_class, LiteraNotepadPage, textView);
	gtk_widget_class_bind_template_child (widget_class, LiteraNotepadPage, notesList);
	gtk_widget_class_bind_template_child (widget_class, LiteraNotepadPage, refreshButton);
	gtk_widget_class_bind_template_child (widget_class, LiteraNotepadPage, saveButton);

	props[LITERA_NOTEPAD_PAGE_SELECTED_NOTEBOOK] = g_param_spec_pointer ("selected-notebook", "Selected notebook", "Selected notebook", G_PARAM_READWRITE);
	
	props[LITERA_NOTEPAD_PAGE_SELECTED_NOTE] = g_param_spec_pointer ("selected-note", "Selected note", "Selected note", G_PARAM_READWRITE);

	litera_notepad_page_refresh_signal_id = g_signal_new("refresh-note", G_TYPE_FROM_CLASS(class), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

	litera_notepad_page_save_signal_id = g_signal_new("save-note", G_TYPE_FROM_CLASS(class), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

	g_object_class_install_properties(object_class, LITERA_NOTEPAD_PAGE_PROPS_COUNT, props);
}

static void litera_notepad_page_selected_notebook_changed(GObject* obj, GParamSpec* spec, LiteraNotepadPage* page) {
	LiteraNotepadPage* this = (LiteraNotepadPage*)page;

	guint selected = gtk_drop_down_get_selected(this->notepadSelector);

	GValue value = G_VALUE_INIT;
	g_value_init(&value, G_TYPE_POINTER);
	LiteraNotebook* notebook = litera_notebook_collection_get(this->notebooks, selected);

	g_value_set_pointer(&value, notebook);
	g_object_set_property(G_OBJECT(this), "selected-notebook", &value);
}

static void litera_notepad_page_on_refresh(GtkButton* button, LiteraNotepadPage* page) {
	g_signal_emit(page, litera_notepad_page_refresh_signal_id, 0, page->selectedNote);
}

static void litera_notepad_page_on_save(GtkButton* button, LiteraNotepadPage* page) {
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(page->textView);

	GtkTextIter start, end;
	int paragraphCount = gtk_text_buffer_get_line_count(buffer);
	litera_note_clear(page->selectedNote);

	for(int i = 0; i < paragraphCount; ++i) {
		gtk_text_buffer_get_iter_at_line(buffer, &start, i);
		gtk_text_buffer_get_iter_at_line(buffer, &end, i + 1);

		//TODO: check for marks to verify no image is here.
		gchar* slice = gtk_text_buffer_get_slice(buffer, &start, &end, FALSE);
		gint len = strlen(slice);

		/*
		 * Looks like it is okay to skip the line because it is empty.
		 * Line with newline would not be empty
		 */
		if(len == 0) {
			continue;
		}

		litera_note_add_text(page->selectedNote, slice, len);
		
		g_free(slice);
	}

	g_signal_emit(page, litera_notepad_page_save_signal_id, 0, page->selectedNote);
}

static void litera_notepad_page_on_note_selected(GtkListBox* box, GtkListBoxRow* row, LiteraNotepadPage* page) {
	LiteraNote* note = NULL;

	if (row != NULL) {
		gint idx = gtk_list_box_row_get_index(row);
		note = page->notes[idx];
	}

	g_object_set(G_OBJECT(page), "selected-note", note, NULL);
}

static void litera_notepad_page_init(LiteraNotepadPage* widget) {
	gtk_widget_init_template(GTK_WIDGET(widget));

	g_signal_connect(widget->refreshButton, "clicked", G_CALLBACK(litera_notepad_page_on_refresh), widget);
	g_signal_connect(widget->saveButton, "clicked", G_CALLBACK(litera_notepad_page_on_save), widget);
	g_signal_connect(widget->notesList, "row-selected", G_CALLBACK(litera_notepad_page_on_note_selected), widget);
}

void litera_notepad_page_set_notebooks(LiteraNotepadPage* page, LiteraNotebookCollection* notebooks) {
	LiteraNotebookCollectionIterator iter;
	litera_notebook_collection_iterate(notebooks, &iter);
	GtkStringList* notebookList = gtk_string_list_new(NULL);

	do {
		LiteraNotebook notebook = litera_notebook_collection_iterator_get_current(&iter);
		gtk_string_list_append(notebookList, notebook.display_name);
	} while(litera_notebook_collection_iterator_move_next(&iter));

	page->notebooks = notebooks;

	gtk_drop_down_set_model(page->notepadSelector, G_LIST_MODEL(notebookList));
	g_signal_connect(page->notepadSelector, "notify::selected", G_CALLBACK(litera_notepad_page_selected_notebook_changed), page);	

	if(iter.currentIdx > 0) {
		LiteraNotebook* firstNotebook = litera_notebook_collection_get(notebooks, 0);
		g_object_set(page, "selected-notebook", firstNotebook, NULL);
	}
}

void litera_notepad_page_set_notes(LiteraNotepadPage* page, LiteraNote** notes) {
	page->notes = notes;
	gint i = 0;
	while(notes[i] != NULL) {
		GtkWidget* row = gtk_list_box_row_new();
		LiteraNote* note = notes[i++];
		GtkWidget* label = gtk_label_new(note->title);
		
		gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);

		gtk_list_box_append(page->notesList, row);
	}
}

LiteraNotebook*	 litera_notepad_page_get_selected_notebook(LiteraNotepadPage* page) {
	/*Use get property?*/
	return page->selectedNotebook;
}

LiteraNote* litera_notepad_page_get_selected_note(LiteraNotepadPage* page) {
	return page->selectedNote;
}

void litera_notepad_page_refresh(LiteraNotepadPage* page) {
	if(page->selectedNote == NULL) {
		return;
	}

	//TODO: make non cancellable operation
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(page->textView);
	GtkTextIter start, end;
	LiteraNoteContentIterator iter;
	litera_note_iterate(page->selectedNote, &iter);
			
	gtk_text_buffer_get_iter_at_offset(buffer, &start, 0);
	gtk_text_buffer_get_iter_at_offset(buffer, &end, -1);
	gtk_text_buffer_delete(buffer, &start, &end);

	//GtkTextMark* insertMark = gtk_text_buffer_get_insert(buffer);
	do {
		DataPiece* piece = litera_note_content_iterator_get_current(&iter);
		if(piece->type == DATA_PIECE_TEXT) {
			gtk_text_buffer_insert_at_cursor(buffer, piece->text.text, piece->text.len);
		}
	} while (litera_note_content_iterator_move_next(&iter));
}
