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

export function connected() {
	return {
		type: "CONNECTED",
		payload: null
	};
}

export function disconnected() {
	return {
		type: "DISCONNECTED",
		payload: null
	};
}

export function cleanConnection() {
	return {
		type: "CLEAN_CONNECTION",
		payload: null
	};
}
