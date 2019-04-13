export function signIn(client, username, password) {
	return {
		type: "USER_LOGIN",
		payload: client.login(username, password)
	};
}

export function startAuth() {
	return {
		type: "AUTH_STARTED",
		payload: null
	};
}

export function finishAuth() {
	return {
		type: "AUTH_FINISHED",
		payload: null
	};
}