export default function reducer(
	state = {
		authenticating: false,
		user: null,
		authError: null
	},
	action
) {
	switch (action.type) {
		case "USER_SIGNUP_PENDING":
		case "USER_LOGIN_PENDING":
			return { ...state, authError: null };

		case "USER_SIGNUP_REJECTED":
		case "USER_LOGIN_REJECTED":
			return { ...state, authError: action.payload.error };

		case "USER_SIGNUP_FULFILLED":
		case "USER_LOGIN_FULFILLED":
			return { ...state, user: action.payload.user };

		case "AUTH_STARTED":
			return { ...state, authenticating: true };

		case "AUTH_FINISHED":
			return { ...state, authenticating: false };

		default:
			return state;
	}
}
