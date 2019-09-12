#ifndef WebSerVER_H_
#define WebSerVER_H_

#include <string>
#include <vector>
#include <libwebsockets.h>

class WebServerBase {
public:
	WebServerBase(int port = 8081);
	~WebServerBase();

	class Session {
	friend class WebServerBase;
	public:
		virtual ~Session();
		virtual void receiveMessage(const std::string& msg) {}
		void sendMessage(const std::string& msg);
	private:
		void* state;
	};

	void service(int time = 10);

	virtual void createSession(void* info);

protected:
	virtual Session* createSession() { return new Session(); }

private:
	lws_context *context;
	std::vector<Session*> sessions;
};

template <typename T>
class WebServer : public WebServerBase {
public:
	WebServer(int port = 8081) : WebServerBase(port) {}
protected:
	Session* createSession() { return new T(); }
};

template <typename T, typename STATE>
class WebServerWithState : public WebServerBase {
public:
	WebServerWithState(STATE state, int port = 8081) : WebServerBase(port), state(state) {}
protected:
	Session* createSession() { return new T(state); }
private:
	STATE state;
};

// JSON support
#include "picojson.h"

class JSONSession : public WebServerBase::Session {
public:
	virtual void receiveJSON(picojson::value& val) {}
	
	void sendJSON(picojson::value& val) {
		sendMessage(val.serialize());
	}

	void receiveMessage(const std::string& msg) {
		picojson::value val;
		std::string err = picojson::parse(val, msg);
		if (!err.empty()) {
			std::cerr << "Parse error: " << err << std::endl;
		}
		else {
			receiveJSON(val);	
		}
	}
};

#endif