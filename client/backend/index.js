var quesync = require("./build/Release/quesync.node")

var smth = new quesync.Client()
smth.connect("127.0.0.1")
    .then(() => {
        smth.login("almogh52", "123456789")
            .then(() => {
                smth.getUserProfile("530e9407-7cfd-4be4-9cc6-70ab38e6310c")
                    .then((res) => {
                        console.log(res)
                    })
                    .catch((ex) => {
                        console.log(ex)
                    })
            })
            .catch((ex) => {
                console.log(ex)
            })
    })
    .catch((ex) => {
        console.log(ex)
    })