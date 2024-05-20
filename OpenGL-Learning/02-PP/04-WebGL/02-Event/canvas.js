//Our Main function
function main()
{   //Get canvas
    var canvas = document.getElementById("AGS");

    if(canvas == null)
    {
        console.log("AGS: Error - Getting canvas failed \n");
        return;
    }

    console.log("AGS: Info - Getting canvas succeeded \n");

    var context = canvas.getContext("2d");

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

    //Set the text in middle of the canvas
    context.textAlign = "center";
    context.textBaseline = "middle";

    //Set font
    context.font = "48px sans-serif";

    //Set text color to white
    context.fillStyle = "lime"; // White color
    
    //Paint the test with above color
    context.fillText(text, canvas.width / 2, canvas.height / 2);

    //Register keyboard and mouse event handlers
    window.addEventListener("keydown", onKeyDown, false); 
    window.addEventListener("click", onMouseDown, false);   
}

function onKeyDown(event)
{
    alert("Key is pressed.");
}

function onMouseDown()
{
    alert("Mouse is clicked.");
}