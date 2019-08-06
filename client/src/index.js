import React from "react";
import ReactDOM from "react-dom";
import { Provider } from "react-redux";

// Must be here to compile the material components SASS first
import "./index.scss";

import Layout from "./layout";
import StartPage from "./start_page";
import CallWindow from "./call_window";

import { PersistGate } from "redux-persist/integration/react";
import { store, persistor } from "./store";

import queryString from "query-string";

const callingWindow = window.location.search.includes("calling");

const app = document.getElementById("root");

if (callingWindow) {
	ReactDOM.render(
		<CallWindow {...queryString.parse(window.location.search)} />,
		app
	);
} else {
	ReactDOM.render(
		<Provider store={store}>
			<PersistGate loading={null} persistor={persistor}>
				<Layout child={<StartPage />} />
			</PersistGate>
		</Provider>,
		app
	);
}
