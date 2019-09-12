#include <iostream>
#include <map>
//#define _USE_MATH_DEFINES
//#include <cmath>
//#include <libwebsockets.h>
#include "WebServer.h"

class WebServerSession;
class WebServerCommand;

struct WebServerSessionState {
	std::map<std::string, WebServerCommand*> commands;
};

class WebServerCommand {
public:
	virtual ~WebServerCommand() {}
	virtual void execute(WebServerSession* session, picojson::value& command, WebServerSessionState* state) = 0;
};

class WebServerSession : public JSONSession {
public:
	WebServerSession(WebServerSessionState state) : state(state) {
	}
	~WebServerSession() {
	}
	void receiveJSON(picojson::value& val) {
		std::string cmd = val.get<picojson::object>()["command"].get<std::string>();
		std::cout << val.get<picojson::object>()["command"].get<std::string>() << std::endl;
		std::map<std::string, WebServerCommand*>::iterator it = state.commands.find(cmd);
		if (it != state.commands.end()) {
			it->second->execute(this, val, &state);
		}
	}

private:
	WebServerSessionState state;
};

class TestCommand : public WebServerCommand {
public:
	void execute(WebServerSession* session, picojson::value& command, WebServerSessionState* state) {
		//std::cout << "Moved to time: " << command.get<picojson::object>()["time"].get<double>() << std::endl;
		std::cout << "state->value" << std::endl;
		picojson::object data;
		data["val"] = picojson::value("test");
		data["cmd"] = picojson::value((double)1.0);
		picojson::value ret(data);
		session->sendJSON(ret);
	}
};

int main(int argc, char**argv) {
	std::cout << "Usage: ./bin/CellServer 8081" << std::endl;

	if (argc > 1) {
		int port = std::atoi(argv[1]);
		WebServerSessionState state;
		state.commands["test"] = new TestCommand();
		WebServerWithState<WebServerSession, WebServerSessionState> server(state,port);
		while (true) {
			server.service();
		}
	}

	return 0;
}


