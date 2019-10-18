export function getPrivateChannel(userId) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "GET_PRIVATE_CHANNEL",
			payload: client.channels().getPrivateChannel(userId)
		});
	}
}
