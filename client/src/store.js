import { applyMiddleware, createStore, compose } from "redux";
import { persistStore, persistReducer } from "redux-persist";
import storage from "redux-persist/lib/storage"; // localStorage

import logger from "redux-logger";
import promise from "redux-promise-middleware";
import thunk from "redux-thunk";

import reducers from "./reducers";

const isDev = window.require("electron-is-dev");

const composeEnhancers = window.__REDUX_DEVTOOLS_EXTENSION_COMPOSE__ || compose;
const middlewares = isDev
	? applyMiddleware(thunk, promise, logger)
	: applyMiddleware(thunk, promise);

const persistConfig = {
	key: "root",
	storage,
	whitelist: ["ui"]
};

const persistedReducer = persistReducer(persistConfig, reducers);

let store = createStore(persistedReducer, composeEnhancers(middlewares));
let persistor = persistStore(store);

export { store, persistor };
