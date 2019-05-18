export default function reducer(
	state = {
		selectedFriendsPageTab: 0
	},
	action
) {
	switch (action.type) {
		case "SET_FRIENDS_PAGE_TAB":
			return { ...state, selectedFriendsPageTab: action.payload };

		default:
			return state;
	}
}
