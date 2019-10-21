const INITIAL_STATE = {
	files: {},
	filesProgress: {},
	downloading: [],
	uploading: []
};

export default function reducer(
	state = INITIAL_STATE,
	action
) {
	switch (action.type) {
		case "FILE_START_UPLOAD_FULFILLED":
			{
				const file = { ...action.payload.file };
				const fileId = file.id;

				// Remove the id from the file object
				delete file.id;

				return {
					...state,
					files: { ...state.files, [fileId]: file },
					filesProgress: {
						...state.filesProgress,
						[fileId]: state.filesProgress[fileId] ? state.filesProgress[fileId] : 0
					},
					uploading: [...state.uploading, fileId]
				};
			}

		case "FILE_START_DOWNLOAD_FULFILLED":
			{
				const { fileId } = action.payload;

				return {
					...state,
					downloading: [...state.downloading, fileId],
					filesProgress: {
						...state.filesProgress,
						[fileId]: state.filesProgress[fileId] ? state.filesProgress[fileId] : 0
					}
				}
			}

		case "STOP_FILE_TRANSMISSION_FULFILLED": {
			const { fileId } = action.payload;

			let downloading = [...state.downloading];
			let uploading = [...state.uploading];

			// If the file is currently downloading, remove it from the downloads's list
			if (downloading.includes(fileId)) {
				downloading = downloading.filter(id => id !== fileId);
			}

			// If the file is currently uploading, remove it from the uploads's list
			if (uploading.includes(fileId)) {
				uploading = uploading.filter(id => id !== fileId);
			}

			return {
				...state,
				downloading,
				uploading
			}
		}

		case "FILE_TRANSMISSION_PROGRESS_UPDATE":
			{
				const { fileId, progress } = action.payload;
				const file = state.files[fileId];

				let downloading = [...state.downloading];
				let uploading = [...state.uploading];

				// If the file is being downloaded and the download is finished remove it as been downloaded
				if (downloading.includes(fileId) && file && file.size === progress) {
					downloading = downloading.filter(id => id !== fileId);
				}

				// If the file is being uploading and the upload is finished remove it as been uploaded
				if (uploading.includes(fileId) && file && file.size === progress) {
					uploading = uploading.filter(id => id !== fileId);
				}

				return {
					...state,
					downloading,
					uploading,
					filesProgress: { ...state.filesProgress, [fileId]: progress }
				};
			}

		case "GET_FILE_INFO_FULFILLED":
			{
				const file = { ...action.payload.file };
				const fileId = file.id;

				// Delete the file's id
				delete file.id;

				return {
					...state,
					files: { ...state.files, [fileId]: file }
				}

			}

		case "LOGOUT_FULFILLED":
		case "LOGOUT_REJECTED":
		case "CLEAN_CONNECTION":
			return INITIAL_STATE;

		default:
			return state;
	}
}
