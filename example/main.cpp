#include <iostream>
#include <map>
#include "WebServer.h"
//#define _USE_MATH_DEFINES
//#include <cmath>
//#include <libwebsockets.h> 


// * * * * * DUMMY DATA * * * * * //
struct position {
    position() : x(0), y(0) {}
    float x;
    float y;
};
struct bus {
    bus() : id(""), pos(position()) {}
    std::string id;
    position pos;
};
struct stop {
    stop() : id(""), pos(position()), numPeople(0) {}
    std::string id;
    position pos;
    int numPeople;
};
struct route {
    route() : id(""), positions(std::vector<position>(0)) {}
    std::string id;
    std::vector<position> positions;
};



class MyWebServerSession;
class MyWebServerCommand;

struct MyWebServerSessionState {
    MyWebServerSessionState() : commands(std::map<std::string, MyWebServerCommand*>()) {}
    std::map<std::string, MyWebServerCommand*> commands;
};

class MyWebServerCommand {
public:
    virtual ~MyWebServerCommand() {}
    virtual void execute(MyWebServerSession* session, picojson::value& command, MyWebServerSessionState* state) = 0;
};


class MyWebServerSession : public JSONSession {
public:
    MyWebServerSession(MyWebServerSessionState s) : state(s) {
    }
    ~MyWebServerSession() {
    }
    void receiveJSON(picojson::value& val) override {
        std::string cmd = val.get<picojson::object>()["command"].get<std::string>();
        std::cout << val.get<picojson::object>()["command"].get<std::string>() << std::endl;
        std::map<std::string, MyWebServerCommand*>::iterator it = state.commands.find(cmd);
        if (it != state.commands.end()) {
            it->second->execute(this, val, &state);
        }
    }
    void update() override {    }

private:
    MyWebServerSessionState state;


};



///***** * * * * * COMMANDS * * * * * ******///

class TestCommand : public MyWebServerCommand {
public:
    void execute(MyWebServerSession* session, picojson::value& command, MyWebServerSessionState* state) override {
        // Unsused input
        (void)command;
        (void)state;


        std::cout << "Test Command" << std::endl;
        
        picojson::object data;
        data["command"] = picojson::value("None");
        data["val"]     = picojson::value("TestValue");
        data["cmd"]     = picojson::value(static_cast<double>(1.0));
        
        picojson::value ret(data);
        
        session->sendJSON(ret);
    }
};

class MouseMovedCommand : public MyWebServerCommand {
public:
    void execute(MyWebServerSession* session, picojson::value& command, MyWebServerSessionState* state) override {
        // Unused input
        (void)state;
        
        
        std::cout 
            << command.get<picojson::object>()["mouseX"] 
            << " " 
            << command.get<picojson::object>()["mouseY"] 
            << std::endl;

        picojson::object data;
        data["command"] = picojson::value("updateElipse");
        data["size"]    = picojson::value(command.get<picojson::object>()["mouseX"].get<double>());
        
        picojson::object color;
        color["r"]  = picojson::value(static_cast<double>(255.0));
        color["g"]  = picojson::value(static_cast<double>(255-command.get<picojson::object>()["mouseY"].get<double>()/4));
        color["b"]  = picojson::value(static_cast<double>(command.get<picojson::object>()["mouseY"].get<double>()/4));
        color["a"]  = picojson::value(static_cast<double>(255.0));
        data["color"]   = picojson::value(color);
        
        picojson::value ret(data);
        
        session->sendJSON(ret);
    }
};

