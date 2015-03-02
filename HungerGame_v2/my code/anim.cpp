// anim.cpp version 5.0 -- Template code for drawing an articulated figure.  CS 174A.
#include "../CS174a template/Utilities.h"
#include "../CS174a template/Shapes.h"
#include "myConstant.h"
#include "math.h"

std::stack<mat4> mvstack;

int g_width = 800, g_height = 800 ;
float zoom = 1 ;

int animate = 0, recording = 0, basis_to_display = -1;
double TIME = 0;

const unsigned X = 0, Y = 1, Z = 2;

 vec4 eye(0 , 200, -15, 1), ref( 0 ,0, -10, 1), up( 0, 1, 0, 0 );	// The eye point and look-at point.

//vec4 eye( 0, 0, 20, 1), ref( 0 , 0, 0, 1 ), up( 0, 1, 0, 0 );	// The eye point and look-at point.
mat4	orientation, model_view;
ShapeData chickenData, cubeData, sphereData, coneData, cylData;				// Structs that hold the Vertex Array Object index and number of vertices of each shape.
GLuint	texture_wolf, texture_tree, texture_chicken, texture_wolf_mouth, texture_skin, texture_earth;
GLint   uModelView, uProjection, uView,
		uAmbient, uDiffuse, uSpecular, uLightPos, uShininess,
		uTex, uEnableTex;

