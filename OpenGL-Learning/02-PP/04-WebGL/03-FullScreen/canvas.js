//Our Main function

var canvas;
var context;

function main()
{   //Get canvas
    canvas = document.getElementById("AGS");

    if(canvas == null)
    {
        console.log("AGS: Error - Getting canvas failed \n");
        return;
    }

    console.log("AGS: Info - Getting canvas succeeded \n");

    context = canvas.getContext("2d");

    if(context == null)
    {
        console.log("AGS: Error - Getting 2d context failed");
        return;
    }

    console.log("AGS: Info - Getting 2d context succeeded");    

    //Set canvas  background color to black
    context.fillStyle = "black";
    //Paint the canvas rectangle with above color
    context.fillRect(0, 0, canvas.width, canvas.height);

    //Declare the text
    var text = "Hello World !!!";
    drawText(text); 

    //Register keyboard event handlers
    window.addEventListener("keydown", onKeyDown, false); 
    //Register mouse event handlers
    window.addEventListener("click", onMouseDown, false);   
}

function onKeyDown(event)
{
    switch (event.key) {
        case "F":
        case "f":
            toggleFullscreen();
            drawText("Hello World !!!");
            break;
    }
}

function onMouseDown(event)
{
}

function drawText(text)
{
        //drawText(text);
        context.textAlign = "center";
        context.textBaseline = "middle";
    
        //Set font
        context.font = "48px sans-serif";
    
        //Set text color to white
        context.fillStyle = "lime"; // White color
        
        //Paint the test with above color
        context.fillText(text, canvas.width / 2, canvas.height / 2);  
}

function toggleFullscreen() {
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
    }
}