#pragma once
#include "Utilities.h"

#define N_UP point3(0,1,0)
#define N_FRONT point3(0,0,1)
#define N_BACK point3(0,0,-1)
#define N_DOWN point3(0,-1,0)
#define N_LEFT point3(-1,0,0)
#define N_RIGHT point3(1,0,0)


struct ShapeData	{	GLuint vao;
						int numVertices;	};

void generateCube(GLuint program, ShapeData* cubeData);
void generateSphere(GLuint program, ShapeData* sphereData);
void generateCone(GLuint program, ShapeData* coneData);
void generateCylinder(GLuint program, ShapeData* cylData);


// Chichen
const int numChickenVertices = 60;
point4 chickenPoints   [numChickenVertices];
point3 chickenNormals   [numChickenVertices];
point2 chickenUV        [numChickenVertices];


point4 chickenVertices[12] = {

    point4(0,5,0,1),
    point4(2,5,0,1),
    point4(0,0,0,1),
    point4(2,0,0,1),
    point4(0,5,-3,1),
    point4(2,5,-3,1),
    point4(0,2,-3,1),
    point4(2,2,-3,1),
    point4(0,2,-4,1),
    point4(2,2,-4,1),
    point4(0,0,-4,1),
    point4(2,0,-4,1)

};

point2 uvVertices[22] = {
    
    /*
    point2(0,0),
    point2(0,0),
    point2(0,0),
    point2(0,0),
    
   */
    point2(5/13.0,10/13.0),
    point2(7/13.0,10/13.0),
    point2(0/13.0,7/13.0),
    point2(2/13.0,7/13.0),
     
    point2(5/13.0,7/13.0),
    point2(7/13.0,7/13.0),
    
    /*
    point2(0,0),
    point2(0,0),
    point2(0,0),
    point2(0,0),
    point2(0,0),
    point2(0,0),
    point2(0,0),
    */
   

    point2(10/13.0,7/13.0),
    point2(1/13.0,4/13.0),
    point2(2/13.0,4/13.0),
    point2(10/13.0,4/13.0),
    point2(11/13.0,4/13.0),
    point2(13/13.0,4/13.0),
    point2(1/13.0,2/13.0),
    
    point2(5/13.0,2/13.0),
    point2(7/13.0,2/13.0),
    
    /*
    point2(0,0),
    point2(0,0),
    point2(0,0),
    point2(0,0),
    point2(0,0),
    point2(0,0),
    point2(0,0),
     */

    point2(11/13.0,2/13.0),
    point2(13/13.0,2/13.0),
    point2(1/13.0,0/13.0),
    point2(5/13.0,0/13.0),
    point2(10/13.0,6/13.0),
    point2(11/13.0,6/13.0),
    point2(0/13.0,4/13.0)
 
};

int chickenIndex = 0;


void drawTriangle(int a, int b, int c, int uv_a, int uv_b, int uv_c, point3 normal) {

    chickenPoints[chickenIndex] = chickenVertices[a];
    chickenNormals[chickenIndex] = normal;
    chickenUV[chickenIndex] = uvVertices[uv_a];
    chickenIndex++;
    std::cout << chickenIndex<< std::endl;
    std::cout << chickenVertices[a] << std:: endl << std:: endl;
    
    chickenPoints[chickenIndex] = chickenVertices[b];
    chickenNormals[chickenIndex] = normal;
    chickenUV[chickenIndex] = uvVertices[uv_b];
    chickenIndex++;
    std::cout << chickenIndex << std::endl;
    std::cout << chickenVertices[b] << std:: endl << std:: endl;

    chickenPoints[chickenIndex] = chickenVertices[c];
    chickenNormals[chickenIndex] = normal;
    chickenUV[chickenIndex] = uvVertices[uv_c];
    chickenIndex++;
    std::cout << chickenIndex << std::endl;
    std::cout << chickenVertices[c] << std:: endl << std:: endl;
    std::cout << std::endl << std::endl;

}
void colorChicken() {
    //drawTop A
    drawTriangle(0,1,4,4,5,0,N_UP);
    drawTriangle(1,4,5,5,0,1,N_UP);
    
    //drawBack A
    drawTriangle(4,5,7,3,2,21,N_BACK);
    drawTriangle(4,6,7,3,8,21,N_BACK);
        //front
    drawTriangle(0,1,2,4,5,13,N_FRONT);
    drawTriangle(1,2,3,5,13,14,N_FRONT);


    //drawTop B
    drawTriangle(7,9,8,9,10,20,N_UP);
    drawTriangle(6,8,7,19,20,9,N_UP);
    
    //draBack B
    drawTriangle(9,8,10,10,11,16,N_BACK);
    drawTriangle(9,11,10,10,15,16,N_BACK);
    
    //drawBottom
    drawTriangle(2,10,11,13,12,17,N_DOWN);
    drawTriangle(2,3,11,13,18,17,N_DOWN);


    

    drawTriangle(3,1,5,14,5,6,N_RIGHT);
    drawTriangle(3,7,5,14,9,6,N_RIGHT);
    drawTriangle(3,7,9,14,9,10,N_RIGHT);
    drawTriangle(3,11,9,14,15,10,N_RIGHT);
    
    //drawLeft
    drawTriangle(2,0,4,13,4,3,N_LEFT);
    drawTriangle(2,6,4,13,8,3,N_LEFT);
    drawTriangle(2,6,8,13,8,7,N_LEFT);
    drawTriangle(2,10,8,13,12,7,N_LEFT);
    



}

