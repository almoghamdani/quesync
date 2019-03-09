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
      connectError: "",

      usernameError: false,
      passwordError: false,
      loginError: ""
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
                easing: "easeInOutCirc",
                complete: () => {
                    // Re-enable the connect form
                    this.setState({
                        connecting: false
                    })
                }
            })
        } else {
            console.log("Connected to ", this.refs.serverIP.input_.value);

            // Save client in window
            window.client = electron.remote.getGlobal("client");

            // Disable all events to the connect form
            this.refs.connectForm.style.pointerEvents = "none"

            // Enable all events to the login form
            this.refs.loginForm.style.pointerEvents = ""

            // Make a fade out animation for the connect form
            anime({
                targets: ".quesync-connect-form",
                opacity: 0,
                duration: 800,
                easing: "easeInOutCirc",
                delay: 250,
                complete: () => {
                    // Re-enable the login form
                    this.setState({
                        connecting: false
                    })
                }
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
                easing: "easeInOutCirc",
                delay: 250
            })
        }
    })
  }

  loginBtnClicked(event)
  {
    // Disable the form's redirect
    event.preventDefault();

    // Reset errors
    this.setState({
        usernameError: false,
        passwordError: false
    });

    // If the username field is empty, set it as error
    if (this.refs.username.input_.value.length == 0)
    {
        // Set it as invalid
        this.setState({
            usernameError: true
        })
        return;
    } else if (this.refs.password.input_.value.length == 0) // If the password field is empty, set it as error
    {
        // Set it as invalid
        this.setState({
            passwordError: true
        })
        return;
    }

    // Set logging in as true to disable all input
    this.setState({
        loggingIn: true
    })

    // If there is an error, fade it out
    if (this.state.loginError)
    {
        // Make a fade out animation
        anime({
            targets: ".quesync-login-error",
            opacity: 0,
            duration: 200,
            easing: "easeInOutCirc"
        })
    }

    // Create an animation timeline for the title transition + loading animation
    var timeline = anime.timeline({
        duration: 1300,
        easing: "easeInOutCirc",
        delay: 250,
        complete: () => {
            // Try to login the user
            window.client.login(this.refs.username.input_.value, this.refs.password.input_.value)
                .then(({ user }) => {
                    console.log(user)
        
                    // Create a timeline for the transition
                    var timeline = anime.timeline({
                        duration: 1400,
                        easing: "easeInOutCirc",
                        delay: 250
                    })

                    // Animate the quesync title moving part to return to it's place
                    timeline.add({
                        targets: '.quesync-transition',
                        width: "100vw",
                        height: "100vh"
                    }, 400)

                    // Fade out the quesync title
                    timeline.add({
                        targets: '.quesync-transition-title',
                        opacity: "0"
                    }, 400)

                    // Fade out the loading indicator
                    timeline.add({
                        targets: '.quesync-loading',
                        opacity: "0",
                        duration: 400
                    }, 0)

                    // Return the title text to the center
                    timeline.add({
                        targets: '.quesync-title-text',
                        marginTop: "52px",
                        duration: 400,
                        complete: () => {
                            // Set transition opacity
                            this.refs.transition.style.opacity = 1
                        }
                    }, 0)
        
                    // Disable the loading indicator
                    this.setState({
                        loggingIn: false
                    })
                })
                .catch(({ error }) => {
                    // Show the error label
                    anime({
                        targets: ".quesync-login-error",
                        opacity: "1",
                        duration: 0
                    })

                    // User not found
                    if (error == window.errors.USER_NOT_FOUND) {
                        this.setState({
                            loginError: "The requested user is not found!",
                            usernameError: true
                        })
                    }
                    // Incorrect password 
                    else if (error == window.errors.INCORRECT_PASSWORD) {
                        this.setState({
                            loginError: "Incorrect password!",
                            passwordError: true
                        })
                    }
                    // Any other error
                    else {
                        this.setState({
                            loginError: "Unknown error occurred!\nPlease try again later."
                        })
                    }
        
                    // Create a timeline for the error animation
                    var timeline = anime.timeline({
                        duration: 800,
                        easing: "easeInOutCirc",
                        delay: 250,
                        complete: () => {
                            // Re-enable the login form
                            this.setState({
                                loggingIn: false
                            })
                        }
                    })

                    // Animate the quesync title moving part to return to it's place
                    timeline.add({
                        targets: '.quesync-title-moving',
                        width: "50%"
                    })

                    // Fade out the loading indicator
                    timeline.add({
                        targets: '.quesync-loading',
                        opacity: "0"
                    }, 0)

                    // Return the title text to the center
                    timeline.add({
                        targets: '.quesync-title-text',
                        marginTop: "52px"
                    }, 0)
                })
        }
    })

    // Add animation for the title to fill the menu
    timeline.add({
        targets: '.quesync-title-moving',
        width: "100%"
    })

    // Reset the title position to make space for the loading indicator
    timeline.add({
        targets: '.quesync-title-text',
        marginTop: "0px"
    }, 900)

    // Fade in the loading indicator
    timeline.add({
        targets: '.quesync-loading',
        opacity: "1"
    }, 900)
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
        <div className="quesync-transition-holder">
            <div className="quesync-transition quesync-title" ref="transition">
                <Typography className="quesync-transition-title" use="headline2" style={{ color: "white", userSelect: "none", opacity: "1" }}>Quesync</Typography>
            </div>
        </div>
        <Elevation className="quesync-start-menu" z="8" style={{ pointerEvents: this.state.connecting || this.state.loggingIn ? "none": "" }}>
            <div className="quesync-form-side quesync-title" />
            <div className="quesync-form-side quesync-title quesync-title-moving">
                <Typography className="quesync-title-text" use="headline2" style={{ color: "white", userSelect: "none", marginTop: "55px" }}>Quesync</Typography>
                <CircularProgress className="quesync-loading" theme="secondary" style={{ marginTop: "38px", opacity: "0" }} />
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
                    <div className="quesync-error-holder">
                        <Typography className="quesync-connect-error" use="caption" style={{ color: "#ff1744", paddingTop: "40px", userSelect: "none", whiteSpace: "pre-line", lineHeight: "12px", opacity: "0" }}>{this.state.connectError}</Typography>
                    </div>
                </form>
                <form className="quesync-form quesync-login-form" ref="loginForm" onSubmit={this.loginBtnClicked} style={{ opacity: "0", pointerEvents: "none" }}>
                    <Typography use="headline2" style={{ color: "var(--mdc-theme-primary)", userSelect: "none" }}>Login</Typography>
                    <TextField invalid={this.state.usernameError} outlined label="Username" ref="username" style={{ marginTop: "38px", width: "300px" }} />
                    <TextField invalid={this.state.passwordError} outlined label="Password" ref="password" type="password" style={{ marginTop: "15px", width: "300px" }} />
                    <Button type="submit" raised style={{ marginTop: "35px", width: "300px" }} theme={['secondary']}>
                    Login
                    </Button>
                    <Button raised style={{ marginTop: "15px", width: "300px", background: "#00A8E8" }} onClick={this.connectBtnClicked}>
                    Don't have an account yet?
                    </Button>
                    <div className="quesync-error-holder">
                        <Typography className="quesync-login-error" use="caption" style={{ color: "#ff1744", paddingTop: "25px", userSelect: "none", whiteSpace: "pre-line", lineHeight: "12px", opacity: "0" }}>{this.state.loginError}</Typography>
                    </div>
                </form>
            </div>
        </Elevation>
      </ThemeProvider>
    );
  }
}

class ParticlesNoUpdate extends Component
{
    shouldComponentUpdate()
    {
        return false;
    }

    render() 
    {
        return (
            <Particles className="quesync-particles" ref="particles" params={this.props.params} style={this.props.style} />
        )
    }
}

export default StartPage;
