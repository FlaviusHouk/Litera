#ifndef _EVERNOTE_BACKEND_HTTP_TRANSPORT_H_
#define _EVERNOTE_BACKEND_HTTP_TRANSPORT_H_

#include <thrift/c_glib/transport/thrift_transport.h>

G_BEGIN_DECLS

#define EVERNOTE_HTTP_TRANSPORT_TYPE (evernote_http_transport_get_type())
GType evernote_http_transport_get_type();
G_GNUC_BEGIN_IGNORE_DEPRECATIONS

typedef struct _EvernoteHttpTransport EvernoteHttpTransport;
typedef struct { ThriftTransportClass parent_class; } EvernoteHttpTransportClass;

G_GNUC_UNUSED static inline EvernoteHttpTransport* EVERNOTE_HTTP_TRANSPORT (gpointer ptr) {
	return G_TYPE_CHECK_INSTANCE_CAST (ptr, EVERNOTE_HTTP_TRANSPORT_TYPE, EvernoteHttpTransport);
}

G_GNUC_UNUSED static inline gboolean EVERNOTE_IS_HTTP_TRANSPORT(gpointer ptr) {
	return G_TYPE_CHECK_INSTANCE_TYPE(ptr, EVERNOTE_HTTP_TRANSPORT_TYPE);
}

G_GNUC_END_IGNORE_DEPRECATIONS

EvernoteHttpTransport* evernote_http_transport_new(gchar* url);

G_END_DECLS

#endif //_EVERNOTE_BACKEND_HTTP_TRANSPORT_H_
