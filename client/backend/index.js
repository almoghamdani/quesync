quesync = require("./build/Release/quesync.node")

var smth = new quesync.Client()
smth.connect("127.0.0.1");
//console.log(smth)

function idk(hex)
{

}

setInterval(idk, 10, smth)