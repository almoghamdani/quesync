export default function reducer(
	state = {
		selectedFriendsPageTab: 0,
		selectedFriendsPageDrawerItemId: null
	},
	action
) {
	switch (action.type) {
		case "SET_FRIENDS_PAGE_TAB":
			return { ...state, selectedFriendsPageTab: action.payload };

		case "SET_FRIENDS_PAGE_DRAWER_ITEM_ID":
			return { ...state, selectedFriendsPageDrawerItemId: action.payload };

		default:
			return state;
	}
}