void generateChicken(GLuint program, ShapeData* chickenData)
{
    colorChicken();
    chickenData->numVertices= numChickenVertices;
    
    // Create a vertex array object
    glGenVertexArrays( 1, &chickenData->vao );
    glBindVertexArray( chickenData->vao );
    
    // Set vertex attributes
    setVertexAttrib(program,
                    (float*)chickenPoints,  sizeof(chickenPoints),
                    (float*)chickenNormals, sizeof(chickenNormals),
                    (float*)chickenUV,      sizeof(chickenUV));
}



//----------------------------------------------------------------------------
// Cube

const int numCubeVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 cubePoints [numCubeVertices];
point3 cubeNormals[numCubeVertices];
point2 cubeUV     [numCubeVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1 ),
    point4( -0.5,  0.5,  0.5, 1 ),
    point4(  0.5,  0.5,  0.5, 1 ),
    point4(  0.5, -0.5,  0.5, 1 ),
    point4( -0.5, -0.5, -0.5, 1 ),
    point4( -0.5,  0.5, -0.5, 1 ),
    point4(  0.5,  0.5, -0.5, 1 ),
    point4(  0.5, -0.5, -0.5, 1 )
};

point2 uvvertices[14] = {
    point2(0.5,0),
    point2(0.75,0),
    point2(0.25,0.25),
    point2(0.5,0.25),
    point2(0.75,0.25),
    point2(1,0.25),
    point2(0.25,0.5),
    point2(0.5,0.5),
    point2(0.75,0.5),
    point2(1,0.5),
    point2(0.5,0.75),
    point2(0.75,0.75),
    point2(0.5,1),
    point2(0.75,1)

};

// quad generates two triangles for each face and assigns normals and texture coordinates
//    to the vertices
int Index = 0;
void quad( int a, int b, int c, int d, const point3& normal, int ua, int ub, int uc, int ud )
{
    cubePoints[Index] = vertices[a]; cubeNormals[Index] = normal; 
    cubeUV[Index] = uvvertices[ua]; Index++;
    cubePoints[Index] = vertices[b]; cubeNormals[Index] = normal;
    cubeUV[Index] = uvvertices[ub]; Index++;
    cubePoints[Index] = vertices[c]; cubeNormals[Index] = normal;
    cubeUV[Index] = uvvertices[uc]; Index++;
    cubePoints[Index] = vertices[a]; cubeNormals[Index] = normal;
    cubeUV[Index] = uvvertices[ua]; Index++;
    cubePoints[Index] = vertices[c]; cubeNormals[Index] = normal;
    cubeUV[Index] = uvvertices[uc]; Index++;
    cubePoints[Index] = vertices[d]; cubeNormals[Index] = normal;
    cubeUV[Index] = uvvertices[ud]; Index++;
}

