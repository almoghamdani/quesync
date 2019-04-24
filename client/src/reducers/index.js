import { combineReducers } from "redux";

import auth from "./authReducer";
import client from "./clientReducer";
import users from './usersReducer';

export default combineReducers({
	auth,
    client,
    users
});