class GetRoutesCommand : public MyWebServerCommand {
public:
    GetRoutesCommand() : routes(std::vector<route*>(3)) {
        for (int i = 0; i < static_cast<int>(routes.size()); i++) {
            routes[i] = new route();
            routes[i]->id = std::to_string(i);

            for (int j = 0; j < 10; j++) {
                position p;
                p.x = i * 100 + 400;
                p.y = j * 100 + 100;
                
                routes[i]->positions.push_back(p);
            }
        }
    }
    void execute(MyWebServerSession* session, picojson::value& command, MyWebServerSessionState* state) override {
        // Unsused input
        (void)command;
        (void)state;
        
        std::cout << "Updating routes" << std::endl;

        picojson::object data;
        data["command"] = picojson::value("updateRoutes");
        data["numRoutes"] = picojson::value(static_cast<double>(routes.size()));

        picojson::array routesArray;

        for (int i = 0; i < static_cast<int>(routes.size()); i++) {
            picojson::object r;
            picojson::value id = picojson::value(std::to_string(i));
            r["id"] = picojson::value(id);
            r["numPositions"] = picojson::value(static_cast<double>(routes[i]->positions.size()));

            picojson::array pArray;
            for (int j = 0; j < static_cast<int>(routes[i]->positions.size()); j++) {
                picojson::object pStruct;
                pStruct["x"] = picojson::value(routes[i]->positions[j].x);
                pStruct["y"] = picojson::value(routes[i]->positions[j].y);

                pArray.push_back(picojson::value(pStruct));
            }

            r["positions"] = picojson::value(pArray);
            routesArray.push_back(picojson::value(r));
        }
        data["routes"] = picojson::value(routesArray);

        picojson::value ret(data);

        session->sendJSON(ret);
    }
private:
    std::vector<route*> routes;
};

class GetBussesCommand : public MyWebServerCommand {
public:
    GetBussesCommand() : busses(std::vector<bus*>(3)) {
        for (int i = 0; i < static_cast<int>(busses.size()); i++) {
            busses[i] = new bus();
            busses[i]->id = std::to_string(i);
            busses[i]->pos.x = i * 100 + 100;
            busses[i]->pos.y = 200;
        }
    }
    void execute(MyWebServerSession* session, picojson::value& command, MyWebServerSessionState* state) override {
        // Unsused input
        (void)command;
        (void)state;
        
        
        std::cout << "Printing Update Busses on web" << std::endl;

        picojson::object data;
        data["command"] = picojson::value("updateBusses");
        

        picojson::value ret(data);

        session->sendJSON(ret);
    }
private:
    std::vector<bus*> busses;
};

class GetStopsCommand : public MyWebServerCommand {
public:
    GetStopsCommand() : stops(std::vector<stop*>(3)) {
        for (int i = 0; i < static_cast<int>(stops.size()); i++) {
            stops[i] = new stop();
            stops[i]->id = std::to_string(i);
            stops[i]->pos.x = i * 100 + 100;
            stops[i]->pos.y = 100;
            stops[i]->numPeople = 10;
        }
    }

    void execute(MyWebServerSession* session, picojson::value& command, MyWebServerSessionState* state) override {
        // Unsused input
        (void)command;
        (void)state;
        
        
        std::cout << "Printing Update Stops on web" << std::endl;

        picojson::object data;
        data["command"] = picojson::value("updateStops");
        

        picojson::value ret(data);

        session->sendJSON(ret);
    }

private:
    std::vector<stop*> stops;
};



int main(int argc, char**argv) {
    std::cout << "Usage: ./build/bin/ExampleServer 8081" << std::endl;

    if (argc > 1) {
        int port = std::atoi(argv[1]);
        MyWebServerSessionState state;
        state.commands["test"]      = new TestCommand();
        state.commands["mouseMoved"]    = new MouseMovedCommand();
        
        std::cout << "before getRoutes creation" << std::endl;

        state.commands["getRoutes"]     = new GetRoutesCommand();
        state.commands["getBusses"]     = new GetBussesCommand();
        state.commands["getStops"]  = new GetStopsCommand();
        
        std::cout << "after command creation" << std::endl;

        WebServerWithState<MyWebServerSession, MyWebServerSessionState> server(state, port);
        while (true) {
            server.service();
        }
    }

    return 0;
}