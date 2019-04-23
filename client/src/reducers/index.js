import { combineReducers } from "redux";

import auth from "./authReducer";
import client from "./clientReducer";

export default combineReducers({
	auth,
	client
});
