

(function() {

  var mainExample, exampleOne, exampleTwo, exampleThree;

  //var colors = d3.scale.category20().range();

  var test_data = stream_layers(3,20 + Math.random()*50,.1).map(function(data, i) {
    return {
      key: 'Stream' + i
    , values: data
    //, color: colors[i]
    };
  });


  nv.addGraph(function() {  
    var chart = nv.models.lineChart()
                  .showLegend(true)
                  .margin({top: 10, bottom: 30, left: 40, right: 10});

    chart.xAxis
       .tickFormat(function(d) {
           return d3.time.format('%x')(new Date(d))
       });

    chart.yAxis
        .tickFormat(d3.format(',.1f'));

  var data;
  console.warn(test_data);

  d3.json("/data", function(error, json) {
    if (error) return console.warn(error);
    data = json;

  d3.select('#exampleOne')
      .datum(data)
      .transition().duration(500)
      .call(chart);
  });


    //TODO: Figure out a good way to do this automatically
    nv.utils.windowResize(chart.update);
    //nv.utils.windowResize(function() { d3.select('#chart1 svg').call(chart) });

    exampleOne = chart;

    return chart;
  });



})();
