// anim.cpp version 5.0 -- Template code for drawing an articulated figure.  CS 174A.
#include "../CS174a template/Utilities.h"
#include "../CS174a template/Shapes.h"
#include "myConstant.h"


std::stack<mat4> mvstack;

int g_width = 800, g_height = 800 ;
float zoom = 1 ;

int animate = 0, recording = 0, basis_to_display = -1;
double TIME = 0;

const unsigned X = 0, Y = 1, Z = 2;

vec4 eye( 0, 150, -20, 1), ref( 0 , 0, -20 ), up( 0, 1, 0, 0 );	// The eye point and look-at point.

mat4	orientation, model_view;
ShapeData chickenData, cubeData, sphereData, coneData, cylData;				// Structs that hold the Vertex Array Object index and number of vertices of each shape.
GLuint	texture_cube, texture_earth;
GLint   uModelView, uProjection, uView,
		uAmbient, uDiffuse, uSpecular, uLightPos, uShininess,
		uTex, uEnableTex;

void init()
{
#ifdef EMSCRIPTEN
    GLuint program = LoadShaders( "vshader.glsl", "fshader.glsl" );								// Load shaders and use the resulting shader program
    TgaImage coolImage ("challenge.tga");    
    TgaImage earthImage("earth.tga");

#else
	GLuint program = LoadShaders( "../my code/vshader.glsl", "../my code/fshader.glsl" );		// Load shaders and use the resulting shader program
    TgaImage coolImage ("../my code/challenge.tga");    
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
    
    glGenTextures( 1, &texture_cube );
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, coolImage.width, coolImage.height, 0,
                 (coolImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, coolImage.data );
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
double hold_time = 0;
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


mat4 chickenPos = Translate(120, 6, -40);


void drawChicken() {
    
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( chickenData.vao );
    glDrawArrays( GL_TRIANGLES, 0, chickenData.numVertices );
    
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

void drawCube()		// draw a cube with dimensions 1,1,1 centered around the origin.
{
	glBindTexture( GL_TEXTURE_2D, texture_cube );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
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
	model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
}

void drawShapes()
{
	mvstack.push(model_view);

    model_view *= Translate	( 0, 3, 0 );									drawAxes(basis_id++);
    model_view *= Scale		( 3, 3, 3 );									drawAxes(basis_id++);
    set_color( .8, .0, .8 );
    drawCube();

    model_view *= Scale		( 1/3.0f, 1/3.0f, 1/3.0f );						drawAxes(basis_id++);
    model_view *= Translate	( 0, 3, 0 );									drawAxes(basis_id++);
    set_color( 0, 1, 0 );
    drawCone();

    model_view *= Translate	( 0, -9, 0 );									drawAxes(basis_id++);
    set_color( 1, 1, 0 );
    drawCylinder();

	model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
	
    model_view *= Scale		( 1/3.0f, 1/3.0f, 1/3.0f );						drawAxes(basis_id++);

	drawGround();
}

void drawPlanets()
{
    set_color( .8, .0, .0 );	//model sun
    mvstack.push(model_view);
    model_view *= Scale(3);													drawAxes(basis_id++);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
    
    set_color( .0, .0, .8 );	//model earth
    model_view *= RotateY	( 10*TIME );									drawAxes(basis_id++);
    model_view *= Translate	( 15, 5*sin( 30*DegreesToRadians*TIME ), 0 );	drawAxes(basis_id++);
    mvstack.push(model_view);
    model_view *= RotateY( 300*TIME );										drawAxes(basis_id++);
    drawCube();
    model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
    
    set_color( .8, .0, .8 );	//model moon
    model_view *= RotateY	( 30*TIME );									drawAxes(basis_id++);
    model_view *= Translate	( 2, 0, 0);										drawAxes(basis_id++);
    model_view *= Scale(0.2);												drawAxes(basis_id++);
    drawCylinder();
	
}

void drawMidterm()
{
	mvstack.push(model_view);
	mvstack.push(model_view);
	model_view *= Translate	( -1, 0, 0 );									drawAxes(basis_id++);
	model_view *= Scale		( 2, 1, 1 );									drawAxes(basis_id++);
	drawCube();
	model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
	
	model_view *= Scale		( 2, 1, 1 );									drawAxes(basis_id++);
	model_view *= Translate	( 1, 0, 0 );									drawAxes(basis_id++);
	drawCube();

	
	model_view *= Translate	( 0, 2, 0 );									drawAxes(basis_id++);
	model_view *= RotateZ	( 90 + 360 * TIME );							drawAxes(basis_id++);
	drawCube();
	model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
}

mat4 leftFood,rightFoot,cLeftWing, cRightWing, cBody;
void placeChicken(mat4 cBody,
                  mat4 leftFoot,
                  mat4 rightFoot,
                  mat4 cLeftWing,
                  mat4 cRightWing) {
    mvstack.push(model_view);
    model_view *= cBody;
    mat4 temp_view = model_view;
    
    drawChicken();
   
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
    
    model_view = mvstack.top();
    mvstack.pop();
    
    
    
}

void drawTree(double x, double z) {
    mvstack.push(model_view);
     set_color(0, 1, 0);
    model_view *=Translate(x, 0, z);
    model_view *= Translate(0, 1.5, 0);
    model_view *= Scale(2.5, 3, 2.5);
    drawCube();
    
    set_color(1, 0, 0);
//    model_view *= Scale(1/2, 1/3, 1/2);
    model_view *= Translate(0,0.5,0);
    model_view *= Translate(0, 1, 0);
    model_view *= Scale(3, 2, 3);
    drawCube();
    
    model_view = mvstack.top();
    mvstack.pop();
    
}

point4 bodyPos;

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
    model_view *= Translate(-0.50, -1.5, 0);
    model_view *= aLeftLeg;
    model_view *= Translate(0,-1.5, 0);
    model_view *= Scale(1,3,1);
    drawCube();

    // draw leftLeg
    model_view = temp_view;
    model_view *= Translate(0.50, -1.5, 0);
    model_view *= aRightLeg;
    model_view *= Translate(0,-1.5, 0);
    model_view *= Scale(1,3,1);
    drawCube();
    
    model_view = mvstack.top();
    mvstack.pop();
    
    
    
    
}


//************ walk function
mat4 aBody;// TODO initialize position
mat4 aLeftLeg;
mat4 aRightLeg;
mat4 aLeftArm;
mat4 aRightArm;
mat4 aHead;


double myTimeStamp[5] = {-1};
int currentDegree = 0;
mat4 currentPosition;

bool walk(double degree, double distance, double timeCounter = 0) {
    std::cout << TIME-myTimeStamp[HUNTER_WALK_STAMP]<<std::endl;
    if(timeCounter == 0) {
        // stand position
        return false;
    }
    else {
        currentDegree = currentDegree + (degree-currentDegree) / 2;
        // if hunter is not moving, init timer
        if(myTimeStamp[HUNTER_WALK_STAMP] == -1) {
            myTimeStamp[HUNTER_WALK_STAMP] = TIME;
            aBody *= RotateY(degree);
            
            currentPosition = aBody;
            return false;
        }
        
        else if(myTimeStamp[HUNTER_WALK_STAMP]+timeCounter >= TIME) {
            std::cout << "t/my_t=" << TIME/(myTimeStamp[HUNTER_WALK_STAMP] + 1) << std::endl;
            
            
            aBody = currentPosition * Translate(0,0, (TIME - myTimeStamp[HUNTER_WALK_STAMP])/timeCounter * distance );
            aLeftArm = RotateX(30 * sin(TIME*DegreesToRadians*100));
            aRightArm = RotateX( 30 * sin(TIME*DegreesToRadians * -100));
            aLeftLeg = RotateX(30 * sin(TIME * DegreesToRadians * -100));
            aRightLeg = RotateX(30 *sin(TIME*DegreesToRadians * 100));
            
          
            
            return false;
        }
        else{
            myTimeStamp[HUNTER_WALK_STAMP] = -1;
            aLeftArm = mat4();
            aRightArm = mat4();
            aLeftLeg = mat4();
            aRightLeg = mat4();
            return true;
        }
    }
    
}
double degree = 0, distance = 0, timeCounter = 0;
int scene = 0;
mat4 camera;

bool scene_one();
bool scene_two();

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

mat4 parts[9];
mat4 tparts[9];
vec4 eye_vector;

int nTrees = 40;
/*   end my variables   */


void drawBackGround() {

    mat4 temp_view = model_view;
    mvstack.push(model_view);
    // 300 * 300 ground 
    model_view *= Translate(0,-20,0);
    model_view *= Scale(g_width, 40, g_height);
    drawCube();

    // draw Trees
    for(int i = 0; i < 20; i++) {
		drawTree(g_width/2 - g_width/20*i ,g_height/2 -g_height/20*i);
    } 
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

#ifdef DEBUG_MODE
	 double timeC = 0;
#endif
void updateHunter() {

    mvstack.push(model_view);
    
    // initializePosition
    if(TIME == 0) {
#ifdef DEBUG_MODE
	if(timeC == 0) {
		timeC = 3;
	}
#endif
        model[H] *= Translate(0, 0, 10);
        model[H] *= RotateY(180);
        tmodel[H] = model[H];
        
        /* TODO have to raise up with a distance */
        
        myTimer[H] = TIME;
    }
    else if(TIME < 3) {

#ifdef DEBUG_MODE
	if(timeC == 3) {
		timeC =5;
        std::cout << "moving forward" << std::endl;

	}
#endif
        vec4 dest(0,0,0,1);
        vec4 v = dest - Positions[H];
        Positions[H] = tPositions[H] + scale(H,0, 3) * v;
        model[H] = Translate(0, 0, -10 * scale(H,0,3)) * tmodel[H];
        /* TODO swing arm and legs
            aLeftArm = RotateX(30 * sin(TIME*DegreesToRadians*100));
            aRightArm = RotateX( 30 * sin(TIME*DegreesToRadians * -100));
            aLeftLeg = RotateX(30 * sin(TIME * DegreesToRadians * -100));
            aRightLeg = RotateX(30 *sin(TIME*DegreesToRadians * 100));
         */
		
    }
    else if(TIME < 5) {
#ifdef DEBUG_MODE
	if(timeC == 5) {
		timeC++;
        std::cout << "rotating left" << std::endl;
        tmodel[H] = model[H];
        tPositions[H] = Positions[H];
	}
#endif



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

vec4 getPosition(mat4 model) {
    
    vec4 p;
    p.x = model[0].w;
    p.y = model[1].w;
    p.z = model[2].w;
    p.w = model[3].w;
    return p;
}
int chickenTimeCounter =  0;
void updateChicken() {
   
    mvstack.push(model_view);
	color c = colors.top();
	set_color(1, 0.75, 0);
    mat4 tempModel = tmodel[C];
    
    if(TIME == 0) {
        if(chickenTimeCounter == 0)
        { chickenTimeCounter = 5;}
        // initialize
        model[C] = Translate(-50, 0, -50);
        tmodel[C] = model[C];
    }
    else if (TIME < 5) {
        if(chickenTimeCounter == 5)
        {chickenTimeCounter = 8;}
        // camera is looking at hunter
        model[C] = tmodel[C] * RotateY(180 * TIME * CHICKEN_SPEED) * Translate(0,0,-5) * RotateY(90);
    }
    else if (TIME < 8){
        if(chickenTimeCounter == 8)
        {chickenTimeCounter = 11;}
        // camera is looking at chicken
        model[C] = tmodel[C] * RotateY(180 * TIME * CHICKEN_SPEED) * Translate(0, 0, -5) * RotateY(90);
        
    }
    else if(TIME < 11){
        if(chickenTimeCounter == 11)
        {chickenTimeCounter = 13;}
        // camera is looking at chicken, raise wing up
        parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 * scale(C, 8, 3));
        parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 * scale(C, 8, 3));
        model[C] = tmodel[C] * RotateY(180 * TIME * CHICKEN_SPEED) * Translate(0, 0, -5) * RotateY(90);
     }
    else if(TIME < 13) {
        if(chickenTimeCounter == 13)
        {chickenTimeCounter = 25;}
        // camera is looking at chicken, jump
        parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 + 135 * scale(C, 11, 2));
        parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 - 135 * scale(C, 11, 2));
        model[C] = tmodel[C] * RotateY(180 * TIME * CHICKEN_SPEED) * Translate(0, 0, -5) * RotateY(90);
        model[C]  *= Translate(0, 4 * sin(180 * DegreesToRadians * scale(C, 11, 2)), 0);
    
    }
    else if(TIME < 25) {
        if(chickenTimeCounter == 25)
        {chickenTimeCounter = 40;}
        // camera is looking at wolf
        model[C] = tmodel[C] * RotateY(180 * (TIME) * CHICKEN_SPEED) * Translate(0, 0, -5) * RotateY(90);
        
    }
    else if(TIME < 38) {
        
        if(chickenTimeCounter == 40 )
        {
            chickenTimeCounter = 26;
            Positions[C] = getPosition(model[C]);
            std::cout << "scene 26" << model[C] << std::endl;
        }
        vec4 pc = Positions[C];
        vec4 dest = getPosition(model[W]);
        vec4 v = dest - pc;
        
        // camera on chicken, chicken face wolf, jump
        // calculate angle
        
        model[C] = tmodel[C] * Translate(pc) * RotateY( atan(v.x/ v.z));

        if(TIME < 26) {
			if(chickenTimeCounter == 26)
				chickenTimeCounter = 27;
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 * scale(C, 25, 1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 * scale(C, 25, 1));
            
        }
        else if(TIME < 27) {
			if(chickenTimeCounter == 27)
				chickenTimeCounter = 38;
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 + 135 * scale(C, 26, 1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 - 135 * scale(C, 26, 1));
            model[C] = tmodel[C] * Translate(0, 4 * sin(180 * DegreesToRadians * scale(C, 26, 1)), 0);
            model[C] *= RotateX(-90 * scale(C, 26, 1));
        }
		/* TODO wolf have to be 5 units away from chicken at time 27 */
        else if(TIME < 38) {
			if(chickenTimeCounter ==38) {
                std::cout << "scene 38" << std::endl;
				chickenTimeCounter = 39;
				vec4 v = Positions[C] - mat4();
				tmodel[C] = RotateY(v.x / v.z);
			}
			
			model[C] = tmodel[C] * RotateY(180 * (TIME-27));
			model[C] *= Translate(0,0,63.6);
            model[C] *= RotateY(-90);
		}
    }
    else if(TIME < 40) {
        model[C] = tmodel[C];
        if(TIME < 39) {
			if(chickenTimeCounter ==39) {
				chickenTimeCounter = 40;
				tmodel[C] = model[C];
			}
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 * scale(C, 38, 1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 * scale(C, 38, 1));
            
        }
        else if(TIME < 40) {
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 + 135 * scale(C, 39, 1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 - 135 * scale(C, 39, 1));
            model[C] = tmodel[C] * Translate(0, 4 * sin(180 * DegreesToRadians * scale(C, 39, 1)), 0);
        }
    }
	else if(TIME < 42){
        if(TIME < 41) {
            
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 * scale(C, 40, 1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 * scale(C, 40, 1));
            
        }
        else if(TIME < 42) {
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 + 135 * scale(C, 41, 1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 - 135 * scale(C, 41, 1));
            model[C] = tmodel[C] * Translate(0, 4 * sin(180 * DegreesToRadians * scale(C, 41, 1)), 0);
        }
        if(TIME < 43) {
            parts[C_WING_L] = tparts[C_WING_L] * RotateZ(-135 * scale(C, 42,1));
            parts[C_WING_R] = tparts[C_WING_R] * RotateZ(135 * scale(C,42,1));
            
        }
        else if(TIME < 44) {
            parts[C_WING_L] = RotateZ(-135 + 135 * scale(C, 43, 1));
            parts[C_WING_R] = RotateZ(135 - 135 * scale(C, 43, 1));
            model[C] = tmodel[C] * Translate(0, 4 * sin(180 * DegreesToRadians * scale(C, 43, 1)), 0);

        }
	}
	else if(TIME < 49) {
        tmodel[C] = Translate(-45, 0, -45);
		model[C] = tmodel[C] * RotateY(45);
		model[C] = model[C] * Translate(0,0,scale(C,44,5) *50);
	}
	else if(TIME < 50) {
		model[C] = RotateZ(90 * scale(C,49,1));
	}
	else if(TIME < 52) {
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

void updateWolf() {

}


void display(void) {
    
	basis_id = 0;
    glClearColor( .1, .1, .2, 1 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	set_color( .6, .6, .6 );
    
	model_view = LookAt( eye, ref, up );
   

    updateHunter();
    //updateWolf();
 //   placeChicken(model[C], parts[C_LEG_L], parts[C_LEG_R], parts[C_WING_L], parts[C_WING_R]);
    updateChicken();
    // drawBackGround();
    // updateCamera();

	model_view *= orientation;
    model_view *= Scale(zoom);												drawAxes(basis_id++);
    
    glutSwapBuffers();
    

}
/*
void display(void)
{
	basis_id = 0;
    glClearColor( .1, .1, .2, 1 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	set_color( .6, .6, .6 );
    
   
	model_view = LookAt( eye, ref, up );

	model_view *= orientation;
    model_view *= Scale(zoom);												drawAxes(basis_id++);
   // drawCylinder();
    //drawChicken();
    
    if(animate == false) {
        TIME = 0;
        
    }
    if(scene == 0 && animate == false) {
    
        degree = 90;
        distance = 50;
        timeCounter = 2;
    }
    if(scene == 0 && walk(degree, distance,timeCounter) == true) {
        myTimeStamp[DEFAULT_STAMP] = TIME;
        timeCounter = 0;
        scene++;
        
    }
    else if(scene == 0){
        camera = aBody;
        ref = aBody * vec4(0, 0, 0, 1);
        eye = aBody * Translate(0,40, -60) * vec4(0,0,0,1);
    }
    
    // scene 1
    
    if(scene == 1) {
        if(scene_one())
        {
            scene++;
            myTimeStamp[DEFAULT_STAMP] = TIME;
        }
    } else if(scene == 2) {
        if(scene_two()) {
            scene++;
            myTimeStamp[DEFAULT_STAMP] = TIME;
        }
        
    }
    
    
    
    mvstack.push(model_view);
    
    drawTree(20,-40);
    drawTree(10, -35);
    drawTree(35, -70);
    drawTree(40, -10);
    drawTree(35, -30);
    model_view = mvstack.top();
    mvstack.pop();
    
    
    drawHunter(aBody, aHead, aLeftArm, aRightArm, aLeftLeg, aRightLeg);
   
    mvstack.push(model_view);
    
    cBody = Scale(0.5, 0.5, 0.5);
    placeChicken(cBody, mat4(), mat4(), mat4(), mat4());
   
    model_view = mvstack.top();
    mvstack.pop();
    
    glutSwapBuffers();
}
*/
bool scene_one() {
    mat4 tempAHead;
    std::cout << "scene one:" << TIME << std::endl;
    if(TIME < myTimeStamp[DEFAULT_STAMP] + 4) {
      //  eye = camera * RotateY(180) * RotateY((myTimeStamp[DEFAULT_STAMP]+ 4) * 180) * Scale(1.5) * vec4(0,0,0,1);
        eye =  camera * RotateY(180) * RotateY(TIME/(myTimeStamp[DEFAULT_STAMP]+4) * 180) * Translate(0, 10, 70-(TIME-myTimeStamp[DEFAULT_STAMP])/4 * 50) * vec4(0,0,0,1);
        return false;
    }
    else if (TIME < (myTimeStamp[DEFAULT_STAMP] + 5)) {
        tempAHead = aHead;
        return false;
    }
    else if (TIME < (myTimeStamp[DEFAULT_STAMP] + 14)) {
        ref = aBody * vec4(0,0,0,1);
        aHead = RotateY(-30 * sin((TIME-myTimeStamp[DEFAULT_STAMP]-5) * DegreesToRadians * 30)) * tempAHead;
        return false;
        
    }
    return true;
}
vec4 dir;
bool zoomIn(double speed,double scale) {
    if( TIME < myTimeStamp[ZOOM_STAMP] + speed) {
        eye = ref + (myTimeStamp[ZOOM_STAMP]-TIME + speed)/(speed*(1+scale)) * dir;
            std::cout << "v= " << dir << "  zoom in scale: " << (myTimeStamp[ZOOM_STAMP]+speed-TIME)/(speed*(1+scale)) << std::endl;
        return false;
    }
    return true;
}

bool scene_two() {
    if(TIME < myTimeStamp[DEFAULT_STAMP] + 3) {
        eye = camera * RotateY(TIME/(myTimeStamp[DEFAULT_STAMP]+3) * 210) * Translate(0,10,70 )* vec4(0,0,0,1);
    
    }
    else if (TIME < myTimeStamp[DEFAULT_STAMP] + 7) {
        ref = chickenPos * vec4(0,0,0,1);
        if(myTimeStamp[ZOOM_STAMP] == -1) {
            myTimeStamp[ZOOM_STAMP] = TIME;
            dir = eye-ref;
        }
        if(zoomIn(4, 0.4))
            myTimeStamp[ZOOM_STAMP] = -1;
        return true;
    }
    return false;
}

bool cJump(){
    if(TIME < myTimeStamp[DEFAULT_STAMP] + 2) {
        cLeftWing *= RotateZ(-90*(TIME - myTimeStamp[DEFAULT_STAMP])/ 2);
        cRightWing *= RotateZ(90*(TIME - myTimeStamp[DEFAULT_STAMP])/ 2);
        return false;
    }
    return true;
    
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
