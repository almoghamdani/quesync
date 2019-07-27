var quesync = require("./build/Release/backend.node")

var smth = new quesync.Client()

smth.registerEventHandler("friend-request", (j) => console.log(j))

smth.connect("127.0.0.1")
    .then(() => {
        smth.login("almogh52", "123456789")
            .then(() => {
                smth.getPrivateChannel("530e9407-7cfd-4be4-9cc6-70ab38e6310c")
                    .then(res => {
                        console.log(res)                  

						smth.call(res.channel.id)
                        smth.sendMessage("😀", res.channel.id)
                            .then(() => {
                                smth.getChannelMessages(res.channel.id, 100, 0)
                                .then((res2) => {
                                    console.log("Sdsd");
                                    console.log(res2)
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

                setInterval(() => {

                }, 1000)
            })
            .catch((ex) => {
                console.log(ex)
            })
    })
    .catch((ex) => {
        console.log(ex)
    })