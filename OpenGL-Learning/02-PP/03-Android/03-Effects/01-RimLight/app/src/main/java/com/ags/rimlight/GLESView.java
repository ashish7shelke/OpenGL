package com.ags.rimlight;

import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

//Event related packages
import android.view.MotionEvent;
import com.ags.rimlight.VertexAttributesEnum;
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
import android.opengl.GLES32;
import android.opengl.Matrix;
import android.opengl.GLUtils;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector = null;

    private int shaderProgramObject = 0;
    private int modelMatrixUniform = 0;
    private int viewMatrixUniform = 0;
    private int projectionMatrixUniform = 0;

    private int textureSamplerUniform = 0;
    
    private int[] vao_pyramid = new int[1];
    private int[] vbo_position_pyramid = new int[1];
    private int[] vbo_texture_pyramid = new int[1];
    private int[] vbo_normal_pyramid = new int[1];
    private int[] texture_stone = new int[1];

    private int[] vao_cube = new int[1];
    private int[] vbo_position_cube = new int[1];
    private int[] vbo_texture_cube = new int[1];
    private int[] texture_kundali = new int[1];

    //Rim Light realted variables
    private int rimColorUniform = 0;
    private int rimPowerUniform = 0;
    private int rimLightPositionUniform = 0;
    private int rimColorcounter = 0;
    private float[] rimLightPosition = new float[4];
    private float[] rimColor = new float[4];
    private float[] rimPower = new float[1];

    private float pAngle;
    private float cAngle;

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
        rimColorcounter++;
        if(rimColorcounter > 4)
            rimColorcounter = 0;
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
        rimPower[0] = rimPower[0] - 0.1f;
        if(rimPower[0] < 0.0f)
            rimPower[0] = 1.0f;
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
            + "in vec4 aPosition;\n"
            + "in vec3 aNormal;\n"
            + "in vec2 aTexCoord;\n"
            + "uniform mat4 uModelMatrix;\n"
            + "uniform mat4 uViewMatrix;\n"
            + "uniform mat4 uProjectionMatrix;\n"
            + "uniform vec4 uRimLightPosition;\n"
            + "out vec3 oTransformedNormals;\n"
            + "out vec3 oLightDirection;\n"
            + "out vec3 oViewerVector;\n"
            + "out vec2 oTexCoord;"
            + "void main(void)\n"
            + "{\n"
            + "vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition; \n"
            + "oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal; \n"
            + "oViewerVector = -eyeCoordinates.xyz;\n"
            + "oLightDirection = vec3(uRimLightPosition - eyeCoordinates); \n"
            + "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n"
            + "oTexCoord = aTexCoord \n;"
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
            + "in vec3 oTransformedNormals;\n"
            + "in vec3 oLightDirection;\n"
            + "in vec3 oViewerVector;\n"
            + "in vec2 oTexCoord;"
            + "uniform sampler2D uTextureSampler;"
            + "uniform vec4 uRimLightPosition;\n"
            + "uniform vec3 uRimColor;\n"
            + "uniform float uRimPower;\n"
            + "out vec4 FragColor;\n"
            + "vec3 calculateRim(vec3 N, vec3 V)\n"
            + "{\n"
            + "float f = 1.0f - max(dot(N, V), 0.0f);\n"
            + "f = smoothstep(0.0f, 1.0f, f); \n"
            + "f = pow(f, uRimPower); \n"
            + "return vec3(f * uRimColor); \n"
            + "}\n"
            + "void main(void)\n"
            + "{\n"
            + "vec3 phongADSLight; \n"
            + "vec3 normalizedTransformedNormals = normalize(oTransformedNormals); \n"
            + "vec3 normalizedViewerVector = normalize(oViewerVector); \n"
            + "phongADSLight = calculateRim(normalizedTransformedNormals, normalizedViewerVector);"
            + "FragColor = vec4(phongADSLight, 1.0f);//texture(uTextureSampler, oTexCoord); \n"
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
        modelMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uModelMatrix");
        viewMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uViewMatrix");
        projectionMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uProjectionMatrix");

        rimLightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uRimLightPosition"); 
        rimColorUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uRimColor");
        rimPowerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uRimPower");

        textureSamplerUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uTextureSampler");
    
        final float[] pyramid_position = new float[]{
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f
        };

        final float[] pyramid_texcoord = new float[]{
            // front
            0.5f, 1.0f, // front-top
            0.0f, 0.0f, // front-left
            1.0f, 0.0f, // front-right

            // right
            0.5f, 1.0f, // right-top
            1.0f, 0.0f, // right-left
            0.0f, 0.0f, // right-right

            // back
            0.5f, 1.0f, // back-top
            0.0f, 0.0f, // back-left
            1.0f, 0.0f, // back-right

            // left
            0.5f, 1.0f, // left-top
            1.0f, 0.0f, // left-left
            0.0f, 0.0f, // left-right
        };
        
        final float[] pyramid_normal = new float[]{
            // front
            0.000000f, 0.447214f,  0.894427f, // front-top
            0.000000f, 0.447214f,  0.894427f, // front-left
            0.000000f, 0.447214f,  0.894427f, // front-right
                                    
            // right			    
            0.894427f, 0.447214f,  0.000000f, // right-top
            0.894427f, 0.447214f,  0.000000f, // right-left
            0.894427f, 0.447214f,  0.000000f, // right-right
    
            // back
            0.000000f, 0.447214f, -0.894427f, // back-top
            0.000000f, 0.447214f, -0.894427f, // back-left
            0.000000f, 0.447214f, -0.894427f, // back-right
    
            // left
            -0.894427f, 0.447214f,  0.000000f, // left-top
            -0.894427f, 0.447214f,  0.000000f, // left-left
            -0.894427f, 0.447214f,  0.000000f, // left-right
        };

        //Vao
        GLES32.glGenVertexArrays(1, this.vao_pyramid, 0);
        GLES32.glBindVertexArray(this.vao_pyramid[0]);

        GLES32.glGenBuffers(1, this.vbo_position_pyramid, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_position_pyramid[0]);
        ByteBuffer pyramidPositionByteBuffer = ByteBuffer.allocateDirect(pyramid_position.length * 4);
        pyramidPositionByteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer pyramidPositionBuffer = pyramidPositionByteBuffer.asFloatBuffer();
        pyramidPositionBuffer.put(pyramid_position);
        pyramidPositionBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramid_position.length * 4, pyramidPositionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AGS_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AGS_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vbo_texture_pyramid, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_texture_pyramid[0]);
        ByteBuffer pyramidTextureByteBuffer = ByteBuffer.allocateDirect(pyramid_texcoord.length * 4);
        pyramidTextureByteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer pyramidTexureBuffer = pyramidTextureByteBuffer.asFloatBuffer();
        pyramidTexureBuffer.put(pyramid_texcoord);
        pyramidTexureBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramid_texcoord.length * 4, pyramidTexureBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AGS_ATTRIBUTE_TEXCORD, 2, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AGS_ATTRIBUTE_TEXCORD);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0); 
        
        GLES32.glGenBuffers(1, this.vbo_normal_pyramid, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_normal_pyramid[0]);
        ByteBuffer pyramidNormalByteBuffer = ByteBuffer.allocateDirect(pyramid_normal.length * 4);
        pyramidNormalByteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer pyramidNormalBuffer = pyramidNormalByteBuffer.asFloatBuffer();
        pyramidNormalBuffer.put(pyramid_normal);
        pyramidNormalBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramid_normal.length * 4, pyramidNormalBuffer, GLES32.GL_STATIC_DRAW);
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

        texture_stone[0] = this.loadGLTexture(R.raw.stone);  
        GLES32.glEnable(GLES32.GL_TEXTURE_2D);
    
        texture_kundali[0] = this.loadGLTexture(R.raw.kundali);  
        GLES32.glEnable(GLES32.GL_TEXTURE_2D);

        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        rimPower[0] = 1.0f;
        rimLightPosition[0] = 0.0f;
        rimLightPosition[1] = 0.0f;
        rimLightPosition[2] = -100.0f;
        rimLightPosition[3] = 1.0f;

        Matrix.setIdentityM(this.perspectiveProjectionMatrix, 0);
     }

     private void printGLInfo(GL10 gl)
     {
         System.out.println("AGS: OpenGLES Renderer : " + gl.glGetString(GL10.GL_RENDERER));
         System.out.println("AGS: OpenGLES - Version : " + gl.glGetString(GL10.GL_VERSION));
         System.out.println("AGS: OpenGLES - Shading Language Version : " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
     }

     private int loadGLTexture(int resourceId)
     {
         int[] texture = new int[1];
 
         BitmapFactory.Options options = new BitmapFactory.Options();
         options.inScaled = false;
 
         Bitmap bitmap = BitmapFactory.decodeResource(this.context.getResources(), resourceId, options);
 
         GLES32.glGenTextures(1, texture, 0);
         GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
         GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
         GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER,GLES32.GL_LINEAR);
         GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER,GLES32.GL_LINEAR_MIPMAP_LINEAR);
 
         GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
 
         GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
 
         GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);
 
         return texture[0];
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
        float[] modelMatrix = new float[16];
        float[] ViewMatrix = new float[16];
        float[] translationMatrix = new float[16];
        float[] rotationMatrix = new float[16];

        // Traingle Transformation
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(ViewMatrix, 0);
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);

        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -6.0f);
        Matrix.rotateM(rotationMatrix, 0, pAngle, 0.0f, 1.0f, 0.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, rotationMatrix, 0);

        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, ViewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix,0);

        if(rimColorcounter == 0)
        {
            rimColor[0] = 1.0f;
            rimColor[1] = 1.0f;
            rimColor[2] = 1.0f;
            rimColor[3] = 1.0f;
        }
        else if(rimColorcounter == 1)
        {
            rimColor[0] = 1.0f;
            rimColor[1] = 0.0f;
            rimColor[2] = 0.0f;
            rimColor[3] = 1.0f;
        }
        else if(rimColorcounter == 2)
        {
            rimColor[0] = 0.0f;
            rimColor[1] = 1.0f;
            rimColor[2] = 0.0f;
            rimColor[3] = 1.0f;
        }
        else if(rimColorcounter == 3)
        {
            rimColor[0] = 0.0f;
            rimColor[1] = 0.0f;
            rimColor[2] = 1.0f;
            rimColor[3] = 1.0f;
        }                

        GLES32.glUniform4fv(rimLightPositionUniform, 1, rimLightPosition, 0);
        GLES32.glUniform3fv(rimColorUniform, 1, rimColor, 0); 
		GLES32.glUniform1f(rimPowerUniform, rimPower[0]);

        // Now bind the VAO to which we want to use
        GLES32.glBindVertexArray(vao_pyramid[0]);
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, this.texture_stone[0]);
        GLES32.glUniform1i(this.textureSamplerUniform, 0);

        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);

        this.requestRender();
    }

    private void update() {
        pAngle = pAngle + 1.0f;
        if (pAngle >= 360.0f)
        {
            pAngle = -360.0f;
        }

        cAngle = cAngle + 1.0f;
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

        if(this.vbo_normal_pyramid[0] >= 0)
        {
            GLES32.glDeleteBuffers(1, this.vbo_normal_pyramid, 0);
            this.vbo_normal_pyramid[0] = 0;
        }

        if(this.vbo_position_pyramid[0] >= 0)
        {
            GLES32.glDeleteBuffers(1, this.vbo_position_pyramid, 0);
            this.vbo_position_pyramid[0] = 0;
        }

        if(this.vbo_texture_pyramid[0] >= 0)
        {
            GLES32.glDeleteBuffers(1, this.vbo_texture_pyramid, 0);
            this.vbo_texture_pyramid[0] = 0;
        }

        if(this.vao_pyramid[0] >= 0) {
            GLES32.glDeleteVertexArrays(1, this.vao_pyramid, 0);
            this.vao_pyramid[0] = 0;
        }

        if(this.vbo_texture_cube[0] >= 0)
        {
            GLES32.glDeleteBuffers(1, this.vbo_texture_cube, 0);
            this.vbo_texture_cube[0] = 0;
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