void init()
{
#ifdef EMSCRIPTEN
    GLuint program = LoadShaders( "vshader.glsl", "fshader.glsl" );								// Load shaders and use the resulting shader program
    TgaImage coolImage ("wolf.tga");
     TgaImage treeImage("skin.tga");
     TgaImage wolfmouthImage("wolf_mouth.tga");
    TgaImage treeImage("tree.tga");
    TgaImage chickenImage("wolf.tga");

#else
	GLuint program = LoadShaders( "../my code/vshader.glsl", "../my code/fshader.glsl" );		// Load shaders and use the resulting shader program
    TgaImage wolfImage ("../my code/wolf.tga");
    TgaImage skinImage ("../my code/skin.tga");
    TgaImage wolfmouthImage("../my code/wolf_mouth.tga");
    TgaImage treeImage("../my code/tree.tga");
    TgaImage chickenImage("../my code/chicken.tga");
    TgaImage earthImage("../my code/earth.tga");

#endif
    glUseProgram(program);

	generateCube(program, &cubeData);		// Generate vertex arrays for geometric shapes
    generateSphere(program, &sphereData);
    generateCone(program, &coneData);
    generateCylinder(program, &cylData);
    generateChicken(program, &chickenData);

    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView		= glGetUniformLocation( program, "View"       );
    uAmbient	= glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse	= glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular	= glGetUniformLocation( program, "SpecularProduct" );
    uLightPos	= glGetUniformLocation( program, "LightPosition"   );
    uShininess	= glGetUniformLocation( program, "Shininess"       );
    uTex		= glGetUniformLocation( program, "Tex"             );
    uEnableTex	= glGetUniformLocation( program, "EnableTex"       );

    glUniform4f( uAmbient,    0.2,  0.2,  0.2, 1 );
    glUniform4f( uDiffuse,    0.6,  0.6,  0.6, 1 );
    glUniform4f( uSpecular,   0.2,  0.2,  0.2, 1 );
    glUniform4f( uLightPos,  15.0, 15.0, 30.0, 0 );
    glUniform1f( uShininess, 100);

    glEnable(GL_DEPTH_TEST);
    
    glGenTextures( 1, &texture_wolf );
    glBindTexture( GL_TEXTURE_2D, texture_wolf );

    glTexImage2D(GL_TEXTURE_2D, 0, 4, wolfImage.width, wolfImage.height, 0,
                 (wolfImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, wolfImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
     glGenTextures( 1, &texture_skin );
    glBindTexture( GL_TEXTURE_2D, texture_skin );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, skinImage.width, skinImage.height, 0,
                 (skinImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, skinImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    glGenTextures( 1, &texture_tree );
    glBindTexture( GL_TEXTURE_2D, texture_tree );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, treeImage.width, treeImage.height, 0,
                 (treeImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, treeImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    
    glGenTextures( 1, &texture_chicken );
    glBindTexture( GL_TEXTURE_2D, texture_chicken );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, chickenImage.width, chickenImage.height, 0,
                 (chickenImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, chickenImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    
    glGenTextures( 1, &texture_wolf_mouth );
    glBindTexture( GL_TEXTURE_2D, texture_wolf_mouth );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, wolfmouthImage.width, wolfmouthImage.height, 0,
                 (wolfmouthImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, wolfmouthImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    glGenTextures( 1, &texture_earth );
    glBindTexture( GL_TEXTURE_2D, texture_earth );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, earthImage.width, earthImage.height, 0,
                 (earthImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, earthImage.data );
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glUniform1i( uTex, 0);	// Set texture sampler variable to texture unit 0
	
	glEnable(GL_DEPTH_TEST);
}

struct color{ color( float r, float g, float b) : r(r), g(g), b(b) {} float r, g, b;};
std::stack<color> colors;
void set_color(float r, float g, float b)
{
	colors.push(color(r, g, b));

	float ambient  = 0.2, diffuse  = 0.6, specular = 0.2;
    glUniform4f(uAmbient,  ambient*r,  ambient*g,  ambient*b,  1 );
    glUniform4f(uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1 );
    glUniform4f(uSpecular, specular*r, specular*g, specular*b, 1 );
}

int mouseButton = -1, prevZoomCoord = 0 ;
vec2 anchor;
void myPassiveMotionCallBack(int x, int y) {	anchor = vec2( 2. * x / g_width - 1, -2. * y / g_height + 1 ); }

void myMouseCallBack(int button, int state, int x, int y)	// start or end mouse interaction
{
    mouseButton = button;
   
    if( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
        mouseButton = -1 ;
    if( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
        prevZoomCoord = -2. * y / g_height + 1;

    glutPostRedisplay() ;
}

void myMotionCallBack(int x, int y)
{
	vec2 arcball_coords( 2. * x / g_width - 1, -2. * y / g_height + 1 );
	 
    if( mouseButton == GLUT_LEFT_BUTTON )
    {
	   orientation = RotateX( -10 * (arcball_coords.y - anchor.y) ) * orientation;
	   orientation = RotateY(  10 * (arcball_coords.x - anchor.x) ) * orientation;
    }
	
	if( mouseButton == GLUT_RIGHT_BUTTON )
		zoom *= 1 + .1 * (arcball_coords.y - anchor.y);
    glutPostRedisplay() ;
}

double pause_time = 0;
void idleCallBack(void)
{
    if( !animate ){
        return;
    }
	double prev_time = TIME;
    TIME = TM.GetElapsedTime() + pause_time;

    if( prev_time == 0 ) {
        TM.Reset();
        pause_time = 0;
    }
    glutPostRedisplay() ;
}


void drawChicken() {
   
	glBindTexture( GL_TEXTURE_2D, texture_chicken );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( chickenData.vao );
    glDrawArrays( GL_TRIANGLES, 0, chickenData.numVertices );
    glUniform1i( uEnableTex, 0 );

    
}
void drawCylinder()	//render a solid cylinder oriented along the Z axis; bases are of radius 1, placed at Z = 0, and at Z = 1.
{
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cylData.numVertices );
}

void drawCone()	//render a solid cone oriented along the Z axis; bases are of radius 1, placed at Z = 0, and at Z = 1.
{
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( coneData.vao );
    glDrawArrays( GL_TRIANGLES, 0, coneData.numVertices );
}


void drawCube(GLuint tc, int enable = 1)		// draw a cube with dimensions 1,1,1 centered around the origin.
{
	glBindTexture( GL_TEXTURE_2D, tc );
    glUniform1i( uEnableTex, enable );
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawCube() {
    drawCube(texture_wolf, 0);
    
}

void drawSphere()	// draw a sphere with radius 1 centered around the origin.
{
    glBindTexture( GL_TEXTURE_2D, texture_earth);
    glUniform1i( uEnableTex, 1);
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

int basis_id = 0;
void drawOneAxis()
{
	mat4 origin = model_view;
	model_view *= Translate	( 0, 0, 4 );
	model_view *= Scale(.25) * Scale( 1, 1, -1 );
	drawCone();
	model_view = origin;
	model_view *= Translate	( 1,  1, .5 );
	model_view *= Scale		( .1, .1, 1 );
	drawCube();
	model_view = origin;
	model_view *= Translate	( 1, 0, .5 );
	model_view *= Scale		( .1, .1, 1 );
	drawCube();
	model_view = origin;
	model_view *= Translate	( 0,  1, .5 );
	model_view *= Scale		( .1, .1, 1 );
	drawCube();
	model_view = origin;
	model_view *= Translate	( 0,  0, 2 );
	model_view *= Scale(.1) * Scale(   1, 1, 20);
    drawCylinder();	
	model_view = origin;
}

void drawAxes(int selected)
{
	if( basis_to_display != selected ) 
		return;
	mat4 given_basis = model_view;
	model_view *= Scale		(.25);
	drawSphere();
	model_view = given_basis;
	set_color( 0, 0, 1 );
	drawOneAxis();
	model_view *= RotateX	(-90);
	model_view *= Scale		(1, -1, 1);
	set_color( 1, 1, 1);
	drawOneAxis();
	model_view = given_basis;
	model_view *= RotateY	(90);
	model_view *= Scale		(-1, 1, 1);
	set_color( 1, 0, 0 );
	drawOneAxis();
	model_view = given_basis;
	
	colors.pop();
	colors.pop();
	colors.pop();
	set_color( colors.top().r, colors.top().g, colors.top().b );
}

void drawGround(){
	mvstack.push(model_view);
    set_color( .0, .8, .0 );
    model_view *= Translate	(0, -10, 0);									drawAxes(basis_id++);
    model_view *= Scale		(100, 1, 100);									drawAxes(basis_id++);
    drawCube();
    colors.pop();
	model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
}



void placeChicken(mat4 cBody,
                  mat4 leftFoot,
                  mat4 rightFoot,
                  mat4 cLeftWing,
                  mat4 cRightWing) {
    
    set_color(1, 1, 1);
    mvstack.push(model_view);
    model_view *= cBody;
    model_view *= Scale(0.5,0.5,0.5);
    mat4 temp_view = model_view;
    

    drawChicken();
  
    set_color(173/255.0, 81/255.0, 0/255.0);
    model_view *= Translate(0.5, 0, -2);
    model_view *= leftFoot;
    model_view *= Translate(0, -1, 0);
    model_view *= Scale(0.5, 2, 0.5);
    drawCube();

    

    model_view = temp_view;
    model_view *= Translate(1.5, 0, -2);
    model_view *= rightFoot;
    model_view *= Translate(0, -1, 0);
    model_view *= Scale(0.5, 2, 0.5);
    drawCube();
    
    colors.pop();
    set_color(184/255.0, 68/255.0, 93/255.0);
    
    model_view = temp_view;
    model_view *= Translate(1, 4, 0);
    mvstack.push(model_view);
    model_view *= Translate(0, 0, 0.5);
    model_view *= Scale(0.7, 0.7, 1);
    drawCube();
    
    model_view =mvstack.top();
    mvstack.pop();
    model_view *= Translate(0, -0.7, 0);
    model_view *= Translate(0, 0, 0.35);
    model_view *= Scale(0.7, 0.7, 0.7);
    drawCube();
    
    model_view = temp_view;
    model_view *= Translate(1, 5, -1.5);
    model_view *= Scale(1, 1, 1.5);
    drawCube();
    
    colors.pop();
    set_color(1,1,1);
    model_view = temp_view;
    model_view *= Translate(-0.15, 2.2, -1.75);
    model_view *= cLeftWing;
    model_view *= Translate(0, -1.1, 0);
    model_view *= Scale(0.3,2.2,1.2);
    drawCube();
    
    model_view = temp_view;
    model_view *= Translate(2.15, 2.2, -1.75);
    model_view *= cRightWing;
    model_view *= Translate(0, -1.1, 0);
    model_view *= Scale(0.3, 2.2, 1.2);
    drawCube();
    
    model_view = temp_view;
    model_view *= Translate(1, 1.5, -4);
    drawCube();
    colors.pop();
    
    model_view = mvstack.top();
    mvstack.pop();
    colors.pop();
    
    
}

void drawTree(double x, double z) {
    mvstack.push(model_view);

    set_color(106/255.0, 76/255.0, 38/255.0);
    model_view *=Translate(x, 0, z);
    model_view *= Translate(0, 1.5, 0);
    model_view *= Scale(2.5, 3, 2.5);
    drawCube(texture_tree);
    
     set_color(47/255.0, 150/255.0, 39/255.0);
//    model_view *= Scale(1/2, 1/3, 1/2);
    model_view *= Translate(0,0.5,0);
    model_view *= Translate(0, 1, 0);
    model_view *= Scale(3, 2, 3);
    drawCube(texture_tree);
   
    colors.pop();
    colors.pop();
    model_view = mvstack.top();
    mvstack.pop();
    
}


void drawHunter(mat4 aBody,
                mat4 aHead,
                mat4 aLeftArm,
                mat4 aRightArm,
                mat4 aLeftLeg,
                mat4 aRightLeg ) {
    
    mvstack.push(model_view);
    // move to position aBodyPosition
    //mat4 temp_view = model_view * Translate(0, 0, TIME*-10);
    model_view *= aBody;
    
    mat4 temp_view = model_view;
    // draw body
    model_view *= Scale(2,3,1);
    drawCube();
    
    // draw head
    model_view = temp_view;
    model_view *= Translate(0, 1.5, 0) * Translate(0, 0.75, 0) * Scale(2, 1.5, 2);
    model_view *= aHead;
    drawCube();
    
    // draw leftArm
    model_view = temp_view;
    model_view *= Translate(-1, 1.25, 0);
    model_view *= Translate(-0.25, 0, 0);
    model_view *= aLeftArm;
    model_view *= Translate(0, -1.25, 0);
    model_view *= Scale(1,3,1);
    drawCube();
    
    // draw rightArm
    model_view = temp_view;
    model_view *= Translate(1, 1.25, 0);
    model_view *= Translate(0.25, 0, 0);
    model_view *= aRightArm;
    model_view *= Translate(0, -1.25, 0);
    model_view *= Scale(1,3,1);
    drawCube();
    
    // draw leftLeg
    model_view = temp_view;
    model_view *= Translate(-0.5, -1.3, 0);
    model_view *= aLeftLeg;
    model_view *= Translate(0,-1.5, 0);
    model_view *= Scale(1,3,1);
    drawCube();

    // draw leftLeg
    model_view = temp_view;
    model_view *= Translate(0.5, -1.3, 0);
    model_view *= aRightLeg;
    model_view *= Translate(0,-1.5, 0);
    model_view *= Scale(1,3,1);
    drawCube();
    
    model_view = mvstack.top();
    mvstack.pop();
    
    
    
    
}

void drawWolf(mat4 wBody, mat4 wHead, mat4 wfrontLeg, mat4 wbackLeg, mat4 wtail) {
    
    mvstack.push(model_view);
    model_view *= wBody;
    mat4 temp_view = model_view;
    model_view *=Scale(1, 1, 3);
    drawCube(texture_skin);
    model_view = temp_view;
    
    model_view *= Translate(0, 0, 1.2);
    model_view *= Scale(1.3,1.3,1.3);
    drawCube(texture_skin);
    
    model_view = temp_view;
    model_view *= Translate(0,0,1.2);
    model_view *= wHead;
    model_view *= Translate(0, 0, 0.8);
    mvstack.push(model_view);
    drawCube(texture_wolf);
    
    // mouth
    model_view *= Translate(0, -0.25,0.5);
    model_view *= Scale(0.5, 0.3, 0.8);
    drawCube(texture_wolf_mouth);

    // ears
    model_view = mvstack.top();
    mvstack.pop();
    mvstack.push(model_view);
    model_view *= Translate(0.25, 0.65, 0);
    model_view *=Scale(0.4,0.5,0.2);
    drawCube(texture_skin);
    
    model_view = mvstack.top();
    model_view *= Translate(-0.25,0.65, 0);
    model_view *=Scale(0.4,0.5,0.2);
    mvstack.pop();
    drawCube(texture_skin);
    
    
    model_view = temp_view;
    model_view *= Translate(0.3, -0.3, 1.3);
    model_view *= wfrontLeg;
    model_view *= Translate(0, -0.5, 0);
    model_view *= Scale(0.3,1,0.3);
    drawCube(texture_skin);
    
    model_view = temp_view;
    model_view *= Translate(0.3, -0.3, -1.1);
    model_view *= wfrontLeg;
    model_view *= Translate(0, -0.5, 0);
    model_view *= Scale(0.3,1,0.3);
    drawCube(texture_skin);
    
    model_view = temp_view;
    model_view *= Translate(-0.3, -0.3, 1.3);
    model_view *= wbackLeg;
    model_view *= Translate(0, -0.5, 0);
    model_view *= Scale(0.3,1,0.3);
    drawCube(texture_skin);
    
    model_view = temp_view;
    model_view *= Translate(-0.3, -0.3, -1.1);
    model_view *= wbackLeg;
    model_view *= Translate(0, -0.5, 0);
    model_view *= Scale(0.3,1,0.3);
    drawCube(texture_skin);
    
    model_view = temp_view;
    model_view *= Translate(0,0,-1.3);
    model_view *= RotateX(-45);
    model_view *= wtail;
    model_view *= Translate(0, 0, -0.5);
    model_view *= Scale(0.5, 0.5, 1);
    drawCube(texture_skin);
    
    model_view = mvstack.top();
    mvstack.pop();
    
    
}


void    updateHunter();
void    updateWolf();
void    updateChicken();
void    drawBackGround();
void    updateCamera();

/*        my variables      */
vec4 Positions[5] = {
    vec4(0,0,10,1),
    vec4(-45,0,-45,1),
    vec4(45,0,45,1),
    vec4(0,0,-20,1),
    vec4(0,0,10,1)
    
};
vec4 tPositions[5] = {
    vec4(0,0,10,1),
    vec4(-45,0,-45,1),
    vec4(45,0,45,1),
    vec4(0,0,-20,1),
    vec4(0,0,10,1)
    
};



mat4 model[3];
mat4 tmodel[3];
double myTimer[5] = {-1};

vec4 trees[20];

mat4 parts[NPARTS];
mat4 tparts[NPARTS];
vec4 eye_vector;

int nTrees = 40;
/*   end my variables   */


void drawBackGround() {

    mat4 temp_view = model_view;
    mvstack.push(model_view);
    // draw Trees
    for(int i = 0; i < 40; i++) {
        for(int j = 0; j < 40; j++) {
            drawTree(g_width/2 - g_width/40*i ,g_height/2 -g_height/40*j);
        }
    }
    
    set_color(0, 0, 1);
    // 300 * 300 ground 
    model_view *= Translate(0,-20,0);
    model_view *= Scale(g_width, 40, g_height);
    drawCube();


    colors.pop();
    model_view = mvstack.top();
    mvstack.pop();
    

}
void updateCamera() {
	
	mvstack.push(model_view);
	
	if(TIME == 0) {
		ref = Positions[H];
		eye = vec4(0,0,15,1);
		eye_vector = eye;
	} else if (TIME < 5) {
		ref = Positions[H];
		eye = parts[H_HEAD] * Translate(0,0,20) * Positions[H];
		eye_vector = eye;
	} else if (TIME < 13){
		ref = Positions[C];
		if(TIME < 7) {
			if(scale(E,5,2) < 0.8) {
				eye = eye_vector + (scale(E,5,2)) * (ref - eye_vector);
			}
			else 
				eye_vector = eye;
		}
		
	} else if (TIME < 15){
		ref = Positions[H];
		eye = parts[H_HEAD] * Translate(0,0,20) * Positions[H];
		eye_vector = eye;

	} else if (TIME < 23){
		ref = Positions[W];
		if(TIME < 17) {
			if(scale(E,5,2) < 0.8) {
				eye = eye_vector + (scale(E,15,2)) * (ref - eye_vector);
			}
			else 
				eye_vector = eye;
		}

		
	} else if (TIME < 33){
		ref = Positions[C];
		if(TIME < 25) {
			if(scale(E,25,2) < 0.8) {
				eye = eye_vector + (scale(E,25,2)) * (ref - eye_vector);
			}
			else 
				eye_vector = eye;
		}

	} else if (TIME < 36){
		ref = Positions[H];
		eye = vec4(0,0,-10,1);
	} else if (TIME < 40){
		ref = Positions[W];
		eye = Positions[H] +0.6 * (Positions[W] - Positions[H]);
	} else if (TIME < 44){
		ref = Positions[C];
		eye = Positions[C] + vec4(20,0,0,0);
	} else if (TIME < 46){
		ref = Positions[C];
		eye = Positions[H];
	} else if (TIME < 48){
		ref = Positions[H];
		eye = Positions[H] + vec4(-5,0,-5,0);	
	} else if (TIME < 50){
		ref = Positions[C];	
	} else if (TIME < 54){
		ref = Positions[H];
		eye = vec4(0,0,-20,1);
	}

	model_view = mvstack.top();
	mvstack.pop();
}


void updateHunter() {

    mvstack.push(model_view);
    
    // initializePosition
    if(TIME == 0) {
        model[H] *= Translate(0,0,10);

        model[H] *= RotateY(180);
        tmodel[H] = model[H];
        
        /* TODO have to raise up with a distance */
        
        myTimer[H] = TIME;
    }
    else if(TIME < 3) {


        vec4 dest(0,0,0,1);
        vec4 v = dest - Positions[H];
        Positions[H] = tPositions[H] + scale(H,0, 3) * v;
        model[H] = Translate(0, 0, -10 * scale(H,0,3)) * tmodel[H];
        parts[H_LEG_L] = tparts[H_LEG_L] * RotateX(30 * sin(360 * 5 * DegreesToRadians * scale(H,0,3)));
        parts[H_LEG_R] = tparts[H_LEG_R] * RotateX(-30 * sin(360 * 5 * DegreesToRadians * scale(H,0,3)));
        parts[H_ARM_L] = tparts[H_ARM_L] * RotateX(-30 * sin(360 * 5 * DegreesToRadians * scale(H,0,3)));
        parts[H_ARM_R] = tparts[H_ARM_R] * RotateX(30 * sin(360 * 5 * DegreesToRadians * scale(H,0,3)));
		
    }
    else if(TIME < 5) {




        parts[H_HEAD] = tparts[H_HEAD] * RotateY(45*sin(180 * DegreesToRadians * scale(H, 3, 5)));
    }
    else if (TIME < 13) {
		tparts[H_HEAD] = parts[H_HEAD];
       // chicken jump  
    }
    else if (TIME < 15) {
        parts[H_HEAD] = tparts[H_HEAD] * RotateY(-90*sin(180 * DegreesToRadians * scale(H, 3, 5)));
    }
    else if(TIME < 23) {
		tparts[H_HEAD] = parts[H_HEAD];	
       // wolf walk around 
    }
    else if(TIME < 33) {
        
    }
    else if(TIME < 35.5) {
		/* TODO might need rotate body */
        parts[H_ARM_L] = tparts[H_ARM_L] * RotateX(-230 * scale(H, 33, 2.5));
    }
    else if(TIME < 36) {
        parts[H_ARM_L] = tparts[H_ARM_L] * RotateX(-230 + 230 * scale(H, 35.5, 0.5));
    }
    else if(TIME < 40) {
        
    }
    else if(TIME < 46) {
        
    }
    else if(TIME < 47.75) {
        parts[H_ARM_L] = tparts[H_ARM_L] * RotateX( -230 * scale(H, 46, 1.75));
    }
    else if(TIME < 48) {
        parts[H_ARM_L] = tparts[H_ARM_L] * RotateX( -230 + 230 * scale(H, 47.75, 0.25));
    }
    else if(TIME < 50) {
        tmodel[H] = model[H];
        tPositions[H] = Positions[H];
    }
    else if(TIME < 54) {
        model[H] = RotateY(180) * tmodel[H];
        model[H] = Translate(0, 0, 20 * scale(H, 50, 4)) * tmodel[H];
        vec4 dest(0,0,-20,1);
        vec4 v = dest - Positions[H];
        Positions[H] = tPositions[H] + scale(H,0, 3) * v;
        parts[H_LEG_L] = tparts[H_LEG_L] * RotateX(30 * sin(360 * 5 * DegreesToRadians * scale(H,50,4)));
        parts[H_LEG_R] = tparts[H_LEG_R] * RotateX(-30 * sin(360 * 5 * DegreesToRadians * scale(H,50,4)));
        parts[H_ARM_L] = tparts[H_ARM_L] * RotateX(-30 * sin(360 * 5 * DegreesToRadians * scale(H,50,4)));
        parts[H_ARM_R] = tparts[H_ARM_R] * RotateX(30 * sin(360 * 5 * DegreesToRadians * scale(H,50,4)));
    }
    else {
        tmodel[H] = model[H];
        
    }
    
    drawHunter(model[H], parts[H_HEAD], parts[H_ARM_L], parts[H_ARM_R], parts[H_LEG_L], parts[H_LEG_R]);
    

    tPositions[H] = Positions[H];
    // walkforward ten unit
    model_view = mvstack.top();
    mvstack.pop();
}

double getDegree(vec4 a, vec4 b) {
    double d = 180 * atan((a.x-b.x)/(a.z-b.z))/PI;
    return d;
}

double getDistance(vec4 a,vec4 b) {
    return sqrt(pow(a.x-b.x,2) + pow(a.y-b.y,2) + pow(a.z-b.y,2));
    
}


vec4 getPosition(mat4 model) {
    
    vec4 p;
    p.x = model[0].w;
    p.y = model[1].w;
    p.z = model[2].w;
    p.w = model[3].w;
    return p;
}


void swingLegs(double start, double t, double distance) {
        parts[W_B_LEG] = tparts[W_B_LEG] * RotateX(-30 * sin(360 * distance * DegreesToRadians * scale(W,start,t)));
        parts[W_F_LEG] = tparts[W_F_LEG] * RotateX(30 * sin(360 * distance * DegreesToRadians * scale(W,start,t)));
    
}
int wolfCounter = 0;
void updateWolf() {
    mvstack.push(model_view);
    
    color c = colors.top();
    set_color(c.r, c.g, c.b);
    
    if(TIME == 0) {
        if(wolfCounter == 0) {
            wolfCounter++;
        }
        model[W] = Translate(50, 0, -50);
        tmodel[W] = model[W];
    }
    else if(TIME < 13) {}
    else if(TIME < 15) {
        if(wolfCounter == 1)
            wolfCounter++;
        model[W] = tmodel[W] * Translate(0, 0, 10 * scale(W, 13,2));
        swingLegs(13,2,10);
    }
    else if(TIME < 17) {
        if(wolfCounter == 2)
            wolfCounter++;
        model[W] = tmodel [W] * Translate(0, 0, 10 - 10*scale(W,15,2)) * RotateY(180);
        swingLegs(15,2,10);
    
    }
    else if(TIME < 19) {
        if(wolfCounter == 2)
            wolfCounter++;
        model[W] = tmodel [W] * Translate(0, 0, -10*scale(W,17,2)) * RotateY(180);
        swingLegs(17,2,10);
    
    }
    else if(TIME < 20) {
        if(wolfCounter == 2)
            wolfCounter++;
        model[W] = tmodel [W] * Translate(0, 0, -10 + 10*scale(W,19,1)) * RotateY(180);
        swingLegs(19,1,10);
    
    }
    else if(TIME < 21) {
        if(wolfCounter == 3)
            wolfCounter++;
        model[W]  = tmodel[W] * RotateY(-90);
        colors.pop();
        set_color(1 * sin(90 * scale(W, 20,1)), 0, 0);
        
    }
    else if(TIME < 22) {
        if(wolfCounter == 4){
            wolfCounter++;
            tmodel[W] = model[W];
        }
        
    }
    else if(TIME < 27) {
        if(wolfCounter == 5)
            wolfCounter++;
        model[W] = tmodel[W] * Translate(0,0,80 * scale(W, 22, 5));
        swingLegs(22,5,80);
    }
    else if(TIME < 36.5) {
        if(wolfCounter == 6)
        {
            tmodel[W] =model[W];
            wolfCounter++;
        }
        model[W] = RotateY(720*scale(H,27, 10)) * tmodel[W];
        swingLegs(27,10,720);
        
        
    }
    
    colors.pop();
    drawWolf(model[W], parts[W_HEAD], parts[W_F_LEG], parts[W_B_LEG], parts[W_TAIL]);
    model_view = mvstack.top();
    mvstack.pop();
}

void swingChickenFeet(double start, double t, double distance) {
        parts[C_LEG_R] = tparts[C_LEG_R] * RotateX(-30 * sin(360 * distance * DegreesToRadians * scale(W,start,t)));
        parts[C_LEG_L] = tparts[C_LEG_L] * RotateX(30 * sin(360 * distance * DegreesToRadians * scale(W,start,t)));
    
}
void swingChickenWing(double start, double t, double distance) {
       parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-65 * cos(distance * 360 * scale(C, start, t)));
       parts[C_WING_R] = tparts[C_WING_R] * RotateZ(65 * cos(distance * 360 * scale(C, start, t)));
}
int chickenTimeCounter =  0;
void updateChicken() {
   
    mvstack.push(model_view);
	color c = colors.top();
	set_color(1, 0.75, 0);
    mat4 tempModel = tmodel[C];
    
    if(TIME == 0) {
        if(chickenTimeCounter == 0)
        { chickenTimeCounter++;}
        // initialize
        model[C] = Translate(-50, 0, -50);
        tmodel[C] = model[C];
    }
    else if (TIME < 5) {
        if(chickenTimeCounter == 1)
        { chickenTimeCounter++;}
        // camera is looking at hunter
        model[C] = tmodel[C] * RotateY(180 * TIME * CHICKEN_SPEED) * Translate(0,0,-5) * RotateY(90);
        swingChickenFeet(0, 5, 360 * 5);
    }
    else if (TIME < 8){
        if(chickenTimeCounter == 2)
        { chickenTimeCounter++;}
        // camera is looking at chicken
        model[C] = tmodel[C] * RotateY(180 * TIME * CHICKEN_SPEED) * Translate(0, 0, -5) * RotateY(90);
        swingChickenFeet(5, 3, 360 * 3);
        
    }
    else if(TIME < 11){
        if(chickenTimeCounter == 3)
        { chickenTimeCounter++;}
        // camera is looking at chicken, raise wing up
        parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 * scale(C, 8, 3));
        parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 * scale(C, 8, 3));
        model[C] = tmodel[C] * RotateY(180 * TIME * CHICKEN_SPEED) * Translate(0, 0, -5) * RotateY(90);
        swingChickenFeet(8, 3, 360 * 3);
     }
    else if(TIME < 13) {
        if(chickenTimeCounter == 4)
        { chickenTimeCounter++;}
        // camera is looking at chicken, jump
        parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 + 135 * scale(C, 11, 2));
        parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 - 135 * scale(C, 11, 2));
        model[C] = tmodel[C] * RotateY(180 * TIME * CHICKEN_SPEED) * Translate(0, 0, -5) * RotateY(90);
        model[C]  *= Translate(0, 4 * sin(180 * DegreesToRadians * scale(C, 11, 2)), 0);
    
    }
    else if(TIME < 25) {
        if(chickenTimeCounter == 5)
        { chickenTimeCounter++;}
        // camera is looking at wolf
        model[C] = tmodel[C] * RotateY(180 * (TIME) * CHICKEN_SPEED) * Translate(0, 0, -5) * RotateY(90);
        swingChickenFeet(13, 3, 360 * 12);
        
    }
    else if(TIME < 38) {
        
        if(chickenTimeCounter == 6 )
        { chickenTimeCounter++;
            Positions[C] = getPosition(model[C]);
            std::cout << "scene 26" << model[C] << std::endl;
        }
        vec4 pc = getPosition(model[C]);
        vec4 dest = getPosition(model[W]);
        vec4 v = dest - pc;
        
        // camera on chicken, chicken face wolf, jump
        // calculate angle
        


        if(TIME < 25.5) {
            if(chickenTimeCounter == 7 )
            { chickenTimeCounter++;}
            model[C] = Translate(pc) * RotateY( atan(v.x/ v.z) * 3.14/180);
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 * scale(C, 25, 0.5));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 * scale(C, 25, 0.5));
            
        }
        else if(TIME < 27) {
            if(chickenTimeCounter == 8 )
            { chickenTimeCounter++;}
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 + 135 * scale(C, 26, 1.5));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 - 135 * scale(C, 26, 1.5));
            model[C] = tmodel[C] * Translate(0, 4 * sin(180 * DegreesToRadians * scale(C, 26, 1.5)), 0);
         //   model[C] *= RotateX(-90 * scale(C, 26, 1));
        }
		/* TODO wolf have to be 5 units away from chicken at time 27 */
        else if(TIME < 37) {
            if(chickenTimeCounter == 9 )
            { chickenTimeCounter++;
           //     model[C] *= RotateX(90);
                tmodel[C] = model[C];

		//		vec4 v = Positions[C] - mat4();
		//		tmodel[C] = RotateY(v.x / v.z);
			}
			
            model[C] = RotateY(getDegree( getPosition(tmodel[C]), getPosition(tmodel[H])) + 180)
                    * RotateY(720 * scale(C, 27,10))
                    * Translate(0,0, getDistance(getPosition(tmodel[C]), getPosition(tmodel[H])));
             model[C] *= RotateY(-90);
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-65 * cos(3600 * scale(C, 27, 10)));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(65 * cos(3600 * scale(C, 27, 10)));
            swingChickenFeet(27,10, 1800);
		}
    }
    else if(TIME < 38) {}
    else if(TIME < 40) {
        model[C] = tmodel[C];
        if(TIME < 39) {
            if(chickenTimeCounter == 10 )
            { chickenTimeCounter++;
				tmodel[C] = model[C];
			}
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 * scale(C, 38, 1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 * scale(C, 38, 1));
            
        }
        else if(TIME < 40) {
            if(chickenTimeCounter == 11 )
            { chickenTimeCounter++;}
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 + 135 * scale(C, 39, 1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 - 135 * scale(C, 39, 1));
            model[C] = tmodel[C] * Translate(0, 4 * sin(180 * DegreesToRadians * scale(C, 39, 1)), 0);
        }
    }
	else if(TIME < 44){
        if(TIME < 41) {
            if(chickenTimeCounter == 12 )
            { chickenTimeCounter++;}
            
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 * scale(C, 40, 1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 * scale(C, 40, 1));
            
        }
        else if(TIME < 42) {
            if(chickenTimeCounter == 13 )
            { chickenTimeCounter++;}
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 + 135 * scale(C, 41, 1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 - 135 * scale(C, 41, 1));
            model[C] = tmodel[C] * Translate(0, 4 * sin(180 * DegreesToRadians * scale(C, 41, 1)), 0);
        }
        if(TIME < 43) {
            if(chickenTimeCounter == 14 )
            { chickenTimeCounter++;}
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 * scale(C, 42,1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 * scale(C,42,1));
            
        }
        else if(TIME < 44) {
            if(chickenTimeCounter == 15 )
            { chickenTimeCounter++;}
            parts[C_WING_L] = RotateZ(-135 + 135 * scale(C, 43, 1));
            parts[C_WING_R] = RotateZ(135 - 135 * scale(C, 43, 1));
            model[C] = tmodel[C] * Translate(0, 4 * sin(180 * DegreesToRadians * scale(C, 43, 1)), 0);

        }
	}

    else if(TIME < 49) {
        if(chickenTimeCounter == 16 )
        { chickenTimeCounter++;
            tmodel[C] = RotateY(getDegree(getPosition(model[C]), getPosition(model[H]))) * Translate(0,0,-1* getDistance(getPosition(model[C]), getPosition(model[H])));
        }
        model[C] = tmodel[C] * Translate(0,0,scale(C,44,5) * 40);

        //model[C] *= Translate(0,0, scale(C,44,5) * 50 );
    }
	else if(TIME < 50) {
        if(chickenTimeCounter == 17 )
        {
            chickenTimeCounter++;
            tmodel[C] = model[C];
        }
		model[C] = tmodel[C] * RotateZ(90 * scale(C,49,1));
	}
	else if(TIME < 52) {
        if(chickenTimeCounter == 18 )
        { chickenTimeCounter++;
        }
        colors.pop();
        set_color(1,0.75 - 0.75 * sin(90 * DegreesToRadians * scale(C,50,2)),0);
	}
	else {
        colors.pop();
        set_color(1,0,0);
	}
    
    placeChicken(model[C], parts[C_LEG_L], parts[C_LEG_R], parts[C_WING_L], parts[C_WING_R]);


	colors.pop();
	model_view = mvstack.top();
    mvstack.pop();
	 
}



