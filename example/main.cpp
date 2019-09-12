#include <iostream>
#include <map>
//#define _USE_MATH_DEFINES
//#include <cmath>
//#include <libwebsockets.h>
#include "WebServer.h" 

class MyWebServerSession;
class MyWebServerCommand;

struct MyWebServerSessionState {
	std::map<std::string, MyWebServerCommand*> commands;
};

class MyWebServerCommand {
public:
	virtual ~MyWebServerCommand() {}
	virtual void execute(MyWebServerSession* session, picojson::value& command, MyWebServerSessionState* state) = 0;
};

class MyWebServerSession : public JSONSession {
public:
	MyWebServerSession(MyWebServerSessionState state) : state(state) {
	}
	~MyWebServerSession() {
	}
	void receiveJSON(picojson::value& val) {
		std::string cmd = val.get<picojson::object>()["command"].get<std::string>();
		std::cout << val.get<picojson::object>()["command"].get<std::string>() << std::endl;
		std::map<std::string, MyWebServerCommand*>::iterator it = state.commands.find(cmd);
		if (it != state.commands.end()) {
			it->second->execute(this, val, &state);
		}
	}

private:
	MyWebServerSessionState state;
};

class TestCommand : public MyWebServerCommand {
public:
	void execute(MyWebServerSession* session, picojson::value& command, MyWebServerSessionState* state) {
		std::cout << "Test Command" << std::endl;
		picojson::object data;
		data["command"] = picojson::value("None");
		data["val"] = picojson::value("TestValue");
		data["cmd"] = picojson::value((double)1.0);
		picojson::value ret(data);
		session->sendJSON(ret);
	}
};

class MouseMovedCommand : public MyWebServerCommand {
public:
	void execute(MyWebServerSession* session, picojson::value& command, MyWebServerSessionState* state) {
		std::cout << command.get<picojson::object>()["mouseX"] << " " << command.get<picojson::object>()["mouseY"]  << std::endl;
		picojson::object data;
		data["command"] = picojson::value("updateElipse");
		data["size"] = picojson::value(command.get<picojson::object>()["mouseX"].get<double>());
		picojson::object color;
		color["r"] = picojson::value((double)255.0);
		color["g"] = picojson::value((double)255-command.get<picojson::object>()["mouseY"].get<double>()/4);
		color["b"] = picojson::value((double)command.get<picojson::object>()["mouseY"].get<double>()/4);
		color["a"] = picojson::value((double)255.0);
		data["color"] = picojson::value(color);
		picojson::value ret(data);
		session->sendJSON(ret); 
	}
};

int main(int argc, char**argv) {
	std::cout << "Usage: ./bin/ExampleServer 8081" << std::endl;

	if (argc > 1) {
		int port = std::atoi(argv[1]);
		MyWebServerSessionState state;
		state.commands["test"] = new TestCommand();
		state.commands["mouseMoved"] = new MouseMovedCommand();
		WebServerWithState<MyWebServerSession, MyWebServerSessionState> server(state,port);
		while (true) {
			server.service();
		}
	}

	return 0;
}


