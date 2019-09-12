var socket = new WebSocket("ws://127.0.0.1:8081", "web_server");

var count = 0;
var size = 80;
var color = {r: 0, g: 0, b: 0, a: 0}

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
	} 
} catch(exception) {
	alert('<p>Error' + exception);  
}

var connected = false;

socket.onopen = function() {
    socket.send(JSON.stringify({command: "test"}));
    connected = true;
  }

// P5 functions
function setup() {
	//createCanvas(640, 480);
	createCanvas(windowWidth,windowHeight);
}

function draw() {
  background(128); 
  fill(color.r, color.g, color.b, color.a)
  ellipse(mouseX, mouseY, size, size);
}

function mouseMoved() {
  socket.send(JSON.stringify({command: "mouseMoved", mouseX: mouseX, mouseY: mouseY}));
}