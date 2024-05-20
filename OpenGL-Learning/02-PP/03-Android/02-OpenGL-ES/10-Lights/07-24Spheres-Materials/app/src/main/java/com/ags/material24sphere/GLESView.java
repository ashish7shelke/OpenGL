package com.ags.material24sphere;

import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

//Event related packages
import android.view.MotionEvent;
import com.ags.material24sphere.VertexAttributesEnum;
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

    private int shaderProgramObject = 0;
    private int mvpUniform = 0;

    private float[] perspectiveProjectionMatrix = new float[16];

    private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
    private int[] numVertices = new int[1];
    private int[] numElements = new int[1];

    private boolean bLightingEnabled = false;
    private int modelMatrixUniform = 0;
    private int viewMatrixUniform = 0;
    private int projectionMatrixUniform = 0;
    
    private int lightAmbientUniform = 0;
    private int lightDiffusedUniform = 0;
    private int lightSpecularUniform = 0;
    private int lightPositionUniform = 0;

    private int materialAmbientUniform = 0;
    private int materialDiffusedUniform = 0;
    private int materialSpecularUniform = 0;
    private int materialShynessUniform = 0;

    private boolean isLightingEnabled = false;
    private boolean isAnimationEnabled = false;
    private boolean rotateLightOnXAxis = false;
    private boolean rotateLightOnYAxis = false;
    private boolean rotateLightOnZAxis = false;
    private float angleX = 0.0f;
    private float angleY = 0.0f;
    private float angleZ = 0.0f;

    private final int numberOfRows = 6;
    private final int numberOfColumns = 4;
    
    private int viewportWidth = 1;
    private int viewportHeight = 1;
    private int viewportInitialX = 0;
    private int viewportInitialY = 0;
    private int doubleTapCounter = 1;
    private int singleTapCounter = 0;    

    private float[] lightAmbient = {0.0f, 0.0f, 0.0f, 1.0f};
    private float[] lightDiffused = {1.0f, 1.0f, 1.0f, 1.0f};
    private float[] lightSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
    private float[] lightPosition = {0.0f, 0.0f, 0.0f, 1.0f};

    private float materialAmbient[][][] = {
        {   // Column 1
            {0.0215f, 0.1745f, 0.0215f, 1.0f },
            {0.135f, 0.2225f, 0.1575f, 1.0f },
            {0.05375f, 0.05f, 0.06625f, 1.0f },
            {0.25f, 0.20725f, 0.20725f, 1.0f },
            {0.1745f, 0.01175f, 0.01175f, 1.0f },
            {0.1f, 0.18725f, 0.1745f, 1.0f }
        },
        {   // Column 2
            {0.329412f, 0.223529f, 0.027451f, 1.0f },
            {0.2125f, 0.1275f, 0.054f, 1.0f },
            {0.25f, 0.25f, 0.25f, 1.0f },
            {0.19125f, 0.0735f, 0.0225f, 1.0f },
            {0.24725f, 0.1995f, 0.0745f, 1.0f },
            {0.19225f, 0.19225f, 0.19225f, 1.0f }
        },
        {   // Column 3
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.1f, 0.06f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f }
        },
        {   // Column 4
            {0.02f, 0.02f, 0.02f, 1.0f },
            {0.0f, 0.05f, 0.05f, 1.0f },
            {0.0f, 0.05f, 0.0f, 1.0f },
            {0.05f, 0.0f, 0.0f, 1.0f },
            {0.05f, 0.05f, 0.05f, 1.0f },
            {0.05f, 0.05f, 0.0f, 1.0f }
        }
    };

    private float materialDiffuse[][][] = {
        {   // Column 1
            {0.07568f, 0.61424f, 0.07568f, 1.0f},
            {0.54f, 0.89f, 0.63f, 1.0f},
            {0.18275f, 0.17f, 0.22525f, 1.0f},
            {1.0f, 0.829f, 0.829f, 1.0f},
            {0.61424f, 0.04136f, 0.04136f, 1.0f},
            {0.396f, 0.74151f, 0.69102f, 1.0f},
        },
        {   // Column 2
            {0.780392f, 0.568627f, 0.113725f, 1.0f},
            {0.714f, 0.4284f, 0.18144f, 1.0f},
            {0.4f, 0.4f, 0.4f, 1.0f},
            {0.7038f, 0.27048f, 0.0828f, 1.0f},
            {0.75164f, 0.60648f, 0.22648f, 1.0f},
            {0.50754f, 0.50754f, 0.50754f, 1.0f},
        },
        {   // Column 3
            {0.01f, 0.01f, 0.01f, 1.0f},
            {0.0f, 0.50980392f, 0.50980392f, 1.0f},
            {0.1f, 0.35f, 0.1f, 1.0f},
            {0.5f, 0.0f, 0.0f, 1.0f},
            {0.55f, 0.55f, 0.55f, 1.0f},
            {0.5f, 0.5f, 0.0f, 1.0f},
        },
        {   // Column 4
            {0.01f, 0.01f, 0.01f, 1.0f},
            {0.4f, 0.5f, 0.5f, 1.0f},
            {0.4f, 0.5f, 0.4f, 1.0f},
            {0.5f, 0.4f, 0.4f, 1.0f},
            {0.5f, 0.5f, 0.5f, 1.0f},
            {0.5f, 0.5f, 0.4f, 1.0f},
        },
    };

    private float materialSpecular[][][] = {
        {   // Column 1
            {0.633f, 0.727811f, 0.633f, 1.0f},
            {0.316228f, 0.316228f, 0.316228f, 1.0f},
            {0.332741f, 0.328634f, 0.346435f, 1.0f},
            {0.296648f, 0.296648f, 0.296648f, 1.0f},
            {0.727811f, 0.626959f, 0.626959f, 1.0f},
            {0.297254f, 0.30829f, 0.306678f, 1.0f},
        },
        {   // Column 2
            {0.992157f, 0.941176f, 0.807843f, 1.0f},
            {0.393548f, 0.271906f, 0.166721f, 1.0f},
            {0.774597f, 0.774597f, 0.774597f, 1.0f},
            {0.256777f, 0.137622f, 0.086014f, 1.0f},
            {0.628281f, 0.555802f, 0.366065f, 1.0f},
            {0.508273f, 0.508273f, 0.508273f, 1.0f},
        },
        {   // Column 3
            {0.50f, 0.50f, 0.50f, 1.0f},
            {0.50196078f, 0.50196078f, 0.50196078f, 1.0f},
            {0.45f, 0.55f, 0.45f, 1.0f},
            {0.7f, 0.6f, 0.6f, 1.0f},
            {0.70f, 0.70f, 0.70f, 1.0f},
            {0.60f, 0.60f, 0.50f, 1.0f},
        },
        {   // Column 4
            {0.4f, 0.4f, 0.4f, 1.0f},
            {0.04f, 0.7f, 0.7f, 1.0f},
            {0.04f, 0.7f, 0.04f, 1.0f},
            {0.7f, 0.04f, 0.04f, 1.0f},
            {0.7f, 0.7f, 0.7f, 1.0f},
            {0.7f, 0.7f, 0.04f, 1.0f},
        }
    };

    private float materialShininess[][] = {
        {   // Column 1
            0.6f * 128.0f,
            0.1f * 128.0f,
            0.3f * 128.0f,
            0.088f * 128.0f,
            0.6f * 128.0f,
            0.1f * 128.0f
        },
        {   // Column 2
            0.21794872f * 128.0f,
            0.2f * 128.0f,
            0.6f * 128.0f,
            0.1f * 128.0f,
            0.4f * 128.0f,
            0.4f * 128.0f
        },
        {   // Column 3
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f
        },
        {   // Column 4
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f
        }
    };    
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
        doubleTapCounter++;

        if(doubleTapCounter > 9) {
            doubleTapCounter = 1;
        }
    
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
        this.singleTapCounter++;

        if(this.singleTapCounter > 3) {
            this.singleTapCounter = 1;

        }

        if(singleTapCounter == 1){
            // Rotating on X-Axis
            this.rotateLightOnXAxis = true;
            this.rotateLightOnYAxis = false;
            this.rotateLightOnZAxis = false;
        }
        else if(singleTapCounter == 2){
            // Rotating on Y-Axis
            this.rotateLightOnXAxis = false;
            this.rotateLightOnYAxis = true;
            this.rotateLightOnZAxis = false;
        }
        else if(singleTapCounter == 3){
            // Rotating on Z-Axis
            this.rotateLightOnXAxis = false;
            this.rotateLightOnYAxis = false;
            this.rotateLightOnZAxis = true;
        }
        else 
        {
            this.rotateLightOnXAxis = false;
            this.rotateLightOnYAxis = false;
            this.rotateLightOnZAxis = false;
        }

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
            + "uniform mat4 uModelMatrix;\n"
            + "uniform mat4 uViewMatrix;\n"
            + "uniform mat4 uProjectionMatrix;\n"
            + "uniform vec4 uLightPosition;\n"
            + "uniform mediump int uKeyPressed;\n"
            + "out vec3 oTransformedNormals;\n"
            + "out vec3 oLightDirection;\n"
            + "out vec3 oViewerVector;\n"
            + "void main(void)\n"
            + "{\n"
            + "if(uKeyPressed == 1) \n"
            + "{\n"
            + "vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition; \n"
            + "oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal; \n"
            + "oLightDirection = vec3(uLightPosition - eyeCoordinates); \n"
            + "oViewerVector = -eyeCoordinates.xyz;\n"
            + "}\n"
            + "else \n"
            + "{\n"
            + "oTransformedNormals = vec3(0.0f, 0.0f, 0.0f); \n"
            + "oLightDirection = vec3(0.0f, 0.0f, 0.0f); \n"
            + "oViewerVector = vec3(0.0f, 0.0f, 0.0f); \n"
            + "}\n"
            + "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n"
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
            + "precision highp float;"
            + "in vec3 oTransformedNormals;\n"
            + "in vec3 oLightDirection;\n"
            + "in vec3 oViewerVector;\n"
            + "uniform vec3 uLightAmbient;\n"
            + "uniform vec3 uLightDiffused;\n"
            + "uniform vec3 uLightSpecular;\n"
            + "uniform vec4 uLightPosition;\n"
            + "uniform vec3 uMaterialAmbient;\n"
            + "uniform vec3 uMaterialDiffused;\n"
            + "uniform vec3 uMaterialSpecular;\n"
            + "uniform float uMaterialShyniness;\n"
            + "uniform int uKeyPressed;\n"
            + "out vec4 FragColor;\n"
            + "void main(void)\n"
            + "{\n"
            + "vec3 phongADSLight; \n"
            + "if(uKeyPressed == 1) \n"
            + "{\n"
            + "vec3 normalizedTransformedNormals = normalize(oTransformedNormals); \n"
            + "vec3 normalizedLightDirection = normalize(oLightDirection); \n"
            + "vec3 normalizedViewerVector = normalize(oViewerVector); \n"
            + "vec3 reflectionVector = reflect(-normalizedLightDirection, normalizedTransformedNormals);\n"
            + "vec3 diffusedLight = uLightDiffused * uMaterialDiffused * max(dot(normalizedLightDirection, normalizedTransformedNormals), 0.0f); \n"
            + "vec3 ambientLight = uLightAmbient * uMaterialAmbient;\n"
            + "vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector, normalizedViewerVector), 0.0f), uMaterialShyniness);\n"
            + "phongADSLight = (ambientLight + diffusedLight + specularLight); \n"
            + "}\n"
            + "else \n"
            + "{\n"
            + "phongADSLight = vec3(1.0f, 1.0f, 1.0f); \n"
            + "}\n"
            + "FragColor = vec4(phongADSLight, 1.0f); \n"
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
        this.modelMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uModelMatrix");
        this.viewMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uViewMatrix");
        this.projectionMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uProjectionMatrix");

        this.isLightingEnabledUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "uKeyPressed");

        lightAmbientUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightAmbient");
        lightDiffusedUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightDiffused");
        lightSpecularUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightSpecular");
        lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightPosition");
    
        materialAmbientUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialAmbient");
        materialDiffusedUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialDiffused");
        materialSpecularUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialSpecular");
        materialShynessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialShyniness");
    
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

        this.viewportWidth = width / this.numberOfRows;
        this.viewportHeight = height / this.numberOfRows;
        this.viewportInitialX = (width - (this.viewportWidth * this.numberOfColumns)) / 2;
        this.viewportInitialY = (height - (this.viewportHeight * this.numberOfRows)) / 2;

    
        GLES32.glViewport(0, 0, width, height);
        Matrix.perspectiveM(this.perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 1.0f, 100.0f);
    }

    private void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT | GLES32.GL_STENCIL_BUFFER_BIT);

        GLES32.glUseProgram(shaderProgramObject);

        for(int columnCounter = 0; columnCounter < numberOfColumns; ++columnCounter)
        {
            for(int rowCounter = 0; rowCounter < numberOfRows; ++rowCounter)
            {
                GLES32.glViewport(this.viewportInitialX + (columnCounter * this.viewportWidth), this.viewportInitialY + (rowCounter * this.viewportHeight), this.viewportWidth, this.viewportHeight);

                //Transformations
                float[] modelMatrix = new float[16];
                float[] viewMatrix = new float[16];
                float[] modelViewProjectionMatrix = new float[16];

                float[] rotationMatrix = new float[16];
                Matrix.setIdentityM(modelMatrix, 0);
                Matrix.setIdentityM(viewMatrix, 0);
                Matrix.setIdentityM(rotationMatrix, 0);
                Matrix.setIdentityM(modelViewProjectionMatrix, 0);

                // Translate the modal view matrix.
                Matrix.translateM(modelMatrix, 0, 0.0f, 0.0f, -3.0f);

                // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
                GLES32.glUniformMatrix4fv(this.modelMatrixUniform, 1, false, modelMatrix, 0);
                GLES32.glUniformMatrix4fv(this.viewMatrixUniform, 1, false, viewMatrix, 0);
                GLES32.glUniformMatrix4fv(this.projectionMatrixUniform, 1, false, this.perspectiveProjectionMatrix, 0);

                if(bLightingEnabled) {

                    if(rotateLightOnXAxis) {
                        lightPosition[0] = (float)Math.sin(angleX) * 100.0f;
                        lightPosition[1] = 0.0f;
                        lightPosition[2] = (float)Math.cos(angleX) * 100.0f - 3.0f;
                        lightPosition[3] = 1.0f;
                    }
                    else if(rotateLightOnYAxis) {
                        lightPosition[0] = 0.0f;
                        lightPosition[1] = (float)Math.sin(angleY) * 100.0f;
                        lightPosition[2] = (float)Math.cos(angleY) * 100.0f - 3.0f;
                        lightPosition[3] = 1.0f;
                        
                    }
                    else if(rotateLightOnZAxis) {
                        lightPosition[0] = (float)Math.cos(angleZ) * 100.0f - 3.0f;
                        lightPosition[1] = (float)Math.sin(angleZ) * 100.0f;
                        lightPosition[2] = 0.0f;
                        lightPosition[3] = 1.0f;
                    }

                    GLES32.glUniform1i(this.isLightingEnabledUniform, 1);

                    GLES32.glUniform3fv(lightAmbientUniform, 1, lightAmbient, 0); 
                    GLES32.glUniform3fv(lightDiffusedUniform, 1, lightDiffused, 0);
                    GLES32.glUniform3fv(lightSpecularUniform, 1, lightSpecular, 0);
                    GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);
                               
                    GLES32.glUniform3fv(materialAmbientUniform, 1, this.materialAmbient[columnCounter][rowCounter], 0);
                    GLES32.glUniform3fv(materialDiffusedUniform, 1, this.materialDiffuse[columnCounter][rowCounter], 0);
                    GLES32.glUniform3fv(materialSpecularUniform, 1, this.materialSpecular[columnCounter][rowCounter], 0);
                    GLES32.glUniform1f(materialShynessUniform, this.materialShininess[columnCounter][rowCounter]);

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
            }
        }

        GLES32.glUseProgram(0);

        this.requestRender();
    }

    private void update()
    {
        if(bLightingEnabled == true)
        {
            if(rotateLightOnXAxis) {
                this.angleX = this.angleX + 0.05f;
                if(this.angleX >= 360.0f) {
                    this.angleX = 0.0f;
                }
            }
            if(rotateLightOnYAxis) {
                this.angleY = this.angleY + 0.05f;
                if(this.angleY >= 360.0f) {
                    this.angleY = 0.0f;
                }
            }
            if(rotateLightOnZAxis) {
                this.angleZ = this.angleZ + 0.05f;
                if(this.angleZ >= 360.0f) {
                    this.angleZ = 0.0f;
                }
            }            
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

        System.out.println("AGS: Program finished " );
    }

}

//>adb logcat | findstr  AGS:

