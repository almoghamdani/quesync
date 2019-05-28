export default class EventHandler {
	constructor(app) {
		this.app = app;
	}

	register = client => {
		client.registerEventHandler("friend-request", this.friendRequestEvent);
	};

	friendRequestEvent = event => {
		console.log(event);
	};
}