// generate 12 triangles: 36 vertices, 36 normals, 36 texture coordinates
void colorcube()
{
    quad( 1, 0, 3, 2, point3( 0,  0,  1), 3,0,1,4);  //uv = 0.5,
    quad( 2, 3, 7, 6, point3( 1,  0,  0), 4,5,9,8);
    quad( 3, 0, 4, 7, point3( 0, -1,  0), 13,12,10,11);
    quad( 6, 5, 1, 2, point3( 0,  1,  0), 8,7,3,4);
    quad( 4, 5, 6, 7, point3( 0,  0, -1),10,7,8,11 );
    quad( 5, 4, 0, 1, point3(-1,  0,  0), 6,7,3,2 );
}

// initialization
void generateCube(GLuint program, ShapeData* cubeData)
{
    colorcube();
    cubeData->numVertices = numCubeVertices;

    // Create a vertex array object
    glGenVertexArrays( 1, &cubeData->vao );
    glBindVertexArray( cubeData->vao );

    // Set vertex attributes
    setVertexAttrib(program, 
        (float*)cubePoints,  sizeof(cubePoints), 
        (float*)cubeNormals, sizeof(cubeNormals),
        (float*)cubeUV,      sizeof(cubeUV));
}


//----------------------------------------------------------------------------
// Sphere approximation by recursive subdivision of a tetrahedron

const int N = 5;  // number of subdivisions
const int numSphereVertices = 16*256*3;  // number of resulting points

point4 spherePoints [numSphereVertices];
point3 sphereNormals[numSphereVertices];
point2 sphereUVs[numSphereVertices];

// four equally spaced points on the unit circle

point4 v[4] = {
    vec4( 0, 0, 1, 1 ), 
    vec4(0.0, 0.942809, -0.333333, 1.0),
    vec4(-0.816497, -0.471405, -0.333333, 1.0),
    vec4(0.816497, -0.471405, -0.333333, 1.0)
};

static int k = 0;

// move a point to unit circle

point4 unit(const point4 &p)
{
    point4 c;
    double d=0;
    for(int i=0; i<3; i++) d+=p[i]*p[i];
    d=sqrt(d);
    if(d > 0) for(int i=0; i<3; i++) c[i] = p[i]/d;
    c[3] = 1;
    return c;
}

void triangle(point4 a, point4 b, point4 c)
{
    spherePoints[k] = a;
    k++;
    spherePoints[k] = b;
    k++;
    spherePoints[k] = c;
    k++;
}

void divide_triangle(point4 a, point4 b, point4 c, int n)
{
    point4 v1, v2, v3;
    if(n>0)
    {
        v1 = unit(a + b);
        v2 = unit(a + c);
        v3 = unit(b + c);   
        divide_triangle(a , v2, v1, n-1);
        divide_triangle(c , v3, v2, n-1);
        divide_triangle(b , v1, v3, n-1);
        divide_triangle(v1, v2, v3, n-1);
    }
    else triangle(a, b, c);
}

void tetrahedron(int n)
{
    divide_triangle(v[0], v[1], v[2], n);
    divide_triangle(v[3], v[2], v[1], n);
    divide_triangle(v[0], v[3], v[1], n);
    divide_triangle(v[0], v[3], v[2], n);
}

// initialization

void generateSphere(GLuint program, ShapeData* sphereData)
{
    tetrahedron(N);

    sphereData->numVertices = numSphereVertices;

    // Normals
    for (int i = 0; i < numSphereVertices; i++)
    {
        sphereNormals[i] = point3(spherePoints[i].x, spherePoints[i].y, spherePoints[i].z);
    }
    
    // TexCoords
    double u, v;
    for (int i = 0; i < numSphereVertices; i++)
    {
        v = 0.5 - asin(spherePoints[i].y)/PI ; //0~1
        u = 0.5*(atan2(spherePoints[i].z,spherePoints[i].x)/PI + 1); //0~1
        sphereUVs[i] = point2(u,v);
    }

    // Create a vertex array object
    glGenVertexArrays( 1, &sphereData->vao );
    glBindVertexArray( sphereData->vao );

    // Set vertex attributes
    setVertexAttrib(program,
        (float*)spherePoints,  sizeof(spherePoints),
        (float*)sphereNormals, sizeof(sphereNormals),
        (float*)sphereUVs, sizeof(sphereUVs));
}

//----------------------------------------------------------------------------
// Cone

const int numConeDivisions = 32;
const int numConeVertices = numConeDivisions * 6;

