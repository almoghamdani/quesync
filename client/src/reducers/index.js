import { combineReducers } from "redux";

import auth from "./authReducer";
import client from "./clientReducer";
import users from "./usersReducer";
import tabs from "./tabsReducer";
import badges from "./badgesReducer";

export default combineReducers({
	auth,
	client,
	users,
	ui: combineReducers({
		tabs,
		badges
	})
});
