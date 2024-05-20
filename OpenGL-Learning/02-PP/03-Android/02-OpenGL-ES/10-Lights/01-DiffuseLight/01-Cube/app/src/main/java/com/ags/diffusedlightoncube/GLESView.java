package com.ags.diffusedlightoncube;

import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

//Event related packages
import android.view.MotionEvent;
import com.ags.diffusedlightoncube.VertexAttributesEnum;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

//IO related packages
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

//OpenGL packages
import android.opengl.GLSurfaceView;
import android.opengl.GLES32;
import android.opengl.Matrix;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector = null;

    private int shaderProgramObject = 0;
    private int mvpUniform = 0;

    private int[] vao_cube = new int[1];
    private int[] vbo_position_cube = new int[1];
    private int[] vbo_normal_cube = new int[1];
    private float cAngle;

    private float[] perspectiveProjectionMatrix = new float[16];

    private boolean bLightingEnabled = false;
    private int singleTapCounter = 1;
    private int modelViewMatrixUniform = 0;
    private int projectionMatrixUniform = 0;
    private int ldUniform = 0;
    private int kdUniform = 0;
    private int lightPositionUniform = 0;
    private int isLightingEnabledUniform = 0;

	public GLESView(Context context)
	{
		super(context);

		this.context = context; 
		
		setEGLContextClientVersion(3);
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

		//Create and set Gesture Detector object
		this.gestureDetector = new GestureDetector(context, this, null, false);
		this.gestureDetector.setOnDoubleTapListener(this);
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		initialize(gl);
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		resize(width, height);
	}

	@Override
	public void onDrawFrame(GL10 gl)
	{
		display();
        update();
	}

	//Implementation of onTouvh Event of view class
	@Override
	public boolean onTouchEvent(MotionEvent e)
	{
		if(!gestureDetector.onTouchEvent(e))
		{
			super.onTouchEvent(e);
		}
		return true;
	}

	//Implementation of 3 methods onDoubleTapListener interface
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		return true;
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		//Do nothing, as we have onDoubleTap method
		return true;
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		return true;
	}

	//Impleemntation of 6 methods onGestureListener interface	
	@Override
	public boolean onDown(MotionEvent e)
	{
		//Do nothing, as we have onSingleTapConfirmed method
		return true;	
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return true;
	}

	@Override
	public void onLongPress(MotionEvent motionEvent)
	{
        if(this.bLightingEnabled == true)
            this.bLightingEnabled = false;
        else
            this.bLightingEnabled = true;
	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		uninitialize();
        System.exit(0);
		return true;
	}

	@Override
	public void onShowPress(MotionEvent e)
	{

	}
	
	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return true;
	}

    private void initialize(GL10 gl) {

		//PrintGL openGl ES Info
		printGLInfo(gl);

        //Vertex Shader
        final String vertexShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "in vec4 aPosition;\n"
            + "in vec3 aNormal;\n"
            + "uniform mat4 uModelViewMatrix;\n"
            + "uniform mat4 uProjectionMatrix;\n"
            + "uniform vec3 uLd;\n"
            + "uniform vec3 uKd;\n"
            + "uniform vec4 uLightPosition;\n"
            + "uniform mediump int uKeyPressed;\n"
            + "out vec3 oDiffusedLight;\n"
            + "void main(void)\n"
            + "{\n"
            + "if(uKeyPressed == 1) \n"
            + "{\n"
            + "vec4 eyePosition = uModelViewMatrix * aPosition; \n"
            + "mat3 normalMatrix = mat3(transpose(inverse(uModelViewMatrix))); \n"
            + "vec3 n = normalize(normalMatrix * aNormal); \n"
            + "vec3 s = normalize(vec3(uLightPosition - eyePosition)); \n"
            + "oDiffusedLight = uLd * uKd * max(dot(s,n), 0.0f); \n"
            + "}\n"
            + "else \n"
            + "{\n"
            + "oDiffusedLight = vec3(0.0f, 0.0f, 0.0f); \n"
            + "}\n"
            + "gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;\n"
            + "}\n"
        );

        int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        GLES32.glShaderSource(vertexShaderObject, vertexShaderCode);
        GLES32.glCompileShader(vertexShaderObject);

        int[] infoLogLength = new int[1];
        int[] status = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, status, 0);

        if(status[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("AGS: Vertex shader compilation log: " + infoLog);
                uninitialize();
                System.exit(1);
            }
        }

        //Fragment Shader
        final String fragmentShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "uniform int uKeyPressed;\n"
            + "precision highp float;"
            + "in vec3 oDiffusedLight;\n"
            + "out vec4 FragColor;\n"
            + "void main(void)\n"
            + "{\n"
            + "if(uKeyPressed == 1) \n"
            + "{\n"
            + "FragColor = vec4(oDiffusedLight, 1.0f);\n"
            + "}\n"
            + "else \n"
            + "{\n"
            + "FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
            + "}\n"
            + "}\n"
        );

        int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderCode);
        GLES32.glCompileShader(fragmentShaderObject);

        infoLogLength = new int[1];
        status = new int[1];
        infoLog = null;

        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, status, 0);

        if(status[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("AGS:  Fragment shader compilation log: " + infoLog);
                uninitialize();
                System.exit(1);
            }
        }
 
        //Shader Program Object
        this.shaderProgramObject = GLES32.glCreateProgram();
        GLES32.glAttachShader(this.shaderProgramObject, vertexShaderObject);
        GLES32.glAttachShader(this.shaderProgramObject, fragmentShaderObject);

        // Bind the position attribute location before linking.
        GLES32.glBindAttribLocation(this.shaderProgramObject, VertexAttributesEnum.AGS_ATTRIBUTE_POSITION, "aPosition");
        GLES32.glBindAttribLocation(this.shaderProgramObject, VertexAttributesEnum.AGS_ATTRIBUTE_NORMAL, "aNormal");

        // Now link and check for error.
        GLES32.glLinkProgram(this.shaderProgramObject);

        infoLogLength = new int[1];
        status = new int[1];
        infoLog = null;

        GLES32.glGetProgramiv(this.shaderProgramObject, GLES32.GL_LINK_STATUS, status, 0);

        if(status[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(this.shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetProgramInfoLog(this.shaderProgramObject);
                System.out.println("AGS:  Shader program link log: " + infoLog);
                uninitialize();
                System.exit(1);
            }
        }

        // After linking get the value of MVP uniform location from the shader program.
        this.modelViewMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uModelViewMatrix");
        this.projectionMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uProjectionMatrix");

        this.isLightingEnabledUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uKeyPressed");
        this.ldUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uLd");
        this.kdUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uKd");
        this.lightPositionUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uLightPosition");

        final float[] cube_position = new float[]{
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

        final float[] cube_normal = new float[]{
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
        //Vao
        GLES32.glGenVertexArrays(1, this.vao_cube, 0);
        GLES32.glBindVertexArray(this.vao_cube[0]);

        GLES32.glGenBuffers(1, this.vbo_position_cube, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_position_cube[0]);
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(cube_position.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
        positionBuffer.put(cube_position);
        positionBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cube_position.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AGS_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AGS_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vbo_normal_cube, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_normal_cube[0]);
        ByteBuffer byteNormalBuffer = ByteBuffer.allocateDirect(cube_normal.length * 4);
        byteNormalBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer normalBuffer = byteNormalBuffer.asFloatBuffer();
        normalBuffer.put(cube_normal);
        normalBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cube_normal.length * 4, normalBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AGS_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AGS_ATTRIBUTE_NORMAL);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);

        //Depth setting
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		//Enable culling
		//GLES32.glEnable(GLES32.GL_CULL_FACE);
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        Matrix.setIdentityM(this.perspectiveProjectionMatrix, 0);
     }

     private void printGLInfo(GL10 gl)
     {
         System.out.println("AGS: OpenGLES Renderer : " + gl.glGetString(GL10.GL_RENDERER));
         System.out.println("AGS: OpenGLES - Version : " + gl.glGetString(GL10.GL_VERSION));
         System.out.println("AGS: OpenGLES - Shading Language Version : " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
     }

    private void resize(int width, int height) {
        if(height == 0) {
            height = 1;
        }

        if(width == 0) {
            width = 1;
        }

        GLES32.glViewport(0, 0, width, height);
        Matrix.perspectiveM(this.perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 1.0f, 100.0f);
    }

    private void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT | GLES32.GL_STENCIL_BUFFER_BIT);
        GLES32.glUseProgram(shaderProgramObject);

        //Transformations
        float[] modelViewMatrix = new float[16];
        float[] rotationMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        // Traingle Transformation
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -6.0f);
        Matrix.rotateM(rotationMatrix, 0, cAngle, 1.0f, 1.0f, 1.0f);
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, this.perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
        GLES32.glUniformMatrix4fv(this.modelViewMatrixUniform, 1, false, modelViewMatrix, 0);
        GLES32.glUniformMatrix4fv(this.projectionMatrixUniform, 1, false, this.perspectiveProjectionMatrix, 0);

        if(bLightingEnabled) {
            GLES32.glUniform1i(this.isLightingEnabledUniform, 1);
            GLES32.glUniform3f(this.ldUniform, 1.0f, 1.0f, 1.0f);
            GLES32.glUniform3f(this.kdUniform, 0.5f, 0.5f, 0.5f);

            float lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };
            GLES32.glUniform4fv(this.lightPositionUniform, 1, lightPosition, 0);
        }
        else {
            GLES32.glUniform1i(this.isLightingEnabledUniform, 0);
        }
        // Now bind the VAO to which we want to use
        GLES32.glBindVertexArray(vao_cube[0]);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);
        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);

        this.requestRender();
    }

    private void update() {
        cAngle = cAngle + 0.5f;
        if (cAngle >= 360.0f)
        {
            cAngle = -360.0f;
        }
    }

    private void uninitialize() {

        if(this.shaderProgramObject >= 0)
        {   
            GLES32.glUseProgram(shaderProgramObject);
            int retVal[] = new int[1];

            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS, retVal, 0);

            if(retVal[0] > 0)
            {
                int numAttachedShaders = retVal[0];
                int shaderObjects[] = new int[numAttachedShaders];
                if(numAttachedShaders > 0)
                {
                    GLES32.glGetAttachedShaders(shaderProgramObject, numAttachedShaders, retVal, 0, shaderObjects, 0);

                    for(int i = 0; i < numAttachedShaders; i++)
                    {
                        GLES32.glDetachShader(shaderProgramObject, shaderObjects[i]);
                        GLES32.glDeleteShader(shaderObjects[i]);
                        shaderObjects[i] = 0;
                    }
                }  
            }          

            GLES32.glUseProgram(0);
            GLES32.glDeleteProgram(shaderProgramObject);
            this.shaderProgramObject = 0;
        }

        if(this.vbo_position_cube[0] >= 0)
        {
            GLES32.glDeleteBuffers(1, this.vbo_position_cube, 0);
            this.vbo_position_cube[0] = 0;
        }

        if(this.vao_cube[0] >= 0) {
            GLES32.glDeleteVertexArrays(1, this.vao_cube, 0);
            this.vao_cube[0] = 0;
        }
    }

}
