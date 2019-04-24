export default function reducer(
	state = {
		profiles: {},
		profileFetchError: null
	},
	action
) {
	switch (action.type) {
		case "PROFILE_FETCH_PENDING":
			return { ...state, profileFetchError: null };

		case "PROFILE_FETCH_REJECTED":
			return { ...state, profileFetchError: action.payload.error };

		case "PROFILE_FETCH_FULFILLED":
            var profile = { ...action.payload.profile };
			var id = profile.id;

			// Delete the id from the profile
			delete profile.id;

			return { ...state, profiles: { ...state.profiles, [id]: profile } };

		default:
			return state;
	}
}
