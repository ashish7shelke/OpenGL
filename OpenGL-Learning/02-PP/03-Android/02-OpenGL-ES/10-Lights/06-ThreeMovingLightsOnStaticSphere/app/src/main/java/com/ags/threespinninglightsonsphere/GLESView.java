package com.ags.threespinninglightsonsphere;

import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

//Event related packages
import android.view.MotionEvent;
import com.ags.threespinninglightsonsphere.VertexAttributesEnum;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

//IO related packages
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

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

    private int shaderPVProgramObject = 0;
    private int shaderPFProgramObject = 0;
    private int mvpUniform = 0;

    private float[] perspectiveProjectionMatrix = new float[16];

    private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
    private int[] numVertices = new int[1];
    private int[] numElements = new int[1];

    private char choosenShader = 'V';
    private boolean bLightingEnabled = false;
    private int singleTapCounter = 1;
    private int modelMatrixUniform = 0;
    private int viewMatrixUniform = 0;
    private int projectionMatrixUniform = 0;
    
    private int[] lightAmbientUniform = new int[3];
    private int[] lightDiffusedUniform = new int[3];
    private int[] lightSpecularUniform = new int[3];
    private int[] lightPositionUniform = new int[3];

    private float[] lightAmbientZero = {0.0f, 0.0f, 0.0f, 0.0f};
    private float[] lightDiffusedZero = {1.0f, 0.0f, 0.0f, 0.0f};
    private float[] lightSpecularZero = {1.0f, 0.0f, 0.0f, 0.0f};
    private float[] lightPostionZero = {0.0f, 0.0f, 0.0f, 0.0f};

    private float[] lightAmbientOne = {0.0f, 0.0f, 0.0f, 0.0f};
    private float[] lightDiffusedOne = {0.0f, 1.0f, 0.0f, 0.0f};
    private float[] lightSpecularOne = {0.0f, 1.0f, 0.0f, 0.0f};
    private float[] lightPostionOne = {0.0f, 0.0f, 0.0f, 0.0f};

    private float[] lightAmbientTwo = {0.0f, 0.0f, 0.0f, 0.0f};
    private float[] lightDiffusedTwo = {0.0f, 0.0f, 1.0f, 0.0f};
    private float[] lightSpecularTwo = {0.0f, 0.0f, 1.0f, 0.0f};
    private float[] lightPostionTwo = {0.0f, 0.0f, 0.0f, 0.0f};

    private float[] materialAmbient = {0.0f, 0.0f, 0.0f, 0.0f};
    private float[] materialDiffused = {1.0f, 1.0f, 1.0f, 1.0f};
    private float[] materialSpecular = {1.0f, 1.0f, 1.0f, 0.0f};
    private float materialShininess = 50.0f;

    private float lightAngleZero = 0.0f;
    private float lightAngleOne = 0.0f;
    private float lightAngleTwo = 0.0f;

    private int materialAmbientUniform = 0;
    private int materialDiffusedUniform = 0;
    private int materialSpecularUniform = 0;
    private int materialShynessUniform = 0;

    private int isLightingEnabledUniform = 0;

  /*   class Light
    {
        public float[3] ambient;
        public float[3] diffused;
        public float[3] specular;
        public float[4] position;
    };
    private Light light[] = new Light[3];
 */
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
        if(choosenShader == 'V')
            choosenShader = 'F';
        else if(choosenShader == 'F')
            choosenShader = 'V';
        else
            choosenShader = 'V';
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
        final String vertexShaderCodePF = String.format(
            "#version 320 es"
            + "\n"
            + "in vec4 aPosition;\n"
            + "in vec3 aNormal;\n"
            + "uniform mat4 uModelMatrix;\n"
            + "uniform mat4 uViewMatrix;\n"
            + "uniform mat4 uProjectionMatrix;\n"
            + "uniform vec4 uLightPosition[3];\n"
            + "uniform mediump int uKeyPressed;\n"
            + "out vec3 oTransformedNormals;\n"
            + "out vec3 oLightDirection[3];\n"
            + "out vec3 oViewerVector;\n"
            + "void main(void)\n"
            + "{\n"
            + "if(uKeyPressed == 1) \n"
            + "{\n"
            + "vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition; \n"
            + "oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal; \n"
            + "for(int i = 0; i < 3; i++)\n"
            + "{\n"
            + "oLightDirection[i] = vec3(uLightPosition[i] - eyeCoordinates); \n"
            + "}\n"
            + "oViewerVector = -eyeCoordinates.xyz;\n"
            + "}\n"
            + "else \n"
            + "{\n"
            + "oTransformedNormals = vec3(0.0f, 0.0f, 0.0f); \n"
            + "oLightDirection[0] = vec3(0.0f, 0.0f, 0.0f); \n"
            + "oLightDirection[1] = vec3(0.0f, 0.0f, 0.0f); \n"
            + "oLightDirection[2] = vec3(0.0f, 0.0f, 0.0f); \n"
            + "oViewerVector = vec3(0.0f, 0.0f, 0.0f); \n"
            + "}\n"
            + "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n"
            + "}\n"
        );
        int vertexShaderObjectPF = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        GLES32.glShaderSource(vertexShaderObjectPF, vertexShaderCodePF);
        GLES32.glCompileShader(vertexShaderObjectPF);

        int[] infoLogLength = new int[1];
        int[] status = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(vertexShaderObjectPF, GLES32.GL_COMPILE_STATUS, status, 0);

        if(status[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObjectPF, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(vertexShaderObjectPF);
                System.out.println("AGS: Vertex shader compilation log: " + infoLog);
                uninitialize();
                System.exit(1);
            }
        }

        //Fragment Shader
        final String fragmentShaderCodePF = String.format(
            "#version 320 es"
            + "\n"
            + "precision highp float;"
            + "in vec3 oTransformedNormals;\n"
            + "in vec3 oLightDirection[3];\n"
            + "in vec3 oViewerVector;\n"
            + "uniform vec3 uLightAmbient[3];\n"
            + "uniform vec3 uLightDiffused[3];\n"
            + "uniform vec3 uLightSpecular[3];\n"
            + "uniform vec4 uLightPosition[3];\n"
            + "uniform vec3 uMaterialAmbient;\n"
            + "uniform vec3 uMaterialDiffused;\n"
            + "uniform vec3 uMaterialSpecular;\n"
            + "uniform float uMaterialShyniness;\n"
            + "uniform int uKeyPressed;\n"
            + "out vec4 FragColor;\n"
            + "void main(void)\n"
            + "{\n"
            + "vec3 phongADSLight = vec3(0.0f, 0.0f, 0.0f); \n"
            + "if(uKeyPressed == 1) \n"
            + "{\n"
            + "vec3 diffusedLight[3]; \n"
            + "vec3 ambientLight[3];\n"
            + "vec3 specularLight[3];\n"
            + "vec3 normalizedLightDirection[3];\n"
            + "vec3 reflectionVector[3];\n"
            + "vec3 normalizedTransformedNormals = normalize(oTransformedNormals); \n"
            + "vec3 normalViewerVector = normalize(oViewerVector);\n"
            + "for(int i = 0; i < 3; i++)\n"
            + "{\n"
            + "normalizedLightDirection[i] = normalize(oLightDirection[i]); \n"
            + "reflectionVector[i] = reflect(-normalizedLightDirection[i], normalizedTransformedNormals);\n"
            + "diffusedLight[i] = uLightDiffused[i] * uMaterialDiffused * max(dot(normalizedLightDirection[i], normalizedTransformedNormals), 0.0f); \n"
            + "ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;\n"
            + "specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i], normalViewerVector), 0.0f), uMaterialShyniness);\n"
            + "phongADSLight = phongADSLight + ambientLight[i] + diffusedLight[i] + specularLight[i]; \n"
            + "}\n"
            + "}\n"
            + "else \n"
            + "{\n"
            + "phongADSLight = vec3(1.0f, 1.0f, 1.0f); \n"
            + "}\n"
            + "FragColor = vec4(phongADSLight, 1.0f); \n"
            + "}\n"
        );
        int fragmentShaderObjectPF = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        GLES32.glShaderSource(fragmentShaderObjectPF, fragmentShaderCodePF);
        GLES32.glCompileShader(fragmentShaderObjectPF);

        infoLogLength = new int[1];
        status = new int[1];
        infoLog = null;

        GLES32.glGetShaderiv(fragmentShaderObjectPF, GLES32.GL_COMPILE_STATUS, status, 0);

        if(status[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObjectPF, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(fragmentShaderObjectPF);
                System.out.println("AGS:  Fragment shader compilation log: " + infoLog);
                uninitialize();
                System.exit(1);
            }
        }
 
        //Shader Program Object
        this.shaderPFProgramObject = GLES32.glCreateProgram();
        GLES32.glAttachShader(this.shaderPFProgramObject, vertexShaderObjectPF);
        GLES32.glAttachShader(this.shaderPFProgramObject, fragmentShaderObjectPF);

        // Bind the position attribute location before linking.
        GLES32.glBindAttribLocation(this.shaderPFProgramObject, VertexAttributesEnum.AGS_ATTRIBUTE_POSITION, "aPosition");
        GLES32.glBindAttribLocation(this.shaderPFProgramObject, VertexAttributesEnum.AGS_ATTRIBUTE_NORMAL, "aNormal");

        // Now link and check for error.
        GLES32.glLinkProgram(this.shaderPFProgramObject);

        infoLogLength = new int[1];
        status = new int[1];
        infoLog = null;

        GLES32.glGetProgramiv(this.shaderPFProgramObject, GLES32.GL_LINK_STATUS, status, 0);

        if(status[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(this.shaderPFProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetProgramInfoLog(this.shaderPFProgramObject);
                System.out.println("AGS:  Shader program link log: " + infoLog);
                uninitialize();
                System.exit(1);
            }
        }

        // After linking get the value of MVP uniform location from the shader program.
/*         this.modelMatrixUniform = GLES32.glGetUniformLocation(this.shaderPFProgramObject, "uModelMatrix");
        this.viewMatrixUniform = GLES32.glGetUniformLocation(this.shaderPFProgramObject, "uViewMatrix");
        this.projectionMatrixUniform = GLES32.glGetUniformLocation(this.shaderPFProgramObject, "uProjectionMatrix");

        this.isLightingEnabledUniform = GLES32.glGetUniformLocation(this.shaderPFProgramObject, "uKeyPressed");

        lightAmbientUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightAmbient");
        lightDiffusedUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightDiffused");
        lightSpecularUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightSpecular");
        lightPositionUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightPosition");
    
        materialAmbientUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uMaterialAmbient");
        materialDiffusedUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uMaterialDiffused");
        materialSpecularUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uMaterialSpecular");
        materialShynessUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uMaterialShyniness"); */
        ////////////

        //Vertex Shader
        final String vertexShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "in vec4 aPosition;\n"
            + "in vec3 aNormal;\n"
            + "uniform mat4 uModelMatrix;\n"
            + "uniform mat4 uViewMatrix;\n"
            + "uniform mat4 uProjectionMatrix;\n"
            + "uniform vec3 uLightAmbient[3];\n"
            + "uniform vec3 uLightDiffused[3];\n"
            + "uniform vec3 uLightSpecular[3];\n"
            + "uniform vec4 uLightPosition[3];\n"
            + "uniform vec3 uMaterialAmbient;\n"
            + "uniform vec3 uMaterialDiffused;\n"
            + "uniform vec3 uMaterialSpecular;\n"
            + "uniform float uMaterialShyniness;\n"
            + "uniform mediump int uKeyPressed;\n"
            + "out vec3 oPhongADSLight;\n"
            + "void main(void)\n"
            + "{\n"
            + "oPhongADSLight = vec3(0.0f, 0.0f, 0.0f); \n"
            + "if(uKeyPressed == 1) \n"
            + "{\n"
            + "vec3 diffusedLight[3]; \n"
            + "vec3 ambientLight[3];\n"
            + "vec3 specularLight[3];\n"
            + "vec3 lightDirection[3];\n"
            + "vec3 reflectionVector[3];\n"
            + "vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition; \n"
            + "vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal); \n"
            + "vec3 viewerVector = normalize(-eyeCoordinates.xyz);\n"
            + "for(int i = 0; i < 3; i++)\n"
            + "{\n"
            + "lightDirection[i] = normalize(vec3(uLightPosition[i] - eyeCoordinates)); \n"
            + "reflectionVector[i] = reflect(-lightDirection[i], transformedNormals);\n"
            + "diffusedLight[i] = uLightDiffused[i] * uMaterialDiffused * max(dot(lightDirection[i],transformedNormals), 0.0f); \n"
            + "ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;\n"
            + "specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i], viewerVector), 0.0f), uMaterialShyniness);\n"
            + "oPhongADSLight =  oPhongADSLight + ambientLight[i] + diffusedLight[i] + specularLight[i]; \n"
            + "}\n"
            + "}\n"
            + "else \n"
            + "{\n"
            + "oPhongADSLight = vec3(0.0f, 0.0f, 0.0f); \n"
            + "}\n"
            + "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n"
            + "}\n"
        );
        int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        GLES32.glShaderSource(vertexShaderObject, vertexShaderCode);
        GLES32.glCompileShader(vertexShaderObject);

        infoLogLength = new int[1];
        status = new int[1];
        infoLog = null;

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
            + "in vec3 oPhongADSLight;\n"
            + "uniform int uKeyPressed;\n"
            + "out vec4 FragColor;\n"
            + "void main(void)\n"
            + "{\n"
            + "if(uKeyPressed == 1) \n"
            + "{\n"
            + "FragColor = vec4(oPhongADSLight, 1.0f);\n"
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
        this.shaderPVProgramObject = GLES32.glCreateProgram();
        GLES32.glAttachShader(this.shaderPVProgramObject, vertexShaderObject);
        GLES32.glAttachShader(this.shaderPVProgramObject, fragmentShaderObject);

        // Bind the position attribute location before linking.
        GLES32.glBindAttribLocation(this.shaderPVProgramObject, VertexAttributesEnum.AGS_ATTRIBUTE_POSITION, "aPosition");
        GLES32.glBindAttribLocation(this.shaderPVProgramObject, VertexAttributesEnum.AGS_ATTRIBUTE_NORMAL, "aNormal");

        // Now link and check for error.
        GLES32.glLinkProgram(this.shaderPVProgramObject);

        infoLogLength = new int[1];
        status = new int[1];
        infoLog = null;

        GLES32.glGetProgramiv(this.shaderPVProgramObject, GLES32.GL_LINK_STATUS, status, 0);

        if(status[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(this.shaderPVProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetProgramInfoLog(this.shaderPVProgramObject);
                System.out.println("AGS:  Shader program link log: " + infoLog);
                uninitialize();
                System.exit(1);
            }
        }

        // After linking get the value of MVP uniform location from the shader program.
        this.modelMatrixUniform = GLES32.glGetUniformLocation(this.shaderPVProgramObject, "uModelMatrix");
        this.viewMatrixUniform = GLES32.glGetUniformLocation(this.shaderPVProgramObject, "uViewMatrix");
        this.projectionMatrixUniform = GLES32.glGetUniformLocation(this.shaderPVProgramObject, "uProjectionMatrix");

        this.isLightingEnabledUniform = GLES32.glGetUniformLocation(this.shaderPVProgramObject, "uKeyPressed");

        lightAmbientUniform[0] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightAmbient[0]");
        lightDiffusedUniform[0] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightDiffused[0]");
        lightSpecularUniform[0] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightSpecular[0]");
        lightPositionUniform[0] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightPosition[0]");
    
        lightAmbientUniform[1] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightAmbient[1]");
        lightDiffusedUniform[1] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightDiffused[1]");
        lightSpecularUniform[1] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightSpecular[1]");
        lightPositionUniform[1] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightPosition[1]");
    
        lightAmbientUniform[2] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightAmbient[2]");
        lightDiffusedUniform[2] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightDiffused[2]");
        lightSpecularUniform[2] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightSpecular[2]");
        lightPositionUniform[2] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightPosition[2]");

        materialAmbientUniform = GLES32.glGetUniformLocation(shaderPVProgramObject, "uMaterialAmbient");
        materialDiffusedUniform = GLES32.glGetUniformLocation(shaderPVProgramObject, "uMaterialDiffused");
        materialSpecularUniform = GLES32.glGetUniformLocation(shaderPVProgramObject, "uMaterialSpecular");
        materialShynessUniform = GLES32.glGetUniformLocation(shaderPVProgramObject, "uMaterialShyniness");


        ///////////
        Sphere sphere=new Sphere();
        float sphere_vertices[]=new float[1146];
        float sphere_normals[]=new float[1146];
        float sphere_textures[]=new float[764];
        short sphere_elements[]=new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices[0] = sphere.getNumberOfSphereVertices();
        numElements[0] = sphere.getNumberOfSphereElements();

        //Vao
        GLES32.glGenVertexArrays(1, this.vao_sphere, 0);
        GLES32.glBindVertexArray(this.vao_sphere[0]);

        GLES32.glGenBuffers(1, this.vbo_sphere_position, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_sphere_position[0]);
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_vertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AGS_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AGS_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vbo_sphere_normal, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_sphere_normal[0]);
        ByteBuffer byteNormalBuffer = ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteNormalBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer normalBuffer = byteNormalBuffer.asFloatBuffer();
        normalBuffer.put(sphere_normals);
        normalBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_normals.length * 4, normalBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AGS_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AGS_ATTRIBUTE_NORMAL);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        GLES32.glGenBuffers(1, this.vbo_sphere_element, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vbo_sphere_element[0]);
        ByteBuffer byteElementBuffer = ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteElementBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementBuffer = byteElementBuffer.asShortBuffer();
        elementBuffer.put(sphere_elements);
        elementBuffer.position(0);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_elements.length * 2, elementBuffer, GLES32.GL_STATIC_DRAW);
        //GLES32.glVertexAttribPointer(VertexAttributesEnum.AGS_ATTRIBUTE_NORMAL, 2, GLES32.GL_FLOAT, false, 0, 0);
        //GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AGS_ATTRIBUTE_NORMAL);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);         

        //Depth setting
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		//Enable culling
		GLES32.glEnable(GLES32.GL_CULL_FACE);
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        Matrix.setIdentityM(this.perspectiveProjectionMatrix, 0);

        /* light[0].ambient = vec3(lightAmbientZero[0], lightAmbientZero[1], lightAmbientZero[2]);
        light[0].diffused = vec3(lightDiffusedZero[0], lightDiffusedZero[1], lightDiffusedZero[2]);
        light[0].specular = vec3(lightSpecularZero[0], lightSpecularZero[1], lightSpecularZero[2]);
        //light[0].position = vec4(lightPostionZero[0], lightPostionZero[1], lightPostionZero[2], lightPostionZero[3]);
    
        light[1].ambient = vec3(lightAmbientOne[0], lightAmbientOne[1], lightAmbientOne[2]);
        light[1].diffused = vec3(lightDiffusedOne[0], lightDiffusedOne[1], lightDiffusedOne[2]);
        light[1].specular = vec3(lightSpecularOne[0], lightSpecularOne[1], lightSpecularOne[2]);
        //light[1].position = vec4(lightPostionOne[0], lightPostionOne[1], lightPostionOne[2], lightPostionOne[3]);
    
        light[2].ambient = vec3(lightAmbientTwo[0], lightAmbientTwo[1], lightAmbientTwo[2]);
        light[2].diffused = vec3(lightDiffusedTwo[0], lightDiffusedTwo[1], lightDiffusedTwo[2]);
        light[2].specular = vec3(lightSpecularTwo[0], lightSpecularTwo[1], lightSpecularTwo[2]);
        //light[1].position = vec4(lightPostionOne[0], lightPostionOne[1], lightPostionOne[2], lightPostionOne[3]); */
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
        

        if(choosenShader == 'F')
            GLES32.glUseProgram(shaderPFProgramObject);
	    else if(choosenShader == 'V')
            GLES32.glUseProgram(shaderPVProgramObject);
	    else
            GLES32.glUseProgram(shaderPVProgramObject);

        //Transformations
        float[] modelMatrix = new float[16];
        float[] viewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        float[] lightRotationMatrix = new float[16];
        float[] lightPositionZero = new float[4]; 

        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // Translate the modal view matrix.
        Matrix.translateM(modelMatrix, 0, 0.0f, 0.0f, -2.0f);

        // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
        GLES32.glUniformMatrix4fv(this.modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(this.viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(this.projectionMatrixUniform, 1, false, this.perspectiveProjectionMatrix, 0);


        if(bLightingEnabled) {
            
            GLES32.glUniform1i(this.isLightingEnabledUniform, 1);

            GLES32.glUniform3fv(lightAmbientUniform[0], 1, lightAmbientZero, 0);
            GLES32.glUniform3fv(lightDiffusedUniform[0], 1, lightDiffusedZero, 0);
            GLES32.glUniform3fv(lightSpecularUniform[0], 1, lightSpecularZero, 0);

            lightPostionZero[0] = 0.0f;
            lightPostionZero[1] = (float)Math.sin(lightAngleZero) * 10.0f;
            lightPostionZero[2] = (float)Math.cos(lightAngleZero) * 10.0f - 3.0f;
            lightPostionZero[3] = 1.0f;
            //light[0].position = vec4(lightPostionZero[0], lightPostionZero[1], lightPostionZero[2], lightPostionZero[3]);
            GLES32.glUniform4fv(lightPositionUniform[0], 1, lightPostionZero, 0);

            GLES32.glUniform3fv(lightAmbientUniform[1], 1, lightAmbientOne, 0);
            GLES32.glUniform3fv(lightDiffusedUniform[1], 1, lightDiffusedOne, 0);
            GLES32.glUniform3fv(lightSpecularUniform[1], 1, lightSpecularOne, 0);
            lightPostionOne[0] = (float)Math.sin(lightAngleOne) * 10.0f;
            lightPostionOne[1] = 0.0f;
            lightPostionOne[2] = (float)Math.cos(lightAngleOne) * 10.0f - 3.0f;
            lightPostionOne[3] = 1.0f;
            //light[1].position = vec4(lightPostionOne[0], lightPostionOne[1], lightPostionOne[2], lightPostionOne[3]);
            GLES32.glUniform4fv(lightPositionUniform[1], 1, lightPostionOne, 0);

            GLES32.glUniform3fv(lightAmbientUniform[2], 1, lightAmbientTwo, 0);
            GLES32.glUniform3fv(lightDiffusedUniform[2], 1, lightDiffusedTwo, 0);
            GLES32.glUniform3fv(lightSpecularUniform[2], 1, lightSpecularTwo, 0);
            lightPostionTwo[0] = (float)Math.cos(lightAngleTwo) * 100.0f - 3.0f;
            lightPostionTwo[1] = (float)Math.sin(lightAngleTwo) * 10.0f;
            lightPostionTwo[2] = 0.0f;
            lightPostionTwo[3] = 1.0f;
            //light[2].position = vec4(lightPostionTwo[0], lightPostionTwo[1], lightPostionTwo[2], lightPostionTwo[3]);
            GLES32.glUniform4fv(lightPositionUniform[2], 1, lightPostionTwo, 0);
    
            GLES32.glUniform3fv(materialAmbientUniform, 1, materialAmbient, 0); 
		    GLES32.glUniform3fv(materialDiffusedUniform, 1, materialDiffused, 0); 
            GLES32.glUniform3fv(materialSpecularUniform, 1, materialSpecular, 0); 
            GLES32.glUniform1f(materialShynessUniform, materialShininess);  
        }
        else {
            GLES32.glUniform1i(this.isLightingEnabledUniform, 0);
        }
        // bind vao
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements[0], GLES32.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao
        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);

        this.requestRender();
    }
    private void update()
    {
        if(bLightingEnabled)
        {
            lightAngleZero = lightAngleZero + 0.01f;
            if (lightAngleZero >= 360.0f)
            {
                lightAngleZero = 0.0f;
            }

            //Making Light 1
            lightAngleOne = lightAngleOne + 0.01f;
            if (lightAngleOne >= 360.0f)
            {
                lightAngleOne = 0.0f;
            }

            //Making Light 2
            lightAngleTwo = lightAngleTwo + 0.01f;
            if (lightAngleTwo >= 360.0f)
            {
                lightAngleTwo = 0.0f;
            }
        }

        if(bLightingEnabled) {

            if(choosenShader == 'F')
            {
                // After linking get the value of MVP uniform location from the shader program.
                this.modelMatrixUniform = GLES32.glGetUniformLocation(this.shaderPFProgramObject, "uModelMatrix");
                this.viewMatrixUniform = GLES32.glGetUniformLocation(this.shaderPFProgramObject, "uViewMatrix");
                this.projectionMatrixUniform = GLES32.glGetUniformLocation(this.shaderPFProgramObject, "uProjectionMatrix");

                this.isLightingEnabledUniform = GLES32.glGetUniformLocation(this.shaderPFProgramObject, "uKeyPressed");
               
                lightAmbientUniform[0] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightAmbient[0]");
                lightDiffusedUniform[0] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightDiffused[0]");
                lightSpecularUniform[0] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightSpecular[0]");
                lightPositionUniform[0] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightPosition[0]");
            
                lightAmbientUniform[1] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightAmbient[1]");
                lightDiffusedUniform[1] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightDiffused[1]");
                lightSpecularUniform[1] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightSpecular[1]");
                lightPositionUniform[1] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightPosition[1]");
            
                lightAmbientUniform[2] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightAmbient[2]");
                lightDiffusedUniform[2] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightDiffused[2]");
                lightSpecularUniform[2] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightSpecular[2]");
                lightPositionUniform[2] = GLES32.glGetUniformLocation(shaderPFProgramObject, "uLightPosition[2]");

                materialAmbientUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uMaterialAmbient");
                materialDiffusedUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uMaterialDiffused");
                materialSpecularUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uMaterialSpecular");
                materialShynessUniform = GLES32.glGetUniformLocation(shaderPFProgramObject, "uMaterialShyniness");
            }
            if(choosenShader == 'V')
            {
                // After linking get the value of MVP uniform location from the shader program.
                this.modelMatrixUniform = GLES32.glGetUniformLocation(this.shaderPVProgramObject, "uModelMatrix");
                this.viewMatrixUniform = GLES32.glGetUniformLocation(this.shaderPVProgramObject, "uViewMatrix");
                this.projectionMatrixUniform = GLES32.glGetUniformLocation(this.shaderPVProgramObject, "uProjectionMatrix");

                this.isLightingEnabledUniform = GLES32.glGetUniformLocation(this.shaderPVProgramObject, "uKeyPressed");

                lightAmbientUniform[0] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightAmbient[0]");
                lightDiffusedUniform[0] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightDiffused[0]");
                lightSpecularUniform[0] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightSpecular[0]");
                lightPositionUniform[0] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightPosition[0]");
            
                lightAmbientUniform[1] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightAmbient[1]");
                lightDiffusedUniform[1] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightDiffused[1]");
                lightSpecularUniform[1] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightSpecular[1]");
                lightPositionUniform[1] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightPosition[1]");
            
                lightAmbientUniform[2] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightAmbient[2]");
                lightDiffusedUniform[2] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightDiffused[2]");
                lightSpecularUniform[2] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightSpecular[2]");
                lightPositionUniform[2] = GLES32.glGetUniformLocation(shaderPVProgramObject, "uLightPosition[2]");
            
                materialAmbientUniform = GLES32.glGetUniformLocation(shaderPVProgramObject, "uMaterialAmbient");
                materialDiffusedUniform = GLES32.glGetUniformLocation(shaderPVProgramObject, "uMaterialDiffused");
                materialSpecularUniform = GLES32.glGetUniformLocation(shaderPVProgramObject, "uMaterialSpecular");
                materialShynessUniform = GLES32.glGetUniformLocation(shaderPVProgramObject, "uMaterialShyniness"); 
            }

        }
    }
    private void uninitialize() {

        if(this.shaderPFProgramObject >= 0)
        {   
            GLES32.glUseProgram(shaderPFProgramObject);
            int retVal[] = new int[1];

            GLES32.glGetProgramiv(shaderPFProgramObject, GLES32.GL_ATTACHED_SHADERS, retVal, 0);

            if(retVal[0] > 0)
            {
                int numAttachedShaders = retVal[0];
                int shaderObjects[] = new int[numAttachedShaders];
                if(numAttachedShaders > 0)
                {
                    GLES32.glGetAttachedShaders(shaderPFProgramObject, numAttachedShaders, retVal, 0, shaderObjects, 0);

                    for(int i = 0; i < numAttachedShaders; i++)
                    {
                        GLES32.glDetachShader(shaderPFProgramObject, shaderObjects[i]);
                        GLES32.glDeleteShader(shaderObjects[i]);
                        shaderObjects[i] = 0;
                    }
                }  
            }          

            GLES32.glUseProgram(0);
            GLES32.glDeleteProgram(shaderPFProgramObject);
            this.shaderPFProgramObject = 0;
        }

        if(vbo_sphere_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0]=0;
        }

        if(vbo_sphere_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0]=0;
        }

        if(vbo_sphere_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0]=0;
        }

        if(this.vao_sphere[0] >= 0) {
            GLES32.glDeleteVertexArrays(1, this.vao_sphere, 0);
            this.vao_sphere[0] = 0;
        }
    }

}
