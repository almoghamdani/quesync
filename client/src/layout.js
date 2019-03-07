import React, { Component } from 'react';
import './layout.scss';
import { IconButton } from '@rmwc/icon-button';

const electron = window.require("electron")
const BrowserWindow = electron.remote.BrowserWindow
const currentWindow = BrowserWindow.getFocusedWindow()

class Layout extends Component
{
    close()
    {
        // Blur active element
        document.activeElement.blur();

        // Close the current window
        currentWindow.close()
    }

    maximize()
    {
        // Blur active element
        document.activeElement.blur();

        // Maximize or unmaximize the current window by the current maximized state
        currentWindow.isMaximized() ? currentWindow.unmaximize() : currentWindow.maximize()
    }

    minimize()
    {
        // Blur active element
        document.activeElement.blur();

        // Minimize the current window
        currentWindow.minimize()
    }

    render()
    {
        return (
            <div className="quesync-layout">
                <div className="quesync-drag-holder">
                    <div className="quesync-drag-zone" />
                    <IconButton className="quesync-drag-zone-button" icon="minimize" onClick={this.minimize}/>
                    <IconButton className="quesync-drag-zone-button" icon="fullscreen" onClick={this.maximize}/>
                    <IconButton className="quesync-drag-zone-button" icon="close" onClick={this.close}/>
                </div>
                {this.props.child}
            </div>
        )
    }
}

export default Layout;