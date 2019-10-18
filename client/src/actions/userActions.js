export function signIn(client, username, password) {
	return {
		type: "USER_LOGIN",
		payload: client.auth().login(username, password)
	};
}

export function register(client, username, email, password) {
	return {
		type: "USER_REGISTER",
		payload: client.auth().register(username, password, email)
	};
}

export function sessionAuth(client, sessionId) {
	return {
		type: "SESSION_AUTH",
		payload: client.auth().sessionAuth(sessionId)
	};
}

export function logout(client) {
	return {
		type: "LOGOUT",
		payload: client.auth().logout
	};
}

export function setUser(user) {
	return {
		type: "USER_SET",
		payload: { user }
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

export function approveFriendRequest(client, friendId) {
	return {
		type: "APPROVE_FRIEND_REQUEST",
		payload: client.users().setFriendshipStatus(friendId, true)
	};
}

export function rejectFriendRequest(client, friendId) {
	return {
		type: "REJECT_FRIEND_REQUEST",
		payload: client.users().setFriendshipStatus(friendId, false)
	};
}

export function sendFriendRequest(client, userId) {
	return {
		type: "SEND_FRIEND_REQUEST",
		payload: client.users().sendFriendRequest(userId)
	};
}

export function resetAuthError() {
	return {
		type: "RESET_AUTH_ERROR",
		payload: null
	};
}
