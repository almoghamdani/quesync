export function setFriendsPageSelectedTab(tab) {
    return {
		type: "SET_FRIENDS_PAGE_TAB",
		payload: tab
	};
}

export function setFriendsPageSelectedDrawerItem(item) {
    return {
		type: "SET_FRIENDS_PAGE_DRAWER_ITEM",
		payload: item
	};
}