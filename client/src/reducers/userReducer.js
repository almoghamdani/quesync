export default function reducer(
  state = {
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

    default:
      return state;
  }
}
