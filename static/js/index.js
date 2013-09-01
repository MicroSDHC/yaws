
(function() {


// var graph = new Rickshaw.Graph({
//         element: document.querySelector("#exampleOne"),
//         renderer: 'line',
//         series: [{
//                 data: [ { x: 0, y: 40 }, { x: 1, y: 49 }],
//                 color: 'steelblue'
//         }]
// });
 
// graph.render();

  updateSensors = function(values) {
      d3.select("#temp").html(values.remote.OUTDOOR.TEMP + " C");
      d3.select("#humidity").html(values.remote.OUTDOOR.HUM + " %");
      d3.select("#indoor_temp").html(values.local.TEMP + " C");
      //d3.select("#indoor_humidity").html(values.humidity + " %");
      d3.select("#pressure").html(values.local.PRESSURE + " mm");
      var datetime = new Date(values.time);
      d3.select("#time").html(datetime.getUTCHours() + ":" + datetime.getUTCMinutes());
  }

  var socket = io.connect('/shouts');
  socket.on('connect', function() {
      console.log("socket connected");
  });
  socket.on('disconnect', function() {
      console.log("socket disconnected");
  });

  socket.on('sensors_update', function(data) {
      console.log("Got message:", data);
      updateSensors(data);
  });



})();
