import React from "react";

import { queue } from "./notifications_queue";

const textForError = error => {
	switch (error) {
		case window.errors.success:
			return "";
		case window.errors.cannot_reach_server:
		case window.errors.no_connection:
			return "Cannot reach server";
		case window.errors.voice_not_connected:
			return "You are not connected to a voice channel";
		case window.errors.call_already_started:
			return "The call already started";
		case window.errors.file_size_exceeded_max:
			return "The size of the file exceedes the max file size";
		case window.errors.empty_file:
			return "Cannot upload an empty file";
		case window.errors.file_not_found:
			return "The file couldn't be found on the server";
		case window.errors.invalid_download_file_path:
			return "The download path is invalid";
		case window.errors.profile_photo_too_big:
			return "The size of the profile photo is too big";
		case window.errors.unknown_error:
		default:
			return "Unknown Error";
	}
}

const errorMiddleware = _ => next => action => {
	let result = next(action);

	if (!(action.type.includes("AUTH")
		|| action.type.includes("LOGIN")
		|| action.type.includes("REGISTER"))
		&& result instanceof Promise) {
		result.catch(ex => queue.notify({
			title: <b>Error</b>,
			body: `${textForError(ex.error)} (Error code: ${ex.error})`,
			icon: "error",
			actions: [
				{
					title: "Dismiss"
				}
			]
		}));
	}

	return result;
}

export default errorMiddleware;