#include "http-transport.h"

#include "libsoup/soup.h"

struct _EvernoteHttpTransport {
	ThriftTransport parent;
	gchar* url;
	SoupSession* session;
	GOutputStream* output;
	GInputStream* input;
};

typedef enum {
	EVERNOTE_HTTP_TRANSPORT_PROPERTY_URL = 1,
	EVERNOTE_HTTP_TRANSPORT_PROPS_COUNT
} EvernoteHttpTransportProps;

static GParamSpec* props[EVERNOTE_HTTP_TRANSPORT_PROPS_COUNT] = { NULL, };

G_DEFINE_TYPE(EvernoteHttpTransport, evernote_http_transport, THRIFT_TYPE_TRANSPORT);

static gboolean evernote_http_transport_is_open(ThriftTransport* this) {
	return EVERNOTE_HTTP_TRANSPORT(this)->session != NULL;
}

static void evernote_http_transport_init (EvernoteHttpTransport* transport) {
	transport->url = NULL;
	transport->session = NULL;
	transport->input = NULL;
	transport->output = NULL;
}

static void evernote_http_transport_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* spec) {
	EvernoteHttpTransport* transport = EVERNOTE_HTTP_TRANSPORT(obj);

	switch ((EvernoteHttpTransportProps)prop_id) {
		case EVERNOTE_HTTP_TRANSPORT_PROPERTY_URL:
			g_value_set_string(value, transport->url);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, spec);
			break;
	}
}

static void evernote_http_transport_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* spec) {
	EvernoteHttpTransport* transport = EVERNOTE_HTTP_TRANSPORT(obj);

	switch ((EvernoteHttpTransportProps)prop_id) {
		case EVERNOTE_HTTP_TRANSPORT_PROPERTY_URL:
			g_free(transport->url);
			transport->url = g_value_dup_string(value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, spec);
			break;
	}
}

static gboolean evernote_http_transport_open(ThriftTransport* transport, GError** error) {
	EvernoteHttpTransport* this = EVERNOTE_HTTP_TRANSPORT(transport);

	this->session = soup_session_new_with_options ("user-agent", "Litera", "accept-language-auto", TRUE, "timeout", 30, NULL);

	//SoupLogger* logger = soup_logger_new(SOUP_LOGGER_LOG_BODY, -1);
	//soup_session_add_feature(this->session, SOUP_SESSION_FEATURE(logger));
	//g_object_unref(G_OBJECT(logger));

	this->output = g_memory_output_stream_new_resizable();

	return TRUE;
}

static gboolean evernote_http_transport_close(ThriftTransport* transport, GError** error) {
	EvernoteHttpTransport* this = EVERNOTE_HTTP_TRANSPORT(transport);

	g_object_unref(this->session);
	this->session = NULL;

	return TRUE;
}

static gboolean evernote_http_transport_write(ThriftTransport* transport, gpointer buf, guint32 len, GError** error) {
	GError* err = NULL;
	EvernoteHttpTransport* this = EVERNOTE_HTTP_TRANSPORT(transport);

	if(!evernote_http_transport_is_open(transport)) {
		if(!evernote_http_transport_open(transport, &err)) {
			g_propagate_error(error, err);
			return FALSE;
		}
	}

	gssize writen = g_output_stream_write(this->output, buf, len, NULL, &err);
	if(writen == -1) {
		g_propagate_error(error, err);
		return FALSE;
	}

	if(writen < len) {
		buf = (((gchar*)buf) + writen);
		len -= writen;
		writen = g_output_stream_write(this->output, buf, len, NULL, &err);

		if(writen == -1) {
			g_propagate_error(error, err);
			return FALSE;
		}

		if(writen < len) {
			GQuark q = g_quark_from_string("EvernoteHttpTransport");
			(*error) = g_error_new(q, 1, "cannot write buffer into output");
			return FALSE;
		}
	}

	return TRUE;
}

