const normalText = "font-weight: normal;";
const boldText = "font-weight: bold;";

const color = color => `color: ${color};`;

class Logger {
	moduleName = "";
	moduleColor = "black";

	constructor(moduleName, moduleColor) {
		this.moduleName = moduleName;
		this.moduleColor = moduleColor;

        this.info("Logger initialized!");
	}

	info = (...string) => {
		console.log(
			`%c[%c${this.moduleName}%c] %cInfo: %c${string}`,
            color("black") + boldText,
            color(this.moduleColor) + boldText,
            color("black") + boldText,
            color("green") + boldText,
            color("black") + normalText
		);
	};

	warning = (...string) => {
		console.log(
			`%c[%c${this.moduleName}%c] %cWarning: %c${string}`,
            color("black") + boldText,
            color(this.moduleColor) + boldText,
            color("black") + boldText,
            color("orange") + boldText,
            color("black") + normalText
		);
	};

	error = (...string) => {
		console.log(
			`%c[%c${this.moduleName}%c] %cError: %c${string}`,
            color("black") + boldText,
            color(this.moduleColor) + boldText,
            color("black") + boldText,
            color("red") + boldText,
            color("black") + normalText
		);
	};
}

export default Logger;
