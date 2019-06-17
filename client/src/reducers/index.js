import { combineReducers } from "redux";

import auth from "./authReducer";
import client from "./clientReducer";
import users from "./usersReducer";
import tabs from "./tabsReducer";
import badges from "./badgesReducer";
import channels from "./channelsReducer";
import messages from "./messagesReducer";

export default combineReducers({
	auth,
	client,
	users,
	channels,
	messages,
	ui: combineReducers({
		tabs,
		badges
	})
});
