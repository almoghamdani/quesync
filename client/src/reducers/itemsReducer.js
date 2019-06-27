const INITIAL_STATE = {
	selectedFriendsPageTab: 0,
	selectedFriendsPageDrawerItemId: null
}

export default function reducer(
	state = INITIAL_STATE,
	action
) {
	switch (action.type) {
		case "SET_FRIENDS_PAGE_TAB":
			return { ...state, selectedFriendsPageTab: action.payload };

		case "SET_FRIENDS_PAGE_DRAWER_ITEM_ID":
			return { ...state, selectedFriendsPageDrawerItemId: action.payload };

		case "RESET_UI":
			return INITIAL_STATE;

		default:
			return state;
	}
}
