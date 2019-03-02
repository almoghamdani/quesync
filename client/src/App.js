import React, { Component } from 'react';
import './App.css';
import { TextField } from '@rmwc/textfield';
import { Button, ButtonIcon } from '@rmwc/button';
import { ThemeProvider } from '@rmwc/theme';
import { CircularProgress } from '@rmwc/circular-progress';
import 'material-components-web/dist/material-components-web.css'
import '@rmwc/circular-progress/circular-progress.css'

const electron = window.require("electron")

class App extends Component {
  /**
   * This function is the constructor of the App class
   * 
   * @param props The props to pass to the component
   * @return None
   */
  constructor(props)
  {
    // Initialize the component super-class
    super(props);

    // Set initial state of component
    this.state = { 
      connecting: false,
      usernameError: false,
      passwordError: false,
      serverIPError: false
    }

    // Make 'this' work in the event funcion
    this.loginClicked = this.loginClicked.bind(this);
  }

  /**
   * This function is an event function that will be called when the login button was clicked.
   * 
   * @return None
   */
  loginClicked()
  {
    // Reset errors
    this.setState({
        usernameError: false,
        passwordError: false,
        serverIPError: false
    });

    // If username empty, set it as invalid
    if (this.refs.username.input_.value.length == 0)
    {
        // Set it as invalid
        this.setState({
            usernameError: true
        })
        return;
    }
    // If password empty, set it as invalid
    else if (this.refs.password.input_.value.length == 0)
    {
        // Set it as invalid
        this.setState({
            passwordError: true
        })
        return;
    }
    // If server IP empty or not matching the validation pattern, set it as invalid
    else if (this.refs.serverIP.input_.value.length == 0 || 
        !/^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$/.test(this.refs.serverIP.input_.value))
    {
        // Set it as invalid
        this.setState({
            serverIPError: true
        })
        return;
    }

    // Set connecting as true to disable all input
    this.setState({
      connecting: true
    })

    // Call the login event with the server IP to connect to
    var {error} = electron.ipcRenderer.sendSync('client-connect', this.refs.serverIP.input_.value)

    // If an error occurred, print it
    if (error)
    {
        console.log(error);
    } else {
        console.log("Connected to ", this.refs.serverIP.input_.value);

        // Save client in window
        window.client = electron.remote.getGlobal("client");

        // Try to login with the username and password
        var {error, user} = window.client.login(this.refs.username.input_.value, this.refs.password.input_.value);

        console.log(user)
    }
  }

  render() {
    return (
      <ThemeProvider className="App" options={{ primary: "#ff3d00", secondary:"white" }} style={{ width: "100vw", height: "100vh", display: "flex", justifyContent: "center", alignItems: "center" }}>
        <div className="login-div" style={{ width: "350px", display: "flex", flexDirection: "column" }} >
          <TextField invalid={this.state.usernameError} outlined label="Username" ref="username" />
          <TextField invalid={this.state.passwordError} outlined label="Password" ref="password" type="password" style={{ marginTop: "18px" }} />
          <TextField invalid={this.state.serverIPError} outlined label="Server IP" ref="serverIP" style={{ marginTop: "18px" }} pattern="^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$" />
          <div style={{ display: "flex", flexDirection: "row", justifyContent: "space-between" }}>
            <Button raised style={{ marginTop: "18px", width: "160px" }} onClick={this.loginClicked}>
              {
                this.state.connecting ? <ButtonIcon icon={<CircularProgress theme="secondary" />}/> : null
              }
              Login
            </Button>
            <Button raised style={{ marginTop: "18px", width: "160px", backgroundColor: "#00b0ff" }}>Register</Button>
          </div>
        </div>
      </ThemeProvider>
    );
  }
}

export default App;
