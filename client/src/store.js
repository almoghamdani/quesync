import { applyMiddleware, createStore, compose } from "redux";
import { persistStore, persistReducer } from 'redux-persist'
import storage from 'redux-persist/lib/storage' // localStorage

import { createLogger } from "redux-logger";
import promise from "redux-promise-middleware";
import thunk from 'redux-thunk';

import reducers from "./reducers";

const composeEnhancers = window.__REDUX_DEVTOOLS_EXTENSION_COMPOSE__ || compose;
const middlewares = applyMiddleware(thunk, promise, createLogger());

const persistConfig = {
	key: 'root',
	storage,
	whitelist: ['ui']
}

const persistedReducer = persistReducer(persistConfig, reducers)

let store = createStore(persistedReducer, composeEnhancers(middlewares))
let persistor = persistStore(store)

export { store, persistor }
