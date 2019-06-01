export default function reducer(
	state = {
		allFriendsBadge: 0,
		pendingFriendsBadge: 0
	},
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

		default:
			return state;
	}
}
