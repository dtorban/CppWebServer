	var socket = new WebSocket("ws://127.0.0.1:8081", "web_server");
	
	

	/*d3.select("#command").on("keydown", function() {
		if (d3.event.keyCode == 13) {
			var command = d3.select("#command").node().value;
			socket.send(JSON.stringify({command: command}));
			d3.select("#command").node().value = '';
		}
		
	});

	d3.select("#time").on("input", function() {
		var time = d3.select("#time").node().value/1000.0;
		//console.log(time);
		if (currentSim) {
			socket.send(JSON.stringify({command: "moveToTime", time: time*(+currentSim.total_time)}));
			socket.send(JSON.stringify({command: "cellFull", time: time*(+currentSim.total_time)}));
		}
	});*/

	/*var count = 0;

	try {
		socket.onmessage =function got_packet(msg) {
			count++;
			console.log(count, JSON.parse(msg.data));
			//d3.select("#output").html(msg.data);
			var data = JSON.parse(msg.data);
		} 
	} catch(exception) {
		alert('<p>Error' + exception);  
	}*/

	socket.onopen = function() {
		console.log("again");
	    socket.send(JSON.stringify({command: "test"}))
	  }

function testThis() {
	//socket = new WebSocket("ws://127.0.0.1:8081", "web_server");
	//socket.send(JSON.stringify({command: "test"}))
	console.log(socket);
}

function setup() {
  testThis();
}

function draw() {
  ellipse(50, 50, 80, 80);
  //console.log(socket, JSON.stringify({command: "test1"}));
			//socket.send(JSON.stringify({command: "test"}));
}
