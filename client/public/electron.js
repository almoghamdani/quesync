const electron = require("electron");
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;

const path = require("path");
const os = require("os");
const isDev = require("electron-is-dev");

const queryString = require("query-string");

// Find the backend library and load it
const backend_name =
	os.platform() === "darwin" ? "backend.node" : "Release/backend.node";
const quesync = require(isDev
	? `../backend/build/${backend_name}`
	: app.getAppPath() + `/../../backend.node`);

// Set the main window as a global var
let mainWindow;

// Create a new backend client
let client = new quesync.Client();

let callWindows = {};

electron.ipcMain.on("create-call-window", (_, callDetails) => {
	const query = queryString.stringify(callDetails);

	// Create the call window
	callWindows[callDetails.id] = new BrowserWindow({
		width: 270,
		height: 390,
		titleBarStyle: "hidden",
		frame: false,
		webPreferences: { nodeIntegration: true },
		fullscreenable: false,
		resizable: false,
		transparent: true,
		center: true,
		alwaysOnTop: true,
		parent: mainWindow
	});

	// Disable resize
	callWindows[callDetails.id].setFullScreenable(false);
	callWindows[callDetails.id].setMaximizable(false);
	callWindows[callDetails.id].setResizable(false);

	// Load the calling window's content using the query
	callWindows[callDetails.id].loadURL(
		isDev
			? "http://localhost:3000?calling&" + query
			: `file://${path.join(__dirname, "../build/index.html?calling&" + query)}`
	);

	callWindows[callDetails.id].on("close", evt => {
		// If the window should still be open, keep it open
		if (callWindows[callDetails.id]) evt.preventDefault();
	});
});

electron.ipcMain.on("close-call-window", (_, channelId) => {
	const callWindow = callWindows[channelId];

	// If there is a window with the call id
	if (callWindow) {
		callWindows[channelId] = undefined;

		// Close the window
		callWindow.close();
	}
});

electron.ipcMain.on("close-all-call-windows", () => {
	const callWindowsCopy = { ...callWindows };

	if (callWindowsCopy) {
		// Erase all the call windows object
		callWindows = {};

		// Close all call windows
		Object.keys(callWindowsCopy).forEach(callId => {
			callWindowsCopy[callWindowsCopy].close();
		});
	}
});

electron.ipcMain.on("call-main-window-event", (_, { name, arg }) =>
	mainWindow.webContents.send(name, arg)
);

function createWindow() {
	// Create a new browser window with a fixed initial size
	mainWindow = new BrowserWindow({
		width: 900,
		height: 680,
		titleBarStyle: "hidden",
		frame: false,
		webPreferences: { nodeIntegration: true },
		fullscreenable: false
	});

	// Load the dev url if electron ran on dev or load the static html file when electron is running in production
	mainWindow.loadURL(
		isDev
			? "http://localhost:3000"
			: `file://${path.join(__dirname, "../build/index.html")}`
	);

	// On the close event release the window from the memory
	mainWindow.on("closed", () => (mainWindow = null));

	// If in dev mode, install devTools
	if (isDev) {
		const {
			default: installExtension,
			REACT_DEVELOPER_TOOLS,
			REDUX_DEVTOOLS
		} = require("electron-devtools-installer");

		installExtension(REACT_DEVELOPER_TOOLS)
			.then(name => console.log(`Added Extension:  ${name}`))
			.catch(err => console.log("An error occurred: ", err));

		installExtension(REDUX_DEVTOOLS)
			.then(name => console.log(`Added Extension:  ${name}`))
			.catch(err => console.log("An error occurred: ", err));
	}

	// Set the client and it's errors as a global vars
	global.client = client;
	global.client.errors = quesync.errors;
}

// When the electron app is ready, create the browser window
app.on("ready", createWindow);

// If all the windows are closed, on windows, quit app
app.on("window-all-closed", () => {
	// Close apps when all windows are closed
	app.quit();
});

// If the app activated and the main window isn't created, create the window
app.on("activate", () => {
	// If the main window is null, create it
	if (mainWindow === null) {
		createWindow();
	}
});
