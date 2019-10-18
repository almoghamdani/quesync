const INITIAL_STATE = {
	profiles: {},
	searchResults: [],
	searching: false,
	error: null,
	searchError: null
};

export default function reducer(
	state = INITIAL_STATE,
	action
) {
	switch (action.type) {
		case "PROFILE_FETCH_PENDING":
			return { ...state, error: null };

		case "PROFILE_FETCH_REJECTED":
			return { ...state, error: action.payload.error };

		case "PROFILE_FETCH_FULFILLED":
			var profile = { ...action.payload.profile };
			var id = profile.id;

			// Delete the id from the profile
			delete profile.id;

			return { ...state, profiles: { ...state.profiles, [id]: profile } };

		case "SEARCH_USER_PENDING":
			return { ...state, searchError: null, searching: true };

		case "SEARCH_USER_REJECTED":
			return { ...state, searchError: action.payload.error, searching: false };

		case "SEARCH_USER_FULFILLED":
			return { ...state, searchResults: action.payload.searchResults, searching: false }

		case "LOGOUT_FULFILLED":
		case "LOGOUT_REJECTED":
			return INITIAL_STATE;

		default:
			return state;
	}
}
