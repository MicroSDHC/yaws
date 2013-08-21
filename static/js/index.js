
(function() {

  var mychart;

  // nv.addGraph(function() {  
  //   var chart = nv.models.lineChart()
  //                 .showLegend(true)
  //                 .margin({top: 10, bottom: 30, left: 40, right: 10});

  //   chart.xAxis
  //      .tickFormat(function(d) {
  //          console.warn(d);
  //          return d3.time.format('%X')(new Date(d*1000))
  //      });

  //   chart.yAxis
  //       .tickFormat(d3.format(',r'));

  // mychart = chart;

  // d3.json("/data", function(error, json) {
  //   //if (error) return console.warn(error);
  //   //alert(error);
  //   console.info(error);

  //   // d3.select('#exampleOne')
  //   //     .datum(error)
  //   //     .transition().duration(500)
  //   //     .call(chart);
  // });


  //   nv.utils.windowResize(chart.update);
  //   return chart;
  // });

var graph = new Rickshaw.Graph({
        element: document.querySelector("#exampleOne"),
        renderer: 'line',
        series: [{
                data: [ { x: 0, y: 40 }, { x: 1, y: 49 }],
                color: 'steelblue'
        }]
});
 
graph.render();


  updateSensors = function(values) {
      d3.select("#temp").html(values.temp + " C");
      d3.select("#humidity").html(values.humidity + " %");
      d3.select("#indoor_temp").html(values.temp + " C");
      d3.select("#indoor_humidity").html(values.humidity + " %");
      d3.select("#pressure").html(values.pressure + " mm");
      var datetime = new Date(values.time);
      d3.select("#time").html(datetime.getUTCHours() + ":" + datetime.getUTCMinutes());
    // d3.json("/data", function(error, json) {
    //   //if (error) return console.warn(error);
    //   //alert(error);
    //   //console.info(error);

    //   d3.select('#exampleOne')
    //       .datum(error)
    //       .transition().duration(500)
    //       .call(mychart);
    // });
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
