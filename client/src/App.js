import React, { Component } from 'react';
import './App.css';
import { TextField } from '@rmwc/textfield';
import 'material-components-web/dist/material-components-web.css'

class App extends Component {
  render() {
    return (
      <div className="App">
        <TextField outlined label="E-Mail" />
      </div>
    );
  }
}

export default App;
