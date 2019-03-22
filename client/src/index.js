import React from "react";
import ReactDOM from "react-dom";
import { Provider } from "react-redux";

import Layout from "./layout";
import StartPage from "./start_page";
import store from './store'
import "./index.scss";

const app = document.getElementById("root");

ReactDOM.render(
  <Provider store={store}>
    <Layout child={<StartPage />} />
  </Provider>,
  app
);
