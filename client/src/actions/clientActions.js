export function setClient(client) {
	return {
		type: "CLIENT_OBJECT",
		payload: client
	};
}

export function addPingValue(pingValue) {
	return {
		type: "ADD_PING_VALUE",
		payload: pingValue
	};
}
