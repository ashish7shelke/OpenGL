package com.ags.diffusedlightoncube;

import android.content.Context;

//OPENGL-ES RELATED PACKAGES
import android.opengl.GLSurfaceView;
import android.opengl.GLES32;                     //OpenGL 3.2
import javax.microedition.khronos.opengles.GL10;           //GL 1.0 for Legacy API
import javax.microedition.khronos.egl.EGLConfig;  //EGL : Embedded Graphics Library


//EVENTS RELATED PACKAGES
import android.opengl.Matrix;
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;

//JAVA I/O RELATED PACKAGES
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import android.opengl.Matrix;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener, OnGestureListener
{

    //CLASS DATA MEMBER
    private GestureDetector gestureDetector;

    /* OPENGL RELATED */
    private int shaderProgramObject;
    private int[] vao_cube = new int[1];
    private int[] vbo_position_cube = new int[1];

    private int[] vbo_normal = new int[1];

    private float cAngle = 0.0f;

    private int modelViewMatrixUniform  = 0;
    private int projectionMatrixUniform = 0;
    private int ldUniform               = 0; //light defuse
    private int kdUniform               = 0;
    private int lightPositionUniform    = 0;
    private int keyPressUniform         = 0;

    private Boolean bLightEnabled   = true;
    private Boolean bAnimationEnabled   = true;

    private float[]  lightDefussed     = {1.0f,1.0f,1.0f,1.0f}; //white defussed light
    private float[] materialDeffused  = {0.5f,0.5f,0.5f,1.0f}; //gray ambient light
    private float[] lightPosition     = {0.0f,0.0f,2.0f,1.0f};

    private float[] perspectiveProjectionMatrix = new float[16];

    private int mvpMatrixUniform = 0;

    /*---- CONSTRUCTOR ----*/
    public GLESView(Context context)      // Context is immediate child of System.object class so it is inherited by this (MainActivity) so here we can use Context
    {
        super(context);

        /* OPENGL-ES RELATED */
        /* BELOW THREE LINES ARE EQUIVALENT WITH PFD, WGL, GLEW CODE OF WINDOWS */
        setEGLContextClientVersion(3);                      //set egl Config with 3 with there subVersion
        setRenderer(this);                                  //who will implement the 3 functions from GLSurfaceView.Renderer
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY); //WILL PAINT WHEN DIRTY


        //CREATE AND SET GESTURE DETECTOR OBJECT
        gestureDetector = new GestureDetector(context,this,null,false);
        gestureDetector.setOnDoubleTapListener(this);
    }

    /* IMPLEMENTATION OF THREE METHODS OF GLSurfaceView.Renderer Interface */

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) //setRenderer(this) will internally call onSurfaceCreated
    {
        //CODE
        initialize(gl);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        //CODE
        resize(width,height);
    }

    @Override
    public void onDrawFrame(GL10 gl)
    {
        //CODE
        display();

        update();
    }

    /* IMPLEMENTATION OF onTouch Event of view class                */
    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        //CODE
        if(!gestureDetector.onTouchEvent(e))
        {
            super.onTouchEvent(e);
        }
        return(true);
    }

    /* IMPLEMENTATION OF 3 METHODS OF onDoubleTabListener interface */
    @Override
    public boolean onDoubleTap(MotionEvent e)
    {
        return(true);
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent e)
    {
        return(true);
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent e)
    {
        return(true);
    }


    /* IMPLEMENTATION OF 6 METHODS OF onGestureListener interface   */
    @Override
    public boolean onDown(MotionEvent e)
    {
        return(true);
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
    {
        return(true);
    }

    @Override
    public void onLongPress(MotionEvent e)
    {
    }

    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
    {
        uninitialize();

        System.exit(0);
        return(true);
    }

    @Override
    public void onShowPress(MotionEvent e)
    {

    }

    @Override
    public boolean onSingleTapUp(MotionEvent e)
    {
        return(true);
    }

    //IMPLEMENTATION OF PRIVATE METHODS
    private void initialize(GL10 gl)
    {
        //CODE

        /* PRINT OPENGL-ES INFO */
        printGLInfo(gl);

        /*---------------------------------------------------------------------------*/

        /* VERTEX SHADER */
        final String vertexShaderSourceCode = String.format(
                       "#version 320 es                         \n" +
                               "\n" +
                "in vec4 aPosition;\n" +
                "in vec3 aNormal;\n" +
                "uniform mat4 uModelViewMatrix;\n" +
                "uniform mat4 uProjectionMatrix;\n" +
                "uniform vec3 uLd;\n" +
                "uniform vec3 uKd;\n" +
                "uniform vec4 uLightPosition;\n" +
                "uniform mediump int uKeyPress;\n"+
                "out vec3 oDeffusedLight;\n"+
                "void main(void)\n" +
                "{\n" +
                "if(uKeyPress==1)\n"+
                "{\n"+
                "vec4 eyePosition=uModelViewMatrix * aPosition;\n"+
                "mat3 normalMatrix=mat3(transpose(inverse(uModelViewMatrix)));\n"+
                "vec3 n=normalize(normalMatrix*aNormal);\n"+
                "vec3 s=normalize(vec3(uLightPosition-eyePosition));\n"+
                "oDeffusedLight=uLd*uKd*max(dot(s,n),0.0f);\n"+
                "}\n"+
                "else\n"+
                "{\n"+
                "oDeffusedLight=vec3(0.0f,0.0f,0.0f);\n"+
                "}\n"+
                "gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;\n"+
                "}\n");

        //CREATE EMPTY SHADER OBJECT USING glCreateShader()
        int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

        //made to recieve shaders source code
        GLES32.glShaderSource(vertexShaderObject,vertexShaderSourceCode);

        int[]  shaderCompileStatus = new int[1];
        int[]  infoLogLength       = new int[1];
        String infoLog            = null;

        GLES32.glCompileShader(vertexShaderObject);

        GLES32.glGetShaderiv(vertexShaderObject,GLES32.GL_COMPILE_STATUS,shaderCompileStatus,0);

        if(shaderCompileStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObject,GLES32.GL_INFO_LOG_LENGTH,infoLogLength,0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("YTS: Vertex Shader Compilation Error Log : "+infoLog);
                uninitialize();
                System.exit(0);
            }
        }

        /* Fragment Shader */
        final String fragmentShaderSource = String.format(
                "#version 320 es                  \n"+
                "precision highp float;           \n"+
                "                                 \n"+
                "in vec3 oDeffusedLight;  \n"+
                "uniform mediump int uKeyPress;   \n"+
                "out vec4 FragColor;      \n"+
                "void main(void)          \n"+
                "{                        \n"+
                "if(uKeyPress == 1 )      \n"+
                "{\n"+
                "FragColor=vec4(oDeffusedLight,1.0f);\n"+
                "}\n"+
                "else\n"+
                "{\n"+
                "FragColor=vec4(1.0f,1.0f,1.0f,1.0f);\n"+
                "}\n"+
                "}\n"
        );

        //CREATE EMPTY SHADER OBJECT USING glCreateShader()
        int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

        //made to recieve shaders source code
        GLES32.glShaderSource(fragmentShaderObject,fragmentShaderSource);

        shaderCompileStatus = new int[1];
        infoLogLength       = new int[1];
        infoLog             = null;

        GLES32.glCompileShader(fragmentShaderObject);

        GLES32.glGetShaderiv(fragmentShaderObject,GLES32.GL_COMPILE_STATUS,shaderCompileStatus,0);

        if(shaderCompileStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObject,GLES32.GL_INFO_LOG_LENGTH,infoLogLength,0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("YTS: Fragment Shader Compilation Error Log : "+infoLog);
                uninitialize();
                System.exit(0);
            }
        }

        //SHADER PROGRAM

        shaderProgramObject = GLES32.glCreateProgram();

        GLES32.glAttachShader(shaderProgramObject,vertexShaderObject);
        GLES32.glAttachShader(shaderProgramObject,fragmentShaderObject);

        GLES32.glBindAttribLocation(shaderProgramObject,VertexAttributesEnum.YTS_ATTRIBUTE_POSITION,"aPosition");
        GLES32.glBindAttribLocation(shaderProgramObject,VertexAttributesEnum.YTS_ATTRIBUTE_NORMAL,"aNormal");

        GLES32.glLinkProgram(shaderProgramObject);

        int[] programLinkStatus = new int[1];
        infoLogLength       = new int[1];
        infoLog             = null;

        GLES32.glGetProgramiv(shaderProgramObject,GLES32.GL_LINK_STATUS,programLinkStatus,0);

        if(programLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(shaderProgramObject,GLES32.GL_INFO_LOG_LENGTH,infoLogLength,0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("Shader Program Link Error Log : "+ infoLog);
                uninitialize();
                System.exit(0);
            }
        }

        modelViewMatrixUniform  = GLES32.glGetUniformLocation(shaderProgramObject,"uModelViewMatrix");
        projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject,"uProjectionMatrix");
        ldUniform               = GLES32.glGetUniformLocation(shaderProgramObject,"uLd");
        kdUniform               = GLES32.glGetUniformLocation(shaderProgramObject,"uKd");
        lightPositionUniform    = GLES32.glGetUniformLocation(shaderProgramObject,"uLightPosition");
        keyPressUniform         = GLES32.glGetUniformLocation(shaderProgramObject,"uKeyPress");

        /* Goemetry / Shape */

        final float[] cube_position =
                {
            // front
            1.0f,  1.0f,  1.0f, // top-right of front
            -1.0f,  1.0f,  1.0f, // top-left of front
            -1.0f, -1.0f,  1.0f, // bottom-left of front
            1.0f, -1.0f,  1.0f, // bottom-right of front

            // right
            1.0f,  1.0f, -1.0f, // top-right of right
            1.0f,  1.0f,  1.0f, // top-left of right
            1.0f, -1.0f,  1.0f, // bottom-left of right
            1.0f, -1.0f, -1.0f, // bottom-right of right

            // back
            1.0f,  1.0f, -1.0f, // top-right of back
            -1.0f,  1.0f, -1.0f, // top-left of back
            -1.0f, -1.0f, -1.0f, // bottom-left of back
            1.0f, -1.0f, -1.0f, // bottom-right of back

            // left
            -1.0f,  1.0f,  1.0f, // top-right of left
            -1.0f,  1.0f, -1.0f, // top-left of left
            -1.0f, -1.0f, -1.0f, // bottom-left of left
            -1.0f, -1.0f,  1.0f, // bottom-right of left

            // top
            1.0f,  1.0f, -1.0f, // top-right of top
            -1.0f,  1.0f, -1.0f, // top-left of top
            -1.0f,  1.0f,  1.0f, // bottom-left of top
            1.0f,  1.0f,  1.0f, // bottom-right of top

            // bottom
            1.0f, -1.0f,  1.0f, // top-right of bottom
            -1.0f, -1.0f,  1.0f, // top-left of bottom
            -1.0f, -1.0f, -1.0f, // bottom-left of bottom
            1.0f, -1.0f, -1.0f, // bottom-right of bottom

                };

        final float[] cube_normals =
            {
                    // front surface
            0.0f,  0.0f,  1.0f, // top-right of front
            0.0f,  0.0f,  1.0f, // top-left of front
            0.0f,  0.0f,  1.0f, // bottom-left of front
            0.0f,  0.0f,  1.0f, // bottom-right of front

            // right surface
            1.0f,  0.0f,  0.0f, // top-right of right
            1.0f,  0.0f,  0.0f, // top-left of right
            1.0f,  0.0f,  0.0f, // bottom-left of right
            1.0f,  0.0f,  0.0f, // bottom-right of right

            // back surface
            0.0f,  0.0f, -1.0f, // top-right of back
            0.0f,  0.0f, -1.0f, // top-left of back
            0.0f,  0.0f, -1.0f, // bottom-left of back
            0.0f,  0.0f, -1.0f, // bottom-right of back

            // left surface
            -1.0f,  0.0f,  0.0f, // top-right of left
            -1.0f,  0.0f,  0.0f, // top-left of left
            -1.0f,  0.0f,  0.0f, // bottom-left of left
            -1.0f,  0.0f,  0.0f, // bottom-right of left

            // top surface
            0.0f,  1.0f,  0.0f, // top-right of top
            0.0f,  1.0f,  0.0f, // top-left of top
            0.0f,  1.0f,  0.0f, // bottom-left of top
            0.0f,  1.0f,  0.0f, // bottom-right of top

            // bottom surface
            0.0f, -1.0f,  0.0f, // top-right of bottom
            0.0f, -1.0f,  0.0f, // top-left of bottom
            0.0f, -1.0f,  0.0f, // bottom-left of bottom
            0.0f, -1.0f,  0.0f, // bottom-right of bottom

            };

        /* vao_cube */
        GLES32.glGenVertexArrays(1,vao_cube,0);

        GLES32.glBindVertexArray(vao_cube[0]);

        GLES32.glGenBuffers(1,vbo_position_cube,0);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_position_cube[0]);

        //prepare triangle array vertices for glBufferData
        //step 1 enough byte buffer allocate
        //step 2 set byte order
        //step 3 treat this buffer as float buffer
        //step 4 fill array
        //step 5 assign position to zero index

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(cube_position.length * 4); // 4 is size of float
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
        positionBuffer.put(cube_position);
        positionBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,cube_position.length * 4,positionBuffer,GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(VertexAttributesEnum.YTS_ATTRIBUTE_POSITION,3,GLES32.GL_FLOAT,false,0,0);

        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.YTS_ATTRIBUTE_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);

