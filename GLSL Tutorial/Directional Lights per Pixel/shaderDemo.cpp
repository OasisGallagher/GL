//
// Lighthouse3D.com GLSL Core Tutorial - Color Example
//
//
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
//
// If you do use it I would love to hear about it. Just post a comment
// at Lighthouse3D.com

// Have Fun :-)


#include <math.h>
#include <string>

#include "shaderDemo.h"

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

// Use Very Simple Libs
#include "VSMathlib.h"
#include "VSShaderlib.h"

#pragma comment(lib, "glew32.lib")

VSMathLib *vsml;
VSShaderLib shader;

// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = -43.0f, beta = 48.0f;
float r = 5.25f;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];

GLuint vao;


// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {

	float ratio;
	// Prevent a divide by zero, when window is too short
	if(h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix
	ratio = (1.0f * w) / h;
	vsml->loadIdentity(VSMathLib::PROJECTION);
	vsml->perspective(53.13f, ratio, 0.1f, 1000.0f);
}


// ------------------------------------------------------------
//
// Render stuff
//



void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// load identity matrices
	vsml->loadIdentity(VSMathLib::VIEW);
	vsml->loadIdentity(VSMathLib::MODEL);
	// set the camera using a function similar to gluLookAt
	vsml->lookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);

	float lightDir[4] = { 1.0f, 1.0f, 1.0f, 0.0f }; // light dir in world space
	float res[4]; // here we'll store the light's direction in camera space
	// transform light to camera space, normalise it, and send to GLSL
	vsml->multMatrixPoint(VSMathLib::VIEW, lightDir, res);
	vsml->normalize(res);
	shader.setBlockUniform("Lights", "l_dir", res);

	float diffuse[] = { 0.8f, 0.8f, 0.8f, 1.f };
	shader.setBlockUniform("Materials", "diffuse", diffuse);

	float specular[] = { 0.8f, 0.8f, 0.8f, 1.f };
	shader.setBlockUniform("Materials", "specular", specular);

	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.f };
	shader.setBlockUniform("Materials", "ambient", ambient);

	float shininess = 100.f;
	shader.setBlockUniform("Materials", "shininess", &shininess);

	float* view = vsml->get(VSMathLib::VIEW);

	// If you don't have non-uniform scaling as part of your model view matrix, 
	// the upper 3x3 of the model-view matrix is just as good as the "normal matrix" 
	// (which is the transpose of the inverse of the model view matrix) for transforming normals.
	// Otherwise, see
	// http://www.lighthouse3d.com/tutorials/glsl-tutorial/spaces-and-matrices/
	float normalMatrix[9];
	int index = 0;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			normalMatrix[index++] = view[i * 4 + j];
		}
	}

	shader.setBlockUniform("Matrices", "m_normal", normalMatrix);
	shader.setBlockUniform("Matrices", "m_viewModel", view);

	// use our shader
	glUseProgram(shader.getProgramIndex());
	// send matrices to uniform buffer
	vsml->matricesToGL();
	// render VAO
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);
	//swap buffers
	glutSwapBuffers();
}


// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{
	switch(key) {

		case 27:
			glutLeaveMainLoop();
			break;

		case 'c': 
			printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
			break;

	}

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
			if (r < 0.1f)
				r = 0.1f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX =  - xx + startX;
	deltaY =    yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = r;
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}





void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}




// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	// Shader for models
	shader.init();
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/color.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/color.frag");

	// set semantics for the shader variables
	shader.setProgramOutput(0, "outputF");
	shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");

	shader.prepareProgram();

	// this is only useful for the uniform version of the shader
	float c[4] = { 1.0f, 0.8f, 0.2f, 1.0f };
	shader.setUniform("color", c);

	printf("InfoLog for Hello World Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	return(shader.isProgramValid());
}



// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//



void initOpenGL()
{
	// set the camera position based on its spherical coordinates
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// create the VAO
	// Vertex Array Object-VAO, VAO是一个对象, 其中包含一个或者更多的Vertex Buffer Objects.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create buffers for our vertex data
	GLuint buffers[4];
	glGenBuffers(4, buffers);

	//vertex coordinates buffer
	// 将vertices绑定到Vertex shader的location = VSShaderLib::VERTEX_COORD_ATTRIB(0)的变量上.
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);

	//texture coordinates buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::TEXTURE_COORD_ATTRIB);
	glVertexAttribPointer(VSShaderLib::TEXTURE_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);

	//normals buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::NORMAL_ATTRIB);
	glVertexAttribPointer(VSShaderLib::NORMAL_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

	//index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faceIndex), faceIndex, GL_STATIC_DRAW);

	// unbind the VAO
	glBindVertexArray(0);
}


void initVSL() {
	vsml = VSMathLib::getInstance();
	// tell VSL the uniform block name
	vsml->setUniformBlockName("Matrices");
	// set semantics for the matrix variable
	vsml->setUniformName(VSMathLib::PROJ_VIEW_MODEL, "m_pvm");
}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (3, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(512,512);
	glutCreateWindow("Lighthouse3D - Simple Shader Demo");

//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	glutMouseWheelFunc ( mouseWheel ) ;

//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
	{
		system("pause");
		return(1);
	}

	initOpenGL();

	initVSL();

	//  GLUT main loop
	glutMainLoop();

	return(0);

}

