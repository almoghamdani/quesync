export function setFriendsPageSelectedTab(tab) {
    return {
		type: "SET_FRIENDS_PAGE_TAB",
		payload: tab
	};
}

export function setFriendsPageSelectedDrawerItemId(id) {
    return {
		type: "SET_FRIENDS_PAGE_DRAWER_ITEM_ID",
		payload: id
	};
}