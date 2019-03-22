import { applyMiddleware, createStore } from "redux";

import { createLogger } from "redux-logger";
import promise from "redux-promise-middleware";

import reducers from "./reducers";

const middlewares = applyMiddleware(promise, createLogger());

export default createStore(reducers, middlewares);
