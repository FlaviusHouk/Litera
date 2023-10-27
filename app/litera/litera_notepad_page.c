#include "litera_notepad_page.h"

struct _LiteraNotepadPage {
	GtkWidget parent;

	GtkDropDown* notepadSelector;
	GtkTextView* textView; 
	GtkListBox* notesList;

	LiteraNotebook* selectedNotebook;
	LiteraNotebook** notebooks;

	LiteraNote* selectedNote;
	LiteraNote** notes;
};

typedef enum {
	LITERA_NOTEPAD_PAGE_SELECTED_NOTEBOOK = 1,
	LITERA_NOTEPAD_PAGE_SELECTED_NOTE = 2,
	LITERA_NOTEPAD_PAGE_PROPS_COUNT
} LiteraNotepadPageProps;

static GParamSpec* props[LITERA_NOTEPAD_PAGE_PROPS_COUNT] = { NULL, };

G_DEFINE_TYPE(LiteraNotepadPage, litera_notepad_page, GTK_TYPE_WIDGET);

static void litera_notepad_page_dispose(GObject* obj) {
	gtk_widget_dispose_template(GTK_WIDGET(obj), LITERA_NOTEPAD_PAGE_TYPE);

	LiteraNotepadPage* this = LITERA_NOTEPAD_PAGE(obj);

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
					printf("%s: %s\n", "Removing note", this->notes[i++]->title);
					gtk_list_box_remove(this->notesList, GTK_WIDGET(toRemove));
				}
			}
			this->notes = NULL;
		    break;

		case LITERA_NOTEPAD_PAGE_SELECTED_NOTE:
		    this->selectedNote = (LiteraNote*)g_value_get_pointer(value);
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

	props[LITERA_NOTEPAD_PAGE_SELECTED_NOTEBOOK] = g_param_spec_pointer ("selected-notebook", "Selected notebook", "Selected notebook", G_PARAM_READWRITE);
	
	props[LITERA_NOTEPAD_PAGE_SELECTED_NOTE] = g_param_spec_pointer ("selected-note", "Selected note", "Selected note", G_PARAM_READWRITE);

	g_object_class_install_properties(object_class, LITERA_NOTEPAD_PAGE_PROPS_COUNT, props);
}

static void litera_notepad_page_selected_notebook_changed(GObject* obj, GParamSpec* spec, LiteraNotepadPage* page) {
	LiteraNotepadPage* this = (LiteraNotepadPage*)page;

    guint selected = gtk_drop_down_get_selected(this->notepadSelector);

	GValue value = G_VALUE_INIT;
	g_value_init(&value, G_TYPE_POINTER);
	LiteraNotebook* notebook = this->notebooks[selected];

	g_value_set_pointer(&value, notebook);
	g_object_set_property(G_OBJECT(this), "selected-notebook", &value);
}

static void litera_notepad_page_init(LiteraNotepadPage* widget) {
	gtk_widget_init_template(GTK_WIDGET(widget));
}

void litera_notepad_page_set_notebooks(LiteraNotepadPage* page, LiteraNotebook** notebooks) {
	int i = 0;
	GtkStringList* notebookList = gtk_string_list_new(NULL);
	while(notebooks[i] != NULL) {
		LiteraNotebook* notebook = notebooks[i++];
		gtk_string_list_append(notebookList, notebook->display_name);
	}

	page->notebooks = notebooks;	

	gtk_drop_down_set_model(page->notepadSelector, G_LIST_MODEL(notebookList));
	g_signal_connect(page->notepadSelector, "notify::selected", G_CALLBACK(litera_notepad_page_selected_notebook_changed), page);	
}

static void litera_notepad_page_on_note_selected(GtkListBox* box, GtkListBoxRow* row, LiteraNotepadPage* page) {
	gint idx = gtk_list_box_row_get_index(row);
	
	GValue value = G_VALUE_INIT;
	g_value_init(&value, G_TYPE_POINTER);
	LiteraNote* note = page->notes[idx];

	g_value_set_pointer(&value, note);
	g_object_set_property(G_OBJECT(page), "selected-note", &value);
}

void litera_notepad_page_set_notes(LiteraNotepadPage* page, LiteraNote** notes) {
	page->notes = notes;
    gint i = 0;
	while(notes[i] != NULL) {
		LiteraNote* note = notes[i++];
		GtkWidget* label = gtk_label_new(note->title);
		gtk_list_box_append(page->notesList, label);
	}

	g_signal_connect(page->notesList, "row-selected", G_CALLBACK(litera_notepad_page_on_note_selected), page);
}

LiteraNotebook*     litera_notepad_page_get_selected_notebook(LiteraNotepadPage* page) {
	/*Use get property?*/
	return page->selectedNotebook;
}

LiteraNote* litera_notepad_page_get_selected_note(LiteraNotepadPage* page) {
	return page->selectedNote;
}

void litera_notepad_page_set_content(LiteraNotepadPage* page, gchar* content) {
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(page->textView);
	gtk_text_buffer_set_text(buffer, content, -1);
}