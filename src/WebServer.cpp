/**
Copyright (c) 2019 Dan Orban
*/

#include "WebServer.h"
#include <iostream>
#include <algorithm>

struct WebServerSessionState {
	struct lws *wsi;
	std::vector<std::string> inMessages;
	std::vector<std::string> outMessages;
	std::vector<WebServerBase::Session*>* sessions;
};

WebServerBase::Session::~Session() {
	WebServerSessionState* sessionState = static_cast<WebServerSessionState*>(state);
	std::vector<WebServerBase::Session*>& sessions = *sessionState->sessions;
	std::vector<Session*>::iterator it = std::find(sessions.begin(), sessions.end(), this);
	if (it != sessions.end()) {
		sessions.erase(it);
	}

	delete sessionState;
}

void WebServerBase::Session::sendMessage(const std::string& msg) {
	WebServerSessionState& sessionState = *static_cast<WebServerSessionState*>(state);
	sessionState.outMessages.push_back(msg);
	lws_callback_on_writable(sessionState.wsi);
}

void WebServerBase::Session::onWrite() {
        WebServerSessionState& sessionState = *static_cast<WebServerSessionState*>(state);

	if (sessionState.outMessages.size() == 0) {
                        return;
        }
        std::string val = sessionState.outMessages[0];

        int newLen = val.length();
        unsigned char *buf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + newLen +
                        LWS_SEND_BUFFER_POST_PADDING);
        memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], val.c_str(), newLen);
        lws_write(sessionState.wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], newLen, LWS_WRITE_TEXT);
        free(buf);
        sessionState.outMessages.erase(sessionState.outMessages.begin());

        if (sessionState.outMessages.size() > 0) {
               lws_callback_on_writable(sessionState.wsi);
        }
}


struct web_server_per_session_data_input {
	struct lws *wsi;
	WebServerBase::Session* impl;
	WebServerSessionState* state;
};

static const struct lws_protocol_vhost_options pvo_mime = {
	NULL,				/* "next" pvo linked-list */
	NULL,				/* "child" pvo linked-list */
	".glb",				/* file suffix to match */
	"model/gltf-binary"		/* mimetype to use */
};

static const struct lws_protocol_vhost_options pvo_mime2 = {
	&pvo_mime,				/* "next" pvo linked-list */
	NULL,				/* "child" pvo linked-list */
	".obj",				/* file suffix to match */
	"model/obj"		/* mimetype to use */
};

static const struct lws_protocol_vhost_options pvo_mime3 = {
	&pvo_mime2,				/* "next" pvo linked-list */
	NULL,				/* "child" pvo linked-list */
	".mtl",				/* file suffix to match */
	"model/mtl"		/* mimetype to use */
};

static struct lws_http_mount mount = {
	/* .mount_next */		NULL,		/* linked-list "next" */
	/* .mountpoint */		"/",		/* mountpoint URL */
	/* .origin */			".", /* serve from dir */
	/* .def */			"index.html",	/* default filename */
	/* .protocol */			NULL,
	/* .cgienv */			NULL,
	/* .extra_mimetypes */		&pvo_mime3,
	/* .interpret */		NULL,
	/* .cgi_timeout */		0,
	/* .cache_max_age */		0,
	/* .auth_mask */		0,
	/* .cache_reusable */		0,
	/* .cache_revalidate */		0,
	/* .cache_intermediaries */	0,
	/* .origin_protocol */		LWSMPRO_FILE,	/* files in a dir */
	/* .mountpoint_len */		1,		/* char count */
	/* .basic_auth_login_file */	NULL, 
};

int callback_web_server(
		struct lws *wsi,
		enum lws_callback_reasons reason, void *user,
		void *in, size_t len)
{
	struct web_server_per_session_data_input *pss =
			(struct web_server_per_session_data_input *)user;

	WebServerBase* WebServer = static_cast<WebServerBase*>(lws_context_user(lws_get_context(wsi)));

	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED: {
		//pss->impl = new WebServerSessionImpl(wsi);
		pss->wsi = wsi;
		WebServer->createSession(pss);
		std::cout << "Connection established" << std::endl;
		break;
	}
	case LWS_CALLBACK_CLOSED: {
		delete pss->impl;
		std::cout << "Connection closed" << std::endl;
		break;
	}
	case LWS_CALLBACK_RECEIVE: {
		std::string data((char *)in, len);
		//pss->impl->receiveMessage(data);
		pss->state->inMessages.push_back(data);
		break;
	}
	case LWS_CALLBACK_SERVER_WRITEABLE: {
		pss->impl->onWrite();
		//std::cout << "Messages: " << pss->state->outMessages.size() << std::endl;
		/*if (pss->state->outMessages.size() == 0) {
			return 0;
		}
			std::string val = pss->state->outMessages[0];

			int newLen = val.length();
			unsigned char *buf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + newLen +
					LWS_SEND_BUFFER_POST_PADDING);
			memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], val.c_str(), newLen);
			lws_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], newLen, LWS_WRITE_TEXT);
			free(buf);
		pss->state->outMessages.erase(pss->state->outMessages.begin());

		if (pss->state->outMessages.size() > 0) {
			lws_callback_on_writable(wsi);
		}*/

		break;
	}
	}
	return 0;
}

struct lws_protocols web_server_protocols[] = {
		/* first protocol must always be HTTP handler */
		{
				"http-only",   // name
				lws_callback_http_dummy, // callback
				0              // per_session_data_size
		},
		{
				"web_server",   // name
				callback_web_server, // callback
				sizeof (struct web_server_per_session_data_input)              // per_session_data_size
		},
		{
				NULL, NULL, 0   /* End of list */
		}
};

WebServerBase::WebServerBase(int port, const std::string& webDir) : webDir(webDir) {
	struct lws_context_creation_info info;

	memset(&info, 0, sizeof info);

	info.port = port;
	info.iface = NULL;
	info.protocols = web_server_protocols;
	info.extensions = NULL;
	info.ssl_cert_filepath = NULL;
	info.ssl_private_key_filepath = NULL;
	info.ssl_ca_filepath = NULL;
	info.gid = -1;
	info.uid = -1;
	info.options = 0;
	info.ka_time = 0;
	info.ka_probes = 0;
	info.ka_interval = 0;
	std::cout << "Starting web server for directory: " << webDir.c_str() << std::endl;
	mount.origin = this->webDir.c_str();
	info.mounts = &mount;
	info.user = this;

	context = lws_create_context(&info);

	if (context == NULL) {
		std::cout << "libwebsocket init failed\n" << std::endl;
	}

	std::cout << "starting server..." << std::endl;

}

WebServerBase::~WebServerBase() {
}

void WebServerBase::createSession(void* info) {
	struct web_server_per_session_data_input *pss = static_cast<struct web_server_per_session_data_input *>(info);
	Session* session = createSession();
	sessions.push_back(session);
	pss->impl = session;
	pss->state = new WebServerSessionState();
	pss->state->wsi = pss->wsi;
	session->state = pss->state;
	pss->state->sessions = &sessions;
}

void WebServerBase::service(int time) {
	lws_service(context, time);
	for (int f = 0; f < sessions.size(); f++) {
		WebServerSessionState* sessionState = static_cast<WebServerSessionState*>(sessions[f]->state);
		for (int i = 0; i < sessionState->inMessages.size(); i++) {
			sessions[f]->receiveMessage(sessionState->inMessages[i]);
		}
			
		sessions[f]->update();

		sessionState->inMessages.clear();
	}
}
