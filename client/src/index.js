import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
const electron = window.require("electron")

electron.ipcRenderer.send("login-event", "sdsd");

ReactDOM.render(<App />, document.getElementById('root'));
