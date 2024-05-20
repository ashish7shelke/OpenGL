//Our Main function

var canvas;
var gl = null;

var canvas_original_width = 1;
var canvas_original_height = 1;

var isRequestedAnimationFrame = 
    window.requestAnimationFrame ||
    window.webkitRequestAnimationFrame ||
    window.mozRequestAnimationFrame ||
    window.oRequestAnimationFrame ||
    window.msRequestAnimationFrame;

var cancelAnimationFrame = 
    window.cancelAnimationFrame ||
    window.webkitCancelRequestAnimationFrame ||
    window.webkitCancelAnimationFrame ||
    window.mozCancelRequestAnimationFrame ||
    window.mozCancelAnimationFrame ||
    window.oCancelRequestAnimationFrame ||
    window.oCancelAnimationFrame ||
    window.msCancelRequestAnimationFrame ||
    window.msCancelAnimationFrame;

var bFullScreen = false;

function main()
{   //Get canvas
    canvas = document.getElementById("AGS");

    if(canvas == null)
    {
        console.log("AGS: Error - Getting canvas failed \n");
        return;
    }

    console.log("AGS: Info - Getting canvas succeeded \n");

    //Set canvas width and height for future use
    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;

    //Register keyboard event handlers
    window.addEventListener("keydown", onKeyDown, false); 
    //Register mouse event handlers
    window.addEventListener("click", onMouseDown, false);
    // Resize resize handler
    window.addEventListener("resize", resize, false);

    initialize();
    resize();
    display();
}

function onKeyDown(event)
{
    switch (event.key) {
        case "F":
        case "f":
            toggleFullscreen();
            break;

        case "Q":
        case "q":
            uninitialize();
            window.close();
            break;
    }
}

function onMouseDown(event)
{
}

function toggleFullscreen() 
{
    var fullscreenElement =
        document.fullscreenElement || document.webkitFullscreenElement || document.mozFullScreenElement || document.msFullscreenElement || null;

    if (fullscreenElement == null) {
        if (canvas.requestFullscreen) {
            canvas.requestFullscreen();
        } else if (canvas.mozRequestFullScreen) {
            canvas.mozRequestFullScreen();
        } else if (canvas.webkitRequestFullscreen) {
            canvas.webkitRequestFullscreen();
        } else if (canvas.msRequestFullscreen) {
            canvas.msRequestFullscreen(); 
        }
        bFullScreen = true;

    } else {
        if (document.exitFullscreen) {
            document.exitFullscreen();
        } else if (document.mozCancelFullScreen) {
            document.mozCancelFullScreen();
        } else if (document.webkitExitFullscreen) {
            document.webkitExitFullscreen();
        } else if (document.msExitFullscreen) {
            document.msExitFullscreen();
        }
        bFullScreen = false;
    }
}

function initialize() {
    gl = canvas.getContext("webgl2");

    if(!gl) {
        console.log("AGS: Error gettting webGL-2 context failed.");
        return;
    }

    console.log("AGS: Info - gettting webGL-2 context succeeded.");

    //Set webgl width & heght
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    gl.clearColor(0.0, 0.0, 1.0, 1.0);
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    resize(canvas_original_width, canvas_original_height);
    display();
}

function resize(width, height)
{
    if(height == 0)
        height = 1;

    if(bFullScreen) {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    } else {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    gl.viewport(0, 0, width, height);
}

function display()
{
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    //Do double buffering
    requestAnimationFrame(display, canvas);    
}

function update()
{

}

function uninitialize()
{

}

