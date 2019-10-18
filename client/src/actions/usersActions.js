export function fetchUserProfile(userId) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "PROFILE_FETCH",
			payload: client.users().getUserProfile(userId)
		});
	}
}

export function searchUser(nickname, tag = -1) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "SEARCH_USER",
			payload: client.users().search(nickname, tag)
		});
	}
}