//        GLES32.glVertexAttrib3f(VertexAttributesEnum.YTS_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

        GLES32.glGenBuffers(1,vbo_normal,0);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_normal[0]);

        ByteBuffer nByteBuffer = ByteBuffer.allocateDirect(cube_normals.length * 4); // 4 is size of float
        nByteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer nPositionBuffer = nByteBuffer.asFloatBuffer();
        nPositionBuffer.put(cube_normals);
        nPositionBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,cube_normals.length * 4,nPositionBuffer,GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(VertexAttributesEnum.YTS_ATTRIBUTE_NORMAL,3,GLES32.GL_FLOAT,false,0,0);

        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.YTS_ATTRIBUTE_NORMAL);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);


        GLES32.glBindVertexArray(0);

        /*---------------------------------------------------------------------------*/

        /* DEPTH ENABLE SETTING */
        GLES32.glClearDepthf(1.0f);
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        /* ENABLE BACK FACE CULLING */
//        GLES32.glEnable(GLES32.GL_CULL_FACE);

        /* CLEAR COLOR */
        GLES32.glClearColor(0.0f,0.0f,1.0f,1.0f);

        Matrix.setIdentityM(perspectiveProjectionMatrix,0);
    }

    private void printGLInfo(GL10 gl)
    {
        //CODE
        System.out.println("YTS: OpenGL-ES Rendered : " + gl.glGetString(GL10.GL_RENDERER));
        System.out.println("YTS: OpenGL-ES Version  : " + gl.glGetString(GL10.GL_VERSION));
        System.out.println("YTS: OpenGL-ES Shading Language Version : " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
    }

    private void resize(int width, int height)
    {
        //CODE
        if(height <= 0 )
        {
            height = 1;
        }

        GLES32.glViewport(0,0,width,height);

        Matrix.perspectiveM(perspectiveProjectionMatrix,0,45.0f,(float) width/(float)height,0.1f,100.0f );
    }

    private void display()
    {
        //CODE
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT | GLES32.GL_STENCIL_BUFFER_BIT);

        GLES32.glUseProgram(shaderProgramObject);

        //TRANSFORMATIONS
        float[] modelViewMatrix = new float[16];
        float[] rotationMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        Matrix.setIdentityM(modelViewMatrix,0);
        Matrix.setIdentityM(rotationMatrix,0);
        Matrix.setIdentityM(modelViewProjectionMatrix,0);

        Matrix.translateM(modelViewMatrix,0,0.0f,0.0f,-6.0f);

        Matrix.rotateM(rotationMatrix,0,cAngle,1.0f,0.0f,0.0f);
        Matrix.rotateM(rotationMatrix,0,cAngle,0.0f,1.0f,0.0f);
        Matrix.rotateM(rotationMatrix,0,cAngle,0.0f,0.0f,1.0f);
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);   

        Matrix.multiplyMM(modelViewProjectionMatrix,0,perspectiveProjectionMatrix,0,modelViewMatrix,0);

        GLES32.glUniformMatrix4fv(mvpMatrixUniform,1,false,modelViewProjectionMatrix,0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform,1,false, perspectiveProjectionMatrix,0);

        if(bLightEnabled == true)
        {
            GLES32.glUniform1i(keyPressUniform,1);
            GLES32.glUniform3f(ldUniform,1.0f, 1.0f, 1.0f);//array is 0f 4 element , but we use 3fv so it sends 3 elements
            GLES32.glUniform3f(kdUniform,0.5f, 0.5f, 0.5f);
            float lightPosition1[] = { 0.0f, 0.0f, 2.0f, 1.0f };
            GLES32.glUniform4fv(lightPositionUniform,1,lightPosition1,0);
        }
        else
        {
            GLES32.glUniform1i(keyPressUniform,0);
        }

        GLES32.glBindVertexArray(vao_cube[0]);

        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,0,4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);
        /* RENDER */
        requestRender(); //similar to swapBuffer
    }

    private void update()
    {
        cAngle = cAngle + 1.0f;
        if(cAngle >= 360.0f)
        {
            cAngle = cAngle - 360.0f;
        }
    }

    private void uninitialize()
    {
        //CODE
        if(shaderProgramObject > 0 )
        {
            GLES32.glUseProgram(shaderProgramObject);
            int[] retVal = new int[1];
            GLES32.glGetProgramiv(shaderProgramObject,GLES32.GL_ATTACHED_SHADERS,retVal,0);
            if(retVal[0] > 0 )
            {
                int numAttachedShaders = retVal[0];

                int[] shaderObjects = new int[numAttachedShaders];

                GLES32.glGetAttachedShaders(shaderProgramObject, numAttachedShaders, retVal, 0, shaderObjects, 0);

                for (int i = 0; i < numAttachedShaders; i++)
                {
                    GLES32.glDetachShader(shaderProgramObject, shaderObjects[i]);
                    GLES32.glDeleteShader(shaderObjects[i]);

                    shaderObjects[i] = 0;
                }
            }
            GLES32.glUseProgram(0);
            GLES32.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }

        if(vbo_position_cube[0]>0)
        {
            GLES32.glDeleteBuffers(1,vbo_position_cube,0);
            vbo_position_cube[0] = 0;
        }

        if(vao_cube[0] > 0 )
        {
            GLES32.glDeleteVertexArrays(1,vao_cube,0);
            vao_cube[0] = 0;
        }
    }
}

