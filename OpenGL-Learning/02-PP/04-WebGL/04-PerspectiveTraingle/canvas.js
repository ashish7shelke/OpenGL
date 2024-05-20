//Our Main function

var canvas;
var gl = null;

var canvas_original_width = 1;
var canvas_original_height = 1;

//WebGl related 
var VertexAttributesEnum =
{
    AGS_ATTRIBUTE_POSITION : 0,
    AGS_ATTRIBUTE_COLOR  : 1,
    AGS_ATTRIBUTE_NORMAL : 2,
    AGS_ATTRIBUTE_TEXTURE0 : 3
};

var shaderProgramObject = null;
var vao = null;
var vbo = null;
var mvpMatrixUniform;
var perspectiveProjectionMatrix;
var displayAnimationRequestId = null;

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

    //VErtex shader source code
    var vertexShaderCode = 
        "#version 300 es"
        + "\n"
        + "in vec4 aPosition;"
        + "uniform mat4 umvpMatrix;"
        + "\n"
        + "void main(void)"
        + "{"
        + "   gl_Position = umvpMatrix * aPosition;"
        + "}"

    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderCode);
    gl.compileShader(vertexShaderObject);

    if(!gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS)) {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if(error.length > 0) {
            var log = "Vertex ShaderCompilation Error : " + error;
            alert(log);
            uninitialize();
        }

    }
    else{
        console.log("AGS: Info - Vertex Shader compilation successful");
    }        

    //Fragment shader source code
    var fragmentShaderCode =
        "#version 300 es"
        + "\n"
        + "precision highp float;"
        + "out vec4 fragColor;"
        + "\n"
        + "void main(void)"
        + "{"
        + "   fragColor = vec4(1.0, 1.0, 1.0, 1.0);"
        + "}"

    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderCode);
    gl.compileShader(fragmentShaderObject);

    if(!gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS)) {
        var error = gl.getShaderInfoLog(fragmentShaderObject);

        if(error.length > 0) {
            var log = "Fragment ShaderCompilation Error : " + error;
            alert(log);
            uninitialize();
        }       
    }
    else{
        console.log("AGS: Info - Fragment Shader compilation successful");
    }     

    // Link Program
    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);

    gl.bindAttribLocation(shaderProgramObject, VertexAttributesEnum.AGS_ATTRIBUTE_POSITION, "aPosition");
    gl.linkProgram(shaderProgramObject);

    if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(shaderProgramObject);

        if(error.length > 0) {
            var log = "Program Link Error : " + error;
            alert(log);
            uninitialize();
        }
    }
    else{
        console.log("AGS: Info - Program linking successful");
    }

    mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject, "umvpMatrix");  
    
    var traingle_position = new Float32Array([0.0, 1.0, 0.0,
                                             -1.0, -1.0, 0.0,
                                             1.0, -1.0, 0.0]);

    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
    gl.bufferData(gl.ARRAY_BUFFER, traingle_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(VertexAttributesEnum.AGS_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(VertexAttributesEnum.AGS_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);
    
    gl.clearColor(0.0, 0.0, 1.0, 1.0);
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    perspectiveProjectionMatrix = mat4.create(); 
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

    gl.viewport(0, 0, canvas.width, canvas.height);
    mat4.perspective(perspectiveProjectionMatrix, 45.0, (canvas.width)/(canvas.height), 0.1, 100.0);
}

function display()
{
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    let modelViewMatrix = mat4.create();
    let modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix, modelViewMatrix, [0, 0, -3.0]);

    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vao);

    gl.drawArrays(gl.TRIANGLES, 0, 3);

    gl.bindVertexArray(null);
    gl.useProgram(null);

    //Do double buffering
    requestAnimationFrame(display, canvas);    
}

function update()
{

}

function uninitialize()
{
    if(vao) {
        gl.deleteVertexArray(vao);
        vao = null;
    }

    if(vbo) {
        gl.deleteBuffer(vbo);
        vbo = null;
    } 
    
     if(shaderProgramObject) {
        gl.useProgram(shaderProgramObject);
        var shaderObjects = gl.getAttachedShader(shaderProgramObject);
        
        if(shaderObjects && shaderObjects.length > 0)
        {
            for(let i = 0; i < shaderObjects.length; i++)
            {
                gl.detachShader(shaderProgramObject,shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;
            }      
        }
        gl.useProgram(0); 
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }

}

