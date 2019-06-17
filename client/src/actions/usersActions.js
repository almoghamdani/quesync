export function fetchUserProfile(client, userId) {
	return {
		type: "PROFILE_FETCH",
		payload: client.getUserProfile(userId)
	};
}
