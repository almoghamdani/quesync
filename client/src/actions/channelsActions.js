export function getPrivateChannel(client, userId) {
	return {
		type: "GET_PRIVATE_CHANNEL",
		payload: client.channels().getPrivateChannel(userId)
	};
}
