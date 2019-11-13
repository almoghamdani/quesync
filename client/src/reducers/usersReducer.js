import imageType from "image-type";

const INITIAL_STATE = {
	profiles: {},
	searchResults: [],
	searching: false
};

const DEFAULT_PROFILE_PHOTO = "https://cdn.iconscout.com/icon/free/png-256/user-avatar-contact-portfolio-personal-portrait-profile-6-5623.png";

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

			// If the user has no profile pic
			if (!profile.photo) {
				profile.photo = DEFAULT_PROFILE_PHOTO;
			} else {
				// Get the type of the photo
				const photoBuffer = Buffer.from(profile.photo, "base64");
				const photoType = imageType(photoBuffer);

				// If the photo is a valid img, format it as base64
				if (photoType) {
					profile.photo = `data:${photoType.mime};base64, ${profile.photo}`;
				} else {
					profile.photo = DEFAULT_PROFILE_PHOTO;
				}
			}

			return { ...state, profiles: { ...state.profiles, [id]: profile } };

		case "SEARCH_USER_PENDING":
			return { ...state, searching: true };

		case "SEARCH_USER_REJECTED":
			return { ...state, searching: false };

		case "SEARCH_USER_FULFILLED":
			var searchResults = [...action.payload.searchResults];

			// Parse profile photo for each result
			searchResults.forEach(profile => {
				// If the user has no profile pic
				if (!profile.photo) {
					profile.photo = DEFAULT_PROFILE_PHOTO;
				} else {
					// Get the type of the photo
					const photoBuffer = Buffer.from(profile.photo, "base64");
					const photoType = imageType(photoBuffer);

					// If the photo is a valid img, format it as base64
					if (photoType) {
						profile.photo = `data:${photoType.mime};base64, ${profile.photo}`;
					} else {
						profile.photo = DEFAULT_PROFILE_PHOTO;
					}
				}
			})

			return { ...state, searchResults, searching: false }

		case "LOGOUT_FULFILLED":
		case "LOGOUT_REJECTED":
		case "CLEAN_CONNECTION":
			return INITIAL_STATE;

		default:
			return state;
	}
}
