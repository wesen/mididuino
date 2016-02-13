Raphael.fn.createEncoder = function (i) {
  var encoder = this.circle(55 + (i * 70), 160, 18);
  encoder.attr({fill: "#000",
                      stroke: "#000"});
  $(encoder.node).mouseover(function (ev) {
    encoder.attr({fill:"red"});
  }).mouseout(function (ev) {
    encoder.attr({fill: "black"});
  });
};

$(function () {
  var paper = Raphael(10, 50, 320, 300);
  var body = paper.rect(10, 10, 300, 190);
  body.attr({stroke: "#000",
             "stroke-linejoin": "round",
             "stroke-width": 2});

  var lcd = paper.rect(65, 35, 180, 70);
  lcd.attr({stroke: "#000",
             "stroke-linejoin": "round",
             "stroke-width": 1});

  var encoders = [];
  for (var i = 0; i < 4; i++) {
    encoders[i] = paper.createEncoder(i);
  }
});