(function() {

  updateSensors = function(values) {
      d3.select("#temp").html(values.remote.OUTDOOR.TEMP + "&deg C");
      d3.select("#humidity").html("Humidity: " + values.remote.OUTDOOR.HUM + "%");
      d3.select("#indoor").html("Indoor: " + values.local.TEMP + "&deg C");
      d3.select("#pressure").html("Pressure: " + values.local.PRESSURE + " mm");
      var datetime = moment(values.time);
      d3.select("#time").html(datetime.format('HH:mm'));
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

  d3.json("/data", function(error, json) {
      console.log(json);
      updateSensors(json);
  });

})();
