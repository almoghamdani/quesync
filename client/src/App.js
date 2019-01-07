import React, { Component } from 'react';
import './App.css';
import { TextField } from '@rmwc/textfield';
import { Button, ButtonIcon } from '@rmwc/button';
import { ThemeProvider } from '@rmwc/theme';
import { CircularProgress } from '@rmwc/circular-progress';
import 'material-components-web/dist/material-components-web.css'
import '@rmwc/circular-progress/circular-progress.css'

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
      connecting: false
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
    this.setState({
      connecting: true
    })
  }

  render() {
    return (
      <ThemeProvider className="App" options={{ primary: "#ff3d00", secondary:"white" }} style={{ width: "100vw", height: "100vh", display: "flex", justifyContent: "center", alignItems: "center" }}>
        <div className="login-div" style={{ width: "350px", display: "flex", flexDirection: "column" }} >
          <TextField outlined label="E-Mail" />
          <TextField outlined label="Password" type="password" style={{ marginTop: "18px" }} />
          <TextField outlined label="Server IP" style={{ marginTop: "18px" }} pattern="^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$" />
          <div style={{ display: "flex", flexDirection: "row", justifyContent: "space-between" }}>
            <Button raised style={{ marginTop: "18px", width: "160px" }} onClick={this.loginClicked}>
              {
                this.state.connecting ? <ButtonIcon icon={<CircularProgress theme="secondary" />}/> : null
              }
              Login
            </Button>
            <Button raised style={{ marginTop: "18px", width: "160px" }}>Register</Button>
          </div>
        </div>
      </ThemeProvider>
    );
  }
}

export default App;
