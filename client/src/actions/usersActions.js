export function fetchUserProfile(client, user_id) {
	return {
		type: "PROFILE_FETCH",
		payload: client.getUserProfile(user_id)
	};
}
