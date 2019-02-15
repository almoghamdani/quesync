quesync = require("./build/Release/quesync.node")

var smth = new quesync.Client()
try {
    smth.connect("127.0.0.1");
    console.log(smth.login("almogh52", "123456789"));
} catch (ex)
{
    console.log(ex)
}