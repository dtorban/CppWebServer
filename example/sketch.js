var socket;
var connected;

var count;
var size;
var color;
var busses;
var stops;
var routes;


function Position(x, y) {
    this.x = x;
    this.y = y;
}
function Bus(id, position) {
    this.id = id;
    this.position = position;
}
function Stop(id, position, numPeople) {
    this.id = id;
    this.position = position;
    this.numPeople = numPeople;
}
function Route(id, positions) {
    this.id = id;
    this.positions = positions;
}


function setupSocket() {
    try {
        socket.onmessage =function got_packet(msg) {
            count++;
            //console.log(count, JSON.parse(msg.data));
            var data = JSON.parse(msg.data);
            if (data.command == "updateElipse") {   
                size = data.size;
                color.r = data.color.r;
                color.g = data.color.g;
                color.b = data.color.b;
                color.a = data.color.a; 
            }
            if (data.command == "updateBusses") {
                
            }
            if (data.command == "updateStops") {
                
            }
            if (data.command == "updateRoutes") {
                
                routes = [];
                
                for (let i = 0; i < data.routes.length; i++) {
                    id = data.routes[i].id;
                    positions = [];

                    for (let j = 0; j < data.routes[i].positions.length; j++) {
                        x = data.routes[i].positions[j].x;
                        y = data.routes[i].positions[j].y;

                        positions.push(new Position(x, y));
                    }
                    routes.push(new Route(id, positions));
                }
            }
        } 
    } catch(exception) {
        alert('<p>Error' + exception);  
    }

    connected = false;

    socket.onopen = function() {
        socket.send(JSON.stringify({command: "test"}));
        connected = true;
    }
}


/// * * * * * P5 functions * * * * * ///

function setup() {
    socket = new WebSocket("ws://127.0.0.1:8081", "web_server");
    setupSocket();

    count  = 0;
    size   = 80;
    color  = {r: 0, g: 0, b: 0, a: 0}
    busses = [];
    stops  = [];
    routes = [];
    
    //createCanvas(640, 480);
    createCanvas(windowWidth,windowHeight);
}

function draw() {
    if (connected) {
        update();
    }
    render();
}

function update() {
    socket.send(JSON.stringify({command: "getRoutes"}));
    socket.send(JSON.stringify({command: "getBusses"}));
    socket.send(JSON.stringify({command: "getStops"}));
}

function render() {
    background(128);

    fill(color.r, color.g, color.b, color.a);
    ellipse(mouseX, mouseY, size, size);

    // Send request to server to get text info
    // Write our text we've gotten
    
    for (let i = 0; i < routes.length; i++) {
        for (let j = 0; j < routes[i].positions.length; j++) {
        x = routes[i].positions[j].x;
        y = routes[i].positions[j].y;
            
        ellipse(x, y, 50, 50);
        }
    }

}

function mouseMoved() {
    socket.send(JSON.stringify({command: "mouseMoved", mouseX: mouseX, mouseY: mouseY}));
}