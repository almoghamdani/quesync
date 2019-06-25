export function fetchUserProfile(client, userId) {
	return {
		type: "PROFILE_FETCH",
		payload: client.getUserProfile(userId)
	};
}

export function searchUser(client, nickname, tag = -1) {
	return {
		type: "SEARCH_USER",
		payload: client.search(nickname, tag)
	};
}