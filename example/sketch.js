var socket = new WebSocket("ws://" + location.hostname+(location.port ? ':'+location.port: ''), "web_server");

var count = 0;
var size = 80;
var color = {r: 0, g: 0, b: 0, a: 0}
var id;
var connected = false;

try {
	socket.onmessage =function got_packet(msg) {
		if (!connected) {
			id = +msg.data;
			connected = true;
		}
		count++;
		console.log(count, JSON.parse(msg.data));
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


socket.onopen = function() {
    socket.send(JSON.stringify({command: "test"}));
   // connected = true;


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

  //socket.send(JSON.stringify({command: "mouseMoved", mouseX: mouseX, mouseY: mouseY}));
}

function mouseClicked() {
	var output = ''
	for (var i = 0; i < 5000; i++) {
		output += i + ', ';
	}

	//socket.send(JSON.stringify({command: "sdf", mouseX: output}));

	$.ajax({
		type: "POST",
		url: "/post/"+id,
		//data: JSON.stringify({command: "mouseClicked", output: output}),
		data: JSON.stringify({command: "mouseMoved", mouseX: mouseX, mouseY: mouseY, output: output}),
		success: function(res) { console.log(res); },
		//error: function(res) { console.log(res); },
		dataType: "json"
	  });

}