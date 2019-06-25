export function signIn(client, username, password) {
	return {
		type: "USER_LOGIN",
		payload: client.login(username, password)
	};
}

export function register(client, username, email, password) {
	return {
		type: "USER_REGISTER",
		payload: client.register(username, password, email)
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
		payload: client.setFriendshipStatus(friendId, true)
	}
}

export function rejectFriendRequest(client, friendId) {
	return {
		type: "REJECT_FRIEND_REQUEST",
		payload: client.setFriendshipStatus(friendId, false)
	}
}

export function sendFriendRequest(client, userId) {
	return {
		type: "SEND_FRIEND_REQUEST",
		payload: client.sendFriendRequest(userId)
	}
}