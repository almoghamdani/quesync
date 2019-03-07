import React, { Component } from 'react';

class Layout extends Component
{
    render()
    {
        return (
            <div className="quesync-layout" style={{ maxHeight: "100%", maxWidth: "100%" }}>
                <div className="quesync-drag-zone" style={{ position: "absolute", top:"0", left: "0", height: "20px", minWidth: "100%", webkitAppRegion: "drag", background: "black", zIndex: "100" }}>
                </div>
                {this.props.child}
            </div>
        )
    }
}

export default Layout;