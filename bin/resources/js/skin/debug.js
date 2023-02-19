import * as dui from "DuiLib";
class TestWindow extends dui.Window {
    constructor() {
        super();
    }
    async initWindow() {
        this.lb = this.findControl("lb");
        this.lb.setText("aaaabbbcccccfffff");
    }
}
let testWindow = new TestWindow();
testWindow.create("layout_editor.xml", "welcome", 0);
testWindow.showWindow();
testWindow.centerWindow();
