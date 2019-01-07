import React, { Component } from 'react';
import './App.css';
import { TextField } from '@rmwc/textfield';
import { Button } from '@rmwc/button';
import { ThemeProvider, Theme } from '@rmwc/theme';
import 'material-components-web/dist/material-components-web.css'

class App extends Component {
  render() {
    return (
      <ThemeProvider className="App" options={{ primary: "blue" }} style={{ width: "100vw", height: "100vh", display: "flex", justifyContent: "center", alignItems: "center" }}>
        <div className="login-div" style={{ width: "350px", display: "flex", flexDirection: "column" }} >
          <TextField outlined label="E-Mail" />
          <TextField outlined label="Password" type="password" style={{ marginTop: "18px" }} />
          <TextField outlined label="Server IP" style={{ marginTop: "18px" }} />
          <Button raised style={{ marginTop: "18px" }}>Login</Button>
        </div>
      </ThemeProvider>
    );
  }
}

export default App;
