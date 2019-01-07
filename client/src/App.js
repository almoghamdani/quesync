import React, { Component } from 'react';
import './App.css';
import { TextField } from '@rmwc/textfield';
import 'material-components-web/dist/material-components-web.css'

class App extends Component {
  render() {
    return (
      <div className="App" style={{ width: "100vw", height: "100vh", display: "flex", justifyContent: "center", alignItems: "center" }}>
        <div className="login-div" style={{ width: "350px", display: "flex", flexDirection: "column" }} >
          <TextField outlined label="E-Mail" />
          <TextField outlined label="Password" type="password" style={{ marginTop: "18px" }} />
        </div>
      </div>
    );
  }
}

export default App;