static gint32 evernote_http_transport_read(ThriftTransport* transport, gpointer buf, guint32 len, GError** error) {
	GError* err = NULL;
	EvernoteHttpTransport* this = EVERNOTE_HTTP_TRANSPORT(transport);

	if(this->input == NULL) {
		return 0;
	}

	gssize read = g_input_stream_read(this->input, buf, len, NULL, &err);
	if (err != NULL) {
		g_propagate_error(error, err);
		return -1;
	} else if (read == -1) {
		GQuark q = g_quark_from_string("EvernoteHttpTransport");
		(*error) = g_error_new(q, 1, "failed to read");
		return -1;
	}

	//g_print("Requesting %d bytes, having %ld.\n", len, read);
	//gint8* bytes = (gint8*)buf;
	//for(gint i = 0; i < read; ++i) {
	//	g_print("%02X", bytes[i]);
	//}
	//g_print("\n");

	return read;
}

static gboolean evernote_http_transport_flush(ThriftTransport* transport, GError** error) {
	GError* err = NULL;
	EvernoteHttpTransport* this = EVERNOTE_HTTP_TRANSPORT(transport);

	GOutputStream* oldOutput = this->output;
	this->output = g_memory_output_stream_new_resizable();
	if(!g_output_stream_close(oldOutput, NULL, &err)) {
		g_propagate_error(error, err);
		return FALSE;
	}

	GBytes* bytes = g_memory_output_stream_steal_as_bytes(G_MEMORY_OUTPUT_STREAM(oldOutput));
	SoupMessage* msg = soup_message_new("POST", this->url);
	gsize dataSize = 0;
	gpointer data = g_bytes_unref_to_data(bytes, &dataSize);
	soup_message_set_request(msg, "application/x-thrift", SOUP_MEMORY_TEMPORARY, data, dataSize);

	if(this->input != NULL) {
		g_object_unref(this->input);
	}

	guint status = soup_session_send_message(this->session, msg);
	if(status != 200) {
		GQuark q = g_quark_from_string("EvernoteHttpTransport");
		(*error) = g_error_new(q, 1, "request failed with status code %d", status);
		return FALSE;
	}

	SoupBuffer* resp = soup_message_body_flatten(msg->response_body);
	GBytes* respData = soup_buffer_get_as_bytes(resp);
	this->input = g_memory_input_stream_new_from_bytes(respData);

	g_object_unref(G_OBJECT(oldOutput));

	soup_buffer_free(resp);
	g_object_unref(msg);
	g_free(data);
	return TRUE;
}

static gboolean evernote_http_transport_read_end(ThriftTransport* transport, GError** error) {
	return TRUE;
}

static gboolean evernote_http_transport_write_end(ThriftTransport* transport, GError** error) {
	return TRUE;
}

static void evernote_http_transport_class_init(EvernoteHttpTransportClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);

	object_class->set_property = evernote_http_transport_set_property;
	object_class->get_property = evernote_http_transport_get_property;

	props[EVERNOTE_HTTP_TRANSPORT_PROPERTY_URL] = g_param_spec_string ("url", "Url", "Url of the HTTP server to communicate with", NULL, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_properties(object_class, EVERNOTE_HTTP_TRANSPORT_PROPS_COUNT, props);

	ThriftTransportClass* base = THRIFT_TRANSPORT_CLASS(class);

	base->is_open   = evernote_http_transport_is_open;
	base->open      = evernote_http_transport_open;
	base->close     = evernote_http_transport_close;
	base->write     = evernote_http_transport_write;
	base->read      = evernote_http_transport_read;
	base->flush     = evernote_http_transport_flush;
	base->read_end  = evernote_http_transport_read_end;
	base->write_end = evernote_http_transport_write_end;
}

EvernoteHttpTransport* evernote_http_transport_new(gchar* url) {
	return g_object_new(EVERNOTE_HTTP_TRANSPORT_TYPE, "url", url, NULL);
}
