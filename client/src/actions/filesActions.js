export function startUpload(filePath) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "FILE_START_UPLOAD",
			payload: client.files().startUpload(filePath)
		});
	}
}

export function updateFileTransmissionProgress(fileId, progress) {
	return {
		type: "FILE_TRANSMISSION_PROGRESS_UPDATE",
		payload: {
			fileId, progress
		}
	}
}
