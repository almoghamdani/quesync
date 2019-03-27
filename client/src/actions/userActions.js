export function signIn(client, username, password) {
    return {
        type: "USER_LOGIN",
        payload: client.login(username, password)
    }
}