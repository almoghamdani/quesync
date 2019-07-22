const electron = require("electron");
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;

const path = require("path");
const isDev = require("electron-is-dev");

const os = require("os");
const backend_name =
	os.platform() === "darwin" ? "backend.node" : "Release/backend.node";
const quesync = require(isDev
	? `../backend/build/${backend_name}`
	: app.getAppPath() + `/../../backend.node`);

// Set the main window as a global var
let mainWindow;

// Create a new backend client
var client = new quesync.Client();

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
