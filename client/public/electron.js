const electron = require("electron");
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;

const path = require("path");
const isDev = require("electron-is-dev");

const queryString = require("query-string");

// Find the backend library and load it
const backendName = "Release/backend.node";
const quesync = require(isDev
	? `../backend/build/${backendName}`
	: app.getAppPath() + `/../../backend/backend.node`);

let mainWindow, serverIPWindow;
let callWindows = {};

// Create a new backend client
let client = new quesync.Client();

let serverIP = null;

// Set app user model id to app id to fix desktop notifications on windows
app.setAppUserModelId("com.almoghamdani.quesync");

// Remove menu on all windows on production
if (!isDev)
	electron.Menu.setApplicationMenu(null);

electron.ipcMain.on("create-call-window", (_, callDetails) => {
	const query = queryString.stringify(callDetails);

	// Create the call window
	callWindows[callDetails.id] = new BrowserWindow({
		width: 270,
		height: 390,
		frame: false,
		webPreferences: { nodeIntegration: true },
		fullscreenable: false,
		resizable: false,
		transparent: true,
		hasShadow: false,
		center: true,
		alwaysOnTop: true,
		show: false,
		parent: mainWindow
	});

	// Disable resize
	callWindows[callDetails.id].fullscreenable = false;
	callWindows[callDetails.id].maximizable = false;
	callWindows[callDetails.id].resizable = false;

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

	callWindows[callDetails.id].on(
		"ready-to-show",
		callWindows[callDetails.id].show
	);
});

electron.ipcMain.on("close-call-window", (_, channelId) => {
	// If there is a window with the call id
	if (channelId in callWindows) {
		const window = callWindows[channelId];

		// Remove the window from the call windows
		delete callWindows[channelId];

		// Close the window
		window.close();
	}
});

electron.ipcMain.on("close-all-call-windows", () => {
	const callWindowsCopy = { ...callWindows };

	if (callWindowsCopy) {
		// Erase all the call windows object
		callWindows = {};

		// Close all call windows
		Object.keys(callWindowsCopy).forEach(callId => {
			callWindowsCopy[callId].close();
		});
	}
});

electron.ipcMain.on("call-main-window-event", (_, { name, arg }) =>
	mainWindow.webContents.send(name, arg)
);

electron.ipcMain.on("save-server-ip", (_, ip) => {
	// If the Server IP Window is open
	if (serverIPWindow) {
		// Save the Server's IP
		serverIP = ip;

		// Create the main window
		createMainWindow();

		// Close the Server's IP Window
		serverIPWindow.close();
	}
}
);

function createServerIPWindow() {
	// Create the Server IP window
	serverIPWindow = new BrowserWindow({
		width: 450,
		height: 300,
		webPreferences: { nodeIntegration: true },
		fullscreenable: false,
		resizable: false,
		center: true,
		show: false
	});

	// Load the dev url if electron ran on dev or load the static html file when electron is running in production
	serverIPWindow.loadURL(
		isDev
			? "http://localhost:3000?serverip"
			: `file://${path.join(__dirname, "../build/index.html?serverip")}`
	);

	// On the close event release the window from the memory
	serverIPWindow.on("closed", () => (serverIPWindow = null));

	// Only show the window when it's ready
	serverIPWindow.on("ready-to-show", serverIPWindow.show);
}

function createMainWindow() {
	// Create a new browser window with a fixed initial size
	mainWindow = new BrowserWindow({
		width: 1000,
		height: 700,
		titleBarStyle: "hidden",
		frame: false,
		webPreferences: { nodeIntegration: true },
		fullscreenable: false,
		show: false
	});

	// Load the dev url if electron ran on dev or load the static html file when electron is running in production
	mainWindow.loadURL(
		isDev
			? "http://localhost:3000"
			: `file://${path.join(__dirname, "../build/index.html")}`
	);

	// On the close event release the window from the memory
	mainWindow.on("closed", () => (mainWindow = null));

	// Only show the window when it's ready
	mainWindow.on("ready-to-show", mainWindow.show);

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

	// Set the client as a global var
	global.client = client;

	// Set the Server's IP as a global var
	global.serverIP = serverIP;
}

// When the electron app is ready, create the browser window
app.on("ready", () => {
	// If in dev mode, connect to local IP
	if (isDev) {
		// Set Server's IP as localhost
		serverIP = "127.0.0.1";

		// Create the Main Window
		createMainWindow();
	} else {
		// Create the Server IP Window to get the IP from the user
		createServerIPWindow();
	}
});

// If all the windows are closed, on windows, quit app
app.on("window-all-closed", () => {
	// Close apps when all windows are closed
	app.quit();
});

app.on("quit", () => {
	// Destroy the client
	client.destroy();
	delete client;
})
