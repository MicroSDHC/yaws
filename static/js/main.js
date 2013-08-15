updateSensors = function(values) {
    $("#temp").html(values.temp + " C");
    $("#humidity").html(values.humidity + " %");
    $("#indoor_temp").html(values.temp + " C");
    $("#indoor_humidity").html(values.humidity + " %");
    $("#pressure").html(values.pressure + " mm");
    var datetime = new Date(values.time);
    $("#time").html(datetime.getUTCHours() + ":" + datetime.getUTCMinutes());
}

$(document).ready(function() {
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
});