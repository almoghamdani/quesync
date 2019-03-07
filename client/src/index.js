import React from 'react';
import ReactDOM from 'react-dom';
import './index.scss';
import Layout from './layout';
import StartPage from './start_page';

ReactDOM.render(<Layout child={<StartPage />} />, document.getElementById('root'));
