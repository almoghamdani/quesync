export default function reducer(
  state = {
    loggingIn: false,
    signingUp: false,
    user: null,
    authError: null
  },
  action
) {
  switch (action.type) {
    case "USER_LOGIN_PENDING":
      return { ...state, loggingIn: true, authError: null };

    case "USER_LOGIN_REJECTED":
      return { ...state, loggingIn: false, authError: action.payload.error };

    case "USER_LOGIN_FULFILLED":
      return { ...state, loggingIn: false, user: action.payload.user };

    case "USER_SIGNUP_PENDING":
      return { ...state, signingUp: true, authError: null };

    case "USER_SIGNUP_REJECTED":
      return { ...state, signingUp: false, authError: action.payload.error };

    case "USER_SIGNUP_FULFILLED":
      return { ...state, signingUp: false, user: action.payload.user };

    default:
      return state;
  }
}
