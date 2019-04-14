import React from "react";
import ReactDOM from "react-dom";
import { Provider } from "react-redux";

// Must be here to compile the material components SASS first
import "./index.scss";

import Layout from "./layout.jsx";
import StartPage from "./start_page.jsx";
import store from './store'

const app = document.getElementById("root");

ReactDOM.render(
  <Provider store={store}>
    <Layout child={<StartPage />} />
  </Provider>,
  app
);
