package com.ags.checkerboard;

import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

//Event related packages
import android.view.MotionEvent;
import com.ags.checkerboard.VertexAttributesEnum;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

//IO related packages
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;

//OpenGL packages
import android.opengl.GLSurfaceView;
import android.opengl.GLES10;
import android.opengl.GLES32;
import android.opengl.Matrix;
import android.opengl.GLUtils;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector = null;

    private final int checkImageWidth = 64;
    private final int checkImageHeight = 64;

    private int shaderProgramObject = 0;
    private int mvpUniform = 0;
    private int textureSamplerUniform = 0;

    private int[] vao = new int[1];
    private int[] vbo_position = new int[1];
    private int[] vbo_texture = new int[1];
    private int[] texture_checkerboard = new int[1];
    private int[] checkImage = new int[checkImageHeight * checkImageWidth];

    private float[] perspectiveProjectionMatrix = new float[16];

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
            + "in vec4 aPosition;"
            + "in vec2 aTexCoord;"
            + "out vec2 oTexCoord;"
            + "uniform mat4 umvpMatrix;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   gl_Position = umvpMatrix * aPosition;"
            + "   oTexCoord= aTexCoord;"
            + "}"
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
            + "precision highp float;"
            + "in vec2 oTexCoord;"
            + "uniform sampler2D uTextureSampler;"
            + "out vec4 fragColor;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   fragColor = texture(uTextureSampler, oTexCoord);"
            + "}"
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
        GLES32.glBindAttribLocation(this.shaderProgramObject, VertexAttributesEnum.AGS_ATTRIBUTE_TEXCORD, "aTexCoord");

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
        mvpUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "umvpMatrix");
        textureSamplerUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uTextureSampler");

        final float[] rectangle_position = new float[]{
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
        };

        final float[] rectangle_texcoord = new float[]{
            0.0f,0.0f,
            1.0f,0.0f,
            1.0f,1.0f,
            0.0f,1.0f
        };        

        //Vao
        GLES32.glGenVertexArrays(1, this.vao, 0);
        GLES32.glBindVertexArray(this.vao[0]);

        GLES32.glGenBuffers(1, this.vbo_position, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_position[0]);
        ByteBuffer positionByteBuffer = ByteBuffer.allocateDirect(rectangle_position.length * 4);
        positionByteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer positionBuffer = positionByteBuffer.asFloatBuffer();
        positionBuffer.put(rectangle_position);
        positionBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, rectangle_position.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AGS_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AGS_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vbo_texture, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_texture[0]);
        ByteBuffer textureByteBuffer = ByteBuffer.allocateDirect(rectangle_texcoord.length * 4);
        textureByteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer texureBuffer = textureByteBuffer.asFloatBuffer();
        texureBuffer.put(rectangle_texcoord);
        texureBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, rectangle_texcoord.length * 4, texureBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AGS_ATTRIBUTE_TEXCORD, 2, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AGS_ATTRIBUTE_TEXCORD);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);        

        GLES32.glBindVertexArray(0);

        //Depth setting
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		//Enable culling
		GLES32.glEnable(GLES32.GL_CULL_FACE);

        loadGLTexture();  
        GLES32.glEnable(GLES32.GL_TEXTURE_2D);
        
        GLES32.glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        Matrix.setIdentityM(this.perspectiveProjectionMatrix, 0);
     }

     private void printGLInfo(GL10 gl)
     {
         System.out.println("AGS: OpenGLES Renderer : " + gl.glGetString(GL10.GL_RENDERER));
         System.out.println("AGS: OpenGLES - Version : " + gl.glGetString(GL10.GL_VERSION));
         System.out.println("AGS: OpenGLES - Shading Language Version : " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
     }

     private void loadGLTexture()
     {
        this.makeCheckImage();

        /*ByteBuffer textureByteBuffer = ByteBuffer.allocateDirect(checkImageWidth * checkImageHeight * 4);
        textureByteBuffer.order(ByteOrder.nativeOrder());
        textureByteBuffer.put(checkImage);
        textureByteBuffer.position(0);*/

        //Copy pixels from buffer
        Bitmap bitmap = Bitmap.createBitmap(this.checkImage, this.checkImageWidth, this.checkImageHeight, Bitmap.Config.ARGB_8888);

        GLES32.glGenTextures(1, this.texture_checkerboard, 0);
        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, this.texture_checkerboard[0]);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_REPEAT);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_REPEAT);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);

        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);

        GLES10.glTexEnvf(GLES10.GL_TEXTURE_ENV, GLES10.GL_TEXTURE_ENV_MODE, GLES10.GL_REPLACE);

        // Unbind the texture else the last loaded texture will be shown in display
        // if we forgot to specify to which texture to bind in display.
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);
     }

     void makeCheckImage() {
        int i = 0;
        int j = 0;

        for (i = 0; i < this.checkImageHeight; ++i)
        {
            for (j = 0; j < this.checkImageWidth; ++j)
            {
                if(((i & 0x8) ^ (j & 0x8)) != 0) {
                    this.checkImage[i * this.checkImageHeight + j] = android.graphics.Color.WHITE;
                }
                else {
                    this.checkImage[i * this.checkImageHeight + j] = android.graphics.Color.BLACK;
                }
            }
        }

        /*
         * c = ((i & 0x8) ^ (j & 0x8)) * 255;
         *  checkImage[(i * 64 + j) * 4 + 0] = (byte)c;
         *  checkImage[(i * 64 + j) * 4 + 1] = (byte)c;
         *  checkImage[(i * 64 + j) * 4 + 2] = (byte)c;
         *  checkImage[(i * 64 + j) * 4 + 3] = (byte)0xFF;
         */
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

        float[] square1PosCoord = new float[12];
        float[] square2PosCoord = new float[12];

        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT | GLES32.GL_STENCIL_BUFFER_BIT);
        GLES32.glUseProgram(shaderProgramObject);

        //Transformations
        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // Translate the modal view matrix.
        Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -6.0f);

        Matrix.multiplyMM(modelViewProjectionMatrix, 0, this.perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, this.texture_checkerboard[0]);
        GLES32.glUniform1i(this.textureSamplerUniform, 0);

        // Position co-ordinates
        square1PosCoord[0] = 0.0f; 
        square1PosCoord[1] = 1.0f;
        square1PosCoord[2] = 0.0f;
        square1PosCoord[3] = -2.0f;
        square1PosCoord[4] = 1.0f;
        square1PosCoord[5] = 0.0f;
        square1PosCoord[6] = -2.0f;
        square1PosCoord[7] = -1.0f;
        square1PosCoord[8] = 0.0f;
        square1PosCoord[9] = 0.0f;
        square1PosCoord[10] = -1.0f;
        square1PosCoord[11] = 0.0f;

        GLES32.glBindVertexArray(vao[0]);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_position[0]);
        ByteBuffer positionByteBuffer = ByteBuffer.allocateDirect(square1PosCoord.length * 4);
        positionByteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer positionBuffer = positionByteBuffer.asFloatBuffer();
        positionBuffer.put(square1PosCoord);
        positionBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, square1PosCoord.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

        // Position co-ordinates
        square2PosCoord[0] = 2.4142f; 
        square2PosCoord[1] = 1.0f;
        square2PosCoord[2] = -1.4142f;
        square2PosCoord[3] = 1.0f;
        square2PosCoord[4] = 1.0f;
        square2PosCoord[5] = 0.0f;
        square2PosCoord[6] = 1.0f;
        square2PosCoord[7] = -1.0f;
        square2PosCoord[8] = 0.0f;
        square2PosCoord[9] = 2.4142f;
        square2PosCoord[10] = -1.0f;
        square2PosCoord[11] = -1.4142f;


        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_position[0]);
        positionByteBuffer = ByteBuffer.allocateDirect(square2PosCoord.length * 4);
        positionByteBuffer.order(ByteOrder.nativeOrder());
        positionBuffer = positionByteBuffer.asFloatBuffer();
        positionBuffer.put(square2PosCoord);
        positionBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, square2PosCoord.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);        

        GLES32.glBindVertexArray(0);

        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

        GLES32.glUseProgram(0);

        this.requestRender();
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

        if(this.vbo_texture[0] >= 0)
        {
            GLES32.glDeleteBuffers(1, this.vbo_texture, 0);
            this.vbo_texture[0] = 0;
        }

        if(this.vbo_position[0] >= 0)
        {
            GLES32.glDeleteBuffers(1, this.vbo_position, 0);
            this.vbo_position[0] = 0;
        }

        if(this.vao[0] >= 0) {
            GLES32.glDeleteVertexArrays(1, this.vao, 0);
            this.vao[0] = 0;
        }
    }

}
