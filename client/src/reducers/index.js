import { combineReducers } from "redux";

import user from "./userReducer";
import client from "./clientReducer";

export default combineReducers({
	user,
	client
});
