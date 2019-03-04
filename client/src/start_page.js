import React, { Component } from 'react';
import './start_page.css';
import { Typography } from '@rmwc/typography';
import { TextField } from '@rmwc/textfield';
import { Button, ButtonIcon } from '@rmwc/button';
import { ThemeProvider } from '@rmwc/theme';
import { CircularProgress } from '@rmwc/circular-progress';
import { Elevation } from '@rmwc/elevation';
import anime from 'animejs/lib/anime.es.js';
import Particles from 'react-particles-js';

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
      loggingIn: false,

      serverIPError: false,
      connectError: ""
    }

    // Make 'this' work in the event funcion
    this.connectBtnClicked = this.connectBtnClicked.bind(this);
  }

  componentDidMount()
  {
      // Add an animation for the background that will run in loops
      anime({
          targets: "body",
          direction: 'alternate',
          loop: true,
          easing: 'easeInOutQuad',
          backgroundPosition: 100,
          duration: 7500
      })
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
    //var {error} = electron.ipcRenderer.sendSync('client-connect', this.refs.serverIP.input_.value)

    var error = 0

    // If an error occurred, print it
    if (error)
    {
        // If the error is that the client couldn't connect to the server, set the error label
        if (error === "10061")
        {
            this.setState({ connectError: "Unable to find the Quesync Server!" });
        }
    } else {
        console.log("Connected to ", this.refs.serverIP.input_.value);

        // Save client in window
        window.client = electron.remote.getGlobal("client");

        // Disable all events to the connect form
        this.refs.connectForm.style.pointerEvents = "none"

        // Enable all events to the login form
        this.refs.loginForm.style.pointerEvents = ""

        // Make a fade out animation
        anime({
            targets: ".connect-form",
            opacity: 0,
            duration: 800,
            easing: "easeInOutCirc",
            delay: 250
        })

        // Make a fade out animation
        anime({
            targets: ".login-form",
            opacity: 1,
            duration: 800,
            easing: "easeInOutCirc",
            delay: 250
        })

        // Make the form holder height bigger
        anime({
            targets: ".form-holder",
            height: "27rem",
            duration: 800,
            easing: "easeInOutCirc"
        })
    }
  }

  render() {
    return (
      <ThemeProvider className="App" options={{ primary: "#00b0ff", secondary:"#e0e0e0" }} style={{ width: "100vw", height: "100vh", display: "flex", justifyContent: "center", alignItems: "center" }}>
        <Particles params={{
            "particles": {
                "number": {
                    "value": 50, 
                    "density": {
                        "enable": true,
                        "value_area": 500
                    }
                },
                "move": {
                    "enable": true,
                    "speed": 1.8,
                    "direction": "none",
                    "random": true,
                    "straight": false,
                    "out_mode": "out",
                    "bounce": false
                }
            },
            "interactivity": {
                "detect_on": "canvas",
                "events": {
                    "onhover": {
                        "enable": true,
                        "mode": "grab"
                    },
                    "resize": true
                },
                "modes": {
                    "grab": {
                        "distance": 180,
                        "line_linked": {
                            "opacity": 1
                        }
                    }
                }
            }
        }} style={{ position: "absolute", top: "0", left: "0", minWidth: "100%", minHeight: "100%" }}/>
        <Elevation className="form-holder" style={{ position: "relative", width: "25rem", height: "20rem", background: "var(--mdc-theme-secondary)" }} z="8">
            <div className="connect-form" ref="connectForm" style={{ position: "absolute", minWidth: "100%", minHeight: "100%", display: "flex", flexDirection: "column", alignItems: "center", justifyContent: "center" }}>
                <Typography use="headline2" style={{ color: "var(--mdc-theme-primary)" }}>Quesync</Typography>
                <TextField invalid={this.state.serverIPError} outlined label="Server IP" ref="serverIP" style={{ marginTop: "50px", width: "300px" }} pattern="^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$" />
                <Button raised style={{ marginTop: "25px", width: "300px" }} theme={['secondary']} onClick={this.connectBtnClicked}>
                {
                    this.state.connecting ? <ButtonIcon icon={<CircularProgress theme="secondary" />}/> : null
                }
                Connect
                </Button>
                <Typography ref="connectErrorLbl" use="body2" style={{ color: "#ff1744", paddingTop: "8px" }}>{this.state.connectError}</Typography>
            </div>

            <div className="login-form" ref="loginForm" style={{ position: "absolute", minWidth: "100%", minHeight: "100%", opacity: "0", pointerEvents: "none", display: "flex", flexDirection: "column", alignItems: "center", justifyContent: "center" }}>
                <Typography use="headline2" style={{ color: "var(--mdc-theme-primary)" }}>Login</Typography>
                <TextField outlined label="Username" style={{ marginTop: "50px", width: "300px" }} />
                <TextField outlined label="Password" style={{ marginTop: "25px", width: "300px" }} />
                <Button raised style={{ marginTop: "25px", width: "300px" }} theme={['secondary']} onClick={this.connectBtnClicked}>
                {
                    this.state.loggingIn ? <ButtonIcon icon={<CircularProgress theme="secondary" />}/> : null
                }
                Login
                </Button>
                <Button raised style={{ marginTop: "25px", width: "300px", background: "#ff9100" }} theme={['secondary']} onClick={this.connectBtnClicked}>
                Don't have an account yet?
                </Button>
            </div>
        </Elevation>
      </ThemeProvider>
    );
  }
}

export default StartPage;
