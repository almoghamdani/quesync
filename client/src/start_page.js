import React, { Component } from 'react';
import './start_page.css';
import { Typography } from '@rmwc/typography';
import { TextField } from '@rmwc/textfield';
import { Button, ButtonIcon } from '@rmwc/button';
import { ThemeProvider } from '@rmwc/theme';
import { CircularProgress } from '@rmwc/circular-progress';
import { Elevation } from '@rmwc/elevation';

import '@rmwc/circular-progress/circular-progress.css'

const electron = window.require("electron")

class StartPage extends Component {
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

      serverIPError: false
    }

    // Make 'this' work in the event funcion
    this.connectBtnClicked = this.connectBtnClicked.bind(this);
  }

  /**
   * This function is an event function that will be called when the connect button was clicked.
   * 
   * @return None
   */
  connectBtnClicked()
  {
    // Reset errors
    this.setState({
        serverIPError: false
    });

    // If server IP empty or not matching the validation pattern, set it as invalid
    if (this.refs.serverIP.input_.value.length == 0 || 
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
    }
  }

  render() {
    return (
      <ThemeProvider className="App" options={{ primary: "#00b0ff", secondary:"#e0e0e0" }} style={{ width: "100vw", height: "100vh", display: "flex", justifyContent: "center", alignItems: "center" }}>
        {/*<div className="login-div" style={{ width: "350px", display: "flex", flexDirection: "column" }} >
          <TextField invalid={this.state.usernameError} outlined label="Username" ref="username" />
          <TextField invalid={this.state.passwordError} outlined label="Password" ref="password" type="password" style={{ marginTop: "18px" }} />
          
          <div style={{ display: "flex", flexDirection: "row", justifyContent: "space-between" }}>
            <Button raised style={{ marginTop: "18px", width: "160px" }} onClick={this.loginClicked}>
              {
                this.state.connecting ? <ButtonIcon icon={<CircularProgress theme="secondary" />}/> : null
              }
              Login
            </Button>
            <Button raised style={{ marginTop: "18px", width: "160px", backgroundColor: "#00b0ff" }}>Register</Button>
          </div>
        </div>*/}

        <Elevation style={{ width: "25rem", height: "20rem", background: "var(--mdc-theme-secondary)" }} z="8">
            <div className="connect-form" style={{ minHeight: "100%", display: "flex", flexDirection: "column", alignItems: "center", justifyContent: "center" }}>
                <Typography use="headline2" style={{ color: "var(--mdc-theme-primary)" }}>Quesync</Typography>
                <TextField invalid={this.state.serverIPError} outlined label="Server IP" ref="serverIP" style={{ marginTop: "50px", width: "300px" }} pattern="^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$" />
                <Button raised style={{ marginTop: "25px", width: "300px" }} theme={['secondary']} onClick={this.connectBtnClicked}>
                {
                    this.state.connecting ? <ButtonIcon icon={<CircularProgress theme="secondary" />}/> : null
                }
                Connect
                </Button>
            </div>
        </Elevation>
      </ThemeProvider>
    );
  }
}

export default StartPage;
