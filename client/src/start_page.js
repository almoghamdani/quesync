import React, { Component } from 'react';
import './start_page.scss';
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
    this.loginBtnClicked = this.loginBtnClicked.bind(this);
  }

  componentWillMount()
  {
    // Save client's errors in window
    window.errors = electron.remote.getGlobal("errors");
  }

  /**
   * This function is an event function that will be called when the connect button was clicked.
   * 
   * @return None
   */
  connectBtnClicked(event)
  {
    // Disable the form's redirect
    event.preventDefault();

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

    // If there is an error, fade it out
    if (this.state.connectError)
    {
        // Make a fade out animation
        anime({
            targets: ".quesync-connect-error",
            opacity: 0,
            duration: 200,
            easing: "easeInOutCirc"
        })
    }

    // Call the login event with the server IP to connect to
    electron.ipcRenderer.send('client-connect', this.refs.serverIP.input_.value)

    electron.ipcRenderer.on("client-connect-callback", (event, error) => {
        // If an error occurred, print it
        if (error)
        {
            // If the error is that the client couldn't connect to the server, set the error label
            if (error === window.errors.CANNOT_REACH_SERVER)
            {
                this.setState({ connectError: "Unable to reach the server!\nPlease make sure you have an active internet connection." });
            } else if (error == window.errors.NO_CONNECTION) { // If the client has no internet connection
                this.setState({ connectError: "You don't have an active internet connection!" });
            } else {
                this.setState({ connectError: "An unknown error occurred!\nPlease make sure you have an active internet connection." });
            }

            // Fade in the connect error
            anime({
                targets: ".quesync-connect-error",
                opacity: 1,
                duration: 200,
                easing: "easeInOutCirc"
            })

            // Disable the loading indicator
            this.setState({
                connecting: false
            })
        } else {
            console.log("Connected to ", this.refs.serverIP.input_.value);

            // Save client in window
            window.client = electron.remote.getGlobal("client");

            // Disable all events to the connect form
            this.refs.connectForm.style.pointerEvents = "none"

            // Enable all events to the login form
            this.refs.loginForm.style.pointerEvents = ""

            // Disable the loading indicator
            this.setState({
                connecting: false
            })

            // Make a fade out animation for the connect form
            anime({
                targets: ".quesync-connect-form",
                opacity: 0,
                duration: 800,
                easing: "easeInOutCirc",
                delay: 250
            })

            // Make a fade in animation for the login form
            anime({
                targets: ".quesync-login-form",
                opacity: 1,
                duration: 800,
                easing: "easeInOutCirc",
                delay: 250
            })

            // Make the form holder height bigger
            anime({
                targets: ".quesync-form-holder",
                height: "27rem",
                duration: 800,
                easing: "easeInOutCirc"
            })
        }
    })
  }

  loginBtnClicked(event)
  {
    // Disable the form's redirect
    event.preventDefault();

    // Set logging in as true to disable all input
    this.setState({
        loggingIn: true
    })

    window.client.login(this.refs.username.input_.value, this.refs.password.input_.value)
        .then(({ user }) => {
            console.log(user)

            // Disable the loading indicator
            this.setState({
                loggingIn: false
            })
        })
        .catch(({ error }) => {
            console.log(error)

            // Disable the loading indicator
            this.setState({
                loggingIn: false
            })
        })
  }

  render() {
    return (
      <ThemeProvider className="quesync-start-page" options={{ primary: "#007EA7", secondary:"#e0e0e0" }}>
        <ParticlesNoUpdate className="quesync-bg-particles" params={{
            "particles": {
                "number": {
                    "value": 80,
                    "density": {
                        "enable": true,
                        "value_area": 800
                    }
                },
                "color": {
                    "value": "#ffffff"
                },
                "shape": {
                    "type": "circle",
                    "stroke": {
                        "width": 0,
                        "color": "#000000"
                    },
                    "polygon": {
                        "nb_sides": 5
                    },
                    "image": {
                        "src": "img/github.svg",
                        "width": 100,
                        "height": 100
                    }
                },
                "opacity": {
                    "value": 1,
                    "random": true,
                    "anim": {
                        "enable": true,
                        "speed": 1,
                        "opacity_min": 0,
                        "sync": false
                    }
                },
                "size": {
                    "value": 3,
                    "random": true,
                    "anim": {
                        "enable": false,
                        "speed": 4,
                        "size_min": 0.3,
                        "sync": false
                    }
                },
                "line_linked": {
                    "enable": false,
                },
                "move": {
                    "enable": true,
                    "speed": 0.3,
                    "direction": "top",
                    "random": true,
                    "straight": false,
                    "out_mode": "out",
                    "bounce": false,
                }
            }
        }} style={{ position: "absolute", top: "0", left: "0", minWidth: "100%", minHeight: "100%", zIndex: "-1" }}/>
        <Elevation className="quesync-start-menu" z="8">
            <div className="quesync-form-side quesync-title">
                <Typography use="headline2" style={{ color: "white", userSelect: "none" }}>Quesync</Typography>
            </div>
            <div className="quesync-form-side quesync-form-holder" style={{ width: "25rem", height: "20rem" }}>
                <form className="quesync-form quesync-connect-form" ref="connectForm" onSubmit={this.connectBtnClicked}>
                    <Typography use="headline2" style={{ color: "var(--mdc-theme-primary)", userSelect: "none" }}>Server</Typography>
                    <TextField invalid={this.state.serverIPError} outlined label="Server IP" ref="serverIP" style={{ marginTop: "38px", width: "300px" }} pattern="^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$" />
                    <Button type="submit" raised style={{ marginTop: "25px", width: "300px" }} theme={['secondary']}>
                    {
                        this.state.connecting ? <ButtonIcon icon={<CircularProgress theme="secondary" />}/> : null
                    }
                    Connect
                    </Button>
                    <div style={{ width: "100%", height: "15px", display: "flex", justifyContent: "center", alignItems: "center", textAlign: "center" }}>
                        <Typography className="quesync-connect-error" ref="connectErrorLbl" use="caption" style={{ color: "#ff1744", paddingTop: "40px", userSelect: "none", whiteSpace: "pre-line", lineHeight: "12px", opacity: "0" }}>{this.state.connectError}</Typography>
                    </div>
                </form>
                <form className="quesync-form quesync-login-form" ref="loginForm" onSubmit={this.loginBtnClicked} style={{ opacity: "0", pointerEvents: "none" }}>
                    <Typography use="headline2" style={{ color: "var(--mdc-theme-primary)" }}>Login</Typography>
                    <TextField outlined label="Username" ref="username" style={{ marginTop: "38px", width: "300px" }} />
                    <TextField type="password" outlined label="Password" ref="password" style={{ marginTop: "15px", width: "300px" }} />
                    <Button type="submit" raised style={{ marginTop: "35px", width: "300px" }} theme={['secondary']}>
                    {
                        this.state.loggingIn ? <ButtonIcon icon={<CircularProgress theme="secondary" />}/> : null
                    }
                    Login
                    </Button>
                    <Button raised style={{ marginTop: "15px", width: "300px", background: "#00A8E8" }} onClick={this.connectBtnClicked}>
                    Don't have an account yet?
                    </Button>
                </form>
            </div>
        </Elevation>
      </ThemeProvider>
    );
  }
}

class ParticlesNoUpdate extends Component
{
    constructor(props)
    {
        super(props);

        // Make 'this' work in the event funcion
        this.onResize = this.onResize.bind(this);
    }

    shouldComponentUpdate()
    {
        return false;
    }

    onResize()
    {
        console.log(this.refs.particles)
    }

    render() 
    {
        return (
            <Particles className="quesync-particles" ref="particles" params={this.props.params} style={this.props.style} />
        )
    }
}

export default StartPage;
