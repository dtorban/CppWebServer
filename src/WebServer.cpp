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

struct web_server_per_session_data_input {
	struct lws *wsi;
	WebServerBase::Session* impl;
	WebServerSessionState* state;
};

static const struct lws_http_mount mount = {
	/* .mount_next */		NULL,		/* linked-list "next" */
	/* .mountpoint */		"/",		/* mountpoint URL */
	/* .origin */			".", /* serve from dir */
	/* .def */			"index.html",	/* default filename */
	/* .protocol */			NULL,
	/* .cgienv */			NULL,
	/* .extra_mimetypes */		NULL,
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
		for (int f = 0; f < pss->state->outMessages.size(); f++) {
			//std::string dataToSend = "this is a test to see if it works."; 
			std::string val = pss->state->outMessages[f];

			int newLen = val.length();
			unsigned char *buf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + newLen +
					LWS_SEND_BUFFER_POST_PADDING);
			memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], val.c_str(), newLen);
			lws_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], newLen, LWS_WRITE_TEXT);
			free(buf);
		}

		pss->state->outMessages.clear();

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

WebServerBase::WebServerBase(int port) {
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

		sessionState->inMessages.clear();
	}
}