void display(void) {
    
	basis_id = 0;
    glClearColor( .1, .1, .2, 1 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    set_color(1, 1, 1);
	model_view = LookAt( eye, ref, up );
   
	model_view *= orientation;
  model_view *= Scale(zoom);												drawAxes(basis_id++);

    drawTree(0, 0);
    //drawWolf(model[W], parts[W_HEAD], parts[W_F_LEG], parts[W_B_LEG], parts[W_TAIL]);
    updateHunter();
    updateWolf();
    updateChicken();
    //drawBackGround();
    // updateCamera();


    std::cout << TIME << std::endl;
    glutSwapBuffers();
    

}

void myReshape(int w, int h)	// Handles window sizing and resizing.
{
    mat4 projection = Perspective( 50, (float)w/h, 1, 1000 );
    glUniformMatrix4fv( uProjection, 1, GL_FALSE, transpose(projection) );
	glViewport(0, 0, g_width = w, g_height = h);	
}		

void instructions() {	 std::cout <<	"Press:a"									<< '\n' <<
										"  r to restore the original view."			<< '\n' <<
										"  0 to restore the original state."		<< '\n' <<
										"  a to toggle the animation."				<< '\n' <<
										"  b to show the next basis's axes."		<< '\n' <<
										"  B to show the previous basis's axes."	<< '\n' <<
										"  q to quit."								<< '\n';	}

void myKey(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':   case 27:				// 27 = esc key
            exit(0); 
		case 'b':
			std::cout << "Basis: " << ++basis_to_display << '\n';
			break;
		case 'B':
			std::cout << "Basis: " << --basis_to_display << '\n';
			break;
        case 'a':							// toggle animation
            
            if(animate) std::cout << "Elapsed time " << TIME << std::endl;
            
            animate = 1 - animate ;
            if(!animate) pause_time = TIME;
            TM.Reset();
            break ;
		case '0':							// Add code to reset your object here.
			TIME = 0;	TM.Reset() ;											
        case 'r':
			orientation = mat4();			
            break ;
    }
    glutPostRedisplay() ;
}

int main() 
{
	char title[] = "Title";
	int argcount = 1;	 char* title_ptr = title;
	glutInit(&argcount,		 &title_ptr);
	glutInitWindowPosition (230, 70);
	glutInitWindowSize     (g_width, g_height);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow(title);
	#if !defined(__APPLE__) && !defined(EMSCRIPTEN)
		glewExperimental = GL_TRUE;
		glewInit();
	#endif

    std::cout << "GL version " << glGetString(GL_VERSION) << '\n';
	instructions();
	init();

	glutDisplayFunc(display);
    glutIdleFunc(idleCallBack) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutMouseFunc(myMouseCallBack) ;
    glutMotionFunc(myMotionCallBack) ;
    glutPassiveMotionFunc(myPassiveMotionCallBack) ;

	glutMainLoop();
}