point4 conePoints [numConeVertices];
point3 coneNormals[numConeVertices];

point2 circlePoints[numConeDivisions];

void makeCircle(point2* dest, int numDivisions)
{
    for (int i = 0; i < numDivisions; i++)
    {
        float a = i * 2 * PI / numDivisions;
        dest[i] = point2(cosf(a), sinf(a));
    }
}

void makeConeWall(point4* destp, point3* destn, int numDivisions, float z1, float z2, int& Index, int dir)
{
    for (int i = 0; i < numDivisions; i++)
    {
        point3 p1(circlePoints[i].x, circlePoints[i].y, z1);
        point3 p2(0, 0, z2);
        point3 p3(circlePoints[(i+1)%numDivisions].x, circlePoints[(i+1)%numDivisions].y, z1);
        if (dir == -1)
        {
            point3 temp = p1;
            p1 = p3;
            p3 = temp;
        }
        point3 n = cross(p1-p2, p3-p2);
        destp[Index] = p1; destn[Index] = n; Index++;
        destp[Index] = p2; destn[Index] = n; Index++;
        destp[Index] = p3; destn[Index] = n; Index++;
    }    
}

void generateCone(GLuint program, ShapeData* coneData)
{
    makeCircle(circlePoints, numConeDivisions);
    int Index = 0;
    makeConeWall(conePoints, coneNormals, numConeDivisions, 1, 1, Index, 1);
    makeConeWall(conePoints, coneNormals, numConeDivisions, 1, -1, Index, -1);
    
    coneData->numVertices = numConeVertices;

    // Create a vertex array object
    glGenVertexArrays( 1, &coneData->vao );
    glBindVertexArray( coneData->vao );
    
    // Set vertex attributes
    setVertexAttrib(program,
                    (float*)conePoints,  sizeof(conePoints),
                    (float*)coneNormals, sizeof(coneNormals),
                    0, 0 );
}

//----------------------------------------------------------------------------
// Cylinder

const int numCylDivisions = 32;
const int numCylVertices = numCylDivisions * 12;

point4 cylPoints [numCylVertices];
point3 cylNormals[numCylVertices];

void generateCylinder(GLuint program, ShapeData* cylData)
{
    makeCircle(circlePoints, numCylDivisions);
    int Index = 0;
    makeConeWall(cylPoints, cylNormals, numCylDivisions, 1, 1, Index, 1);
    makeConeWall(cylPoints, cylNormals, numCylDivisions, -1, -1, Index, -1);
    
    for (int i = 0; i < numCylDivisions; i++)
    {
        int i2 = (i+1)%numCylDivisions;
        point3 p1(circlePoints[i2].x, circlePoints[i2].y, -1);
        point3 p2(circlePoints[i2].x, circlePoints[i2].y, 1);
        point3 p3(circlePoints[i].x,  circlePoints[i].y,  1);
        //point3 n = cross(p3-p2, p1-p2);
        cylPoints[Index] = p1; cylNormals[Index] = point3(p1.x, p1.y, 0); Index++;
        cylPoints[Index] = p2; cylNormals[Index] = point3(p2.x, p2.y, 0); Index++;
        cylPoints[Index] = p3; cylNormals[Index] = point3(p3.x, p3.y, 0); Index++;
        p1 = point3(circlePoints[i2].x, circlePoints[i2].y, -1);
        p2 = point3(circlePoints[i].x,  circlePoints[i].y,  1);
        p3 = point3(circlePoints[i].x,  circlePoints[i].y,  -1);
        //n = cross(p3-p2, p1-p2);
        cylPoints[Index] = p1; cylNormals[Index] = point3(p1.x, p1.y, 0); Index++;
        cylPoints[Index] = p2; cylNormals[Index] = point3(p2.x, p2.y, 0); Index++;
        cylPoints[Index] = p3; cylNormals[Index] = point3(p3.x, p3.y, 0); Index++;
    }
    
    cylData->numVertices = numCylVertices;
    
    // Create a vertex array object
    glGenVertexArrays( 1, &cylData->vao );
    glBindVertexArray( cylData->vao );
    
    // Set vertex attributes
    setVertexAttrib(program,
                    (float*)cylPoints,  sizeof(cylPoints),
                    (float*)cylNormals, sizeof(cylNormals),
                    0, 0 );
}