import * as dui from "DuiLib"

class TestWindow extends dui.Window {
	lb: dui.Label;

	constructor(){
		super();
	}

	async initWindow() {
		this.lb = <dui.Label>this.findControl("lb");
		this.lb.setText("aaaabbbcccccfffff");
	}
}

let testWindow = new TestWindow();
testWindow.create("layout_editor.xml", "welcome", 0);
testWindow.showWindow();
testWindow.centerWindow();