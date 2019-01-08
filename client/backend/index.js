quesync = require("./build/Release/quesync.node")

var smth = new quesync.Client()
try {
    smth.connect("127.0.0.1");
} catch (ex)
{
    console.log(ex)
}

//console.log(smth)

function idk(hex)
{

}

setInterval(idk, 10, smth)