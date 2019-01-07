import React, { Component } from 'react';
import './App.css';
import { TextField } from '@rmwc/textfield';
import 'material-components-web/dist/material-components-web.css'

class App extends Component {
  render() {
    return (
      <div className="App">
        <div className="login-div" style={{ width: "350px", display: "flex", flexDirection: "column" }} >
          <TextField outlined label="E-Mail" />
          <TextField outlined label="Password" type="password" />
        </div>
      </div>
    );
  }
}

export default App;
