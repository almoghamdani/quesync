export function signIn(username, password) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "USER_LOGIN",
			payload: client.auth().login(username, password)
		});
	}
}

export function register(username, email, password) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "USER_REGISTER",
			payload: client.auth().register(username, password, email)
		});
	}
}

export function sessionAuth(sessionId) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "SESSION_AUTH",
			payload: client.auth().sessionAuth(sessionId)
		});
	}
}

export function logout() {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "LOGOUT",
			payload: client.auth().logout
		});
	}
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

export function approveFriendRequest(friendId) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "APPROVE_FRIEND_REQUEST",
			payload: client.users().setFriendshipStatus(friendId, true)
		});
	}
}

export function rejectFriendRequest(friendId) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "REJECT_FRIEND_REQUEST",
			payload: client.users().setFriendshipStatus(friendId, false)
		});
	}
}

export function sendFriendRequest(userId) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "SEND_FRIEND_REQUEST",
			payload: client.users().sendFriendRequest(userId)
		});
	}
}

export function removeFriend(friendId) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "REMOVE_FRIEND",
			payload: client.users().setFriendshipStatus(friendId, false)
		});
	}
}

export function resetAuthError() {
	return {
		type: "RESET_AUTH_ERROR",
		payload: null
	};
}
