import React from "react";
import ReactDOM from "react-dom";
import { Provider } from "react-redux";

// Must be here to compile the material components SASS first
import "./index.scss";

import Layout from "./layout";
import StartPage from "./start_page";

import { PersistGate } from "redux-persist/integration/react";
import { store, persistor } from "./store";

import queryString from "query-string";

import { setClient } from "./actions/clientActions";
import eventHandler from "./event_handler";

const electron = window.require("electron");

const callingWindow = window.location.search.includes("calling");

const app = document.getElementById("root");

if (callingWindow) {
	import("./call_window").then(module => {
		const CallWindow = module.default;

		ReactDOM.render(
			<CallWindow {...queryString.parse(window.location.search)} />,
			app
		);
	});
} else {
	ReactDOM.render(
		<Provider store={store}>
			<PersistGate loading={null} persistor={persistor} onBeforeLift={() => {
				// Get the client from the global vars
				var client = electron.remote.getGlobal("client");

				// Clear old event handlers if exist
				client.clearAllEventHandlers();

				// Save the errors object in the window to be accessible for all
				window.errors = client.errors();

				// Set the client in the store
				store.dispatch(setClient(client));

				// Register the event handler
				eventHandler.register(client);
			}}>
				<Layout child={<StartPage />} />
			</PersistGate>
		</Provider>,
		app
	);
}
