export function getPrivateChannel(client, userId) {
	return {
		type: "GET_PRIVATE_CHANNEL",
		payload: client.getPrivateChannel(userId)
	};
}
