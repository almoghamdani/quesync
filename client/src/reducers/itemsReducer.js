export default function reducer(
	state = {
		selectedFriendsPageTab: 0,
		selectedFriendsPageDrawerItem: -1
	},
	action
) {
	switch (action.type) {
		case "SET_FRIENDS_PAGE_TAB":
			return { ...state, selectedFriendsPageTab: action.payload };

		case "SET_FRIENDS_PAGE_DRAWER_ITEM":
			return { ...state, selectedFriendsPageDrawerItem: action.payload };

		default:
			return state;
	}
}
