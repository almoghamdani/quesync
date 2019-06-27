const INITIAL_STATE = {
	allFriendsBadge: 0,
	pendingFriendsBadge: 0
}

export default function reducer(
	state = INITIAL_STATE,
	action
) {
	switch (action.type) {
		case "INC_ALL_FRIENDS_BADGE":
			return { ...state, allFriendsBadge: state.allFriendsBadge + 1 };

		case "INC_PENDING_FRIENDS_BADGE":
			return { ...state, pendingFriendsBadge: state.pendingFriendsBadge + 1 };

		case "DEC_ALL_FRIENDS_BADGE":
			return { ...state, allFriendsBadge: state.allFriendsBadge - 1 };

		case "DEC_PENDING_FRIENDS_BADGE":
			return { ...state, pendingFriendsBadge: state.pendingFriendsBadge - 1 };

		case "RESET_UI":
			return INITIAL_STATE;

		default:
			return state;
	}
}
