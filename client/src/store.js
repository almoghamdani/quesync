import { applyMiddleware, createStore, compose } from "redux";

import { createLogger } from "redux-logger";
import promise from "redux-promise-middleware";

import reducers from "./reducers";

const composeEnhancers = window.__REDUX_DEVTOOLS_EXTENSION_COMPOSE__ || compose;
const middlewares = applyMiddleware(promise, createLogger());

export default createStore(reducers, composeEnhancers(middlewares));
