#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
// #include <OpenGL/gl3.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp>	// GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

int gWidth, gHeight;

// ############################# Bunny transformation Matricis Start #############################
GLint modelingMatrixLocBunny;
GLint viewingMatrixLocBunny;
GLint projectionMatrixLocBunny;

glm::mat4 projectionMatrixBunny;
glm::mat4 viewingMatrixBunny;
glm::mat4 modelingMatrixBunny;

// ############################# Bunny transformation Matricis End #############################

// ############################# Quad transformation Matricis Start #############################
GLint modelingMatrixLocQuad;
GLint viewingMatrixLocQuad;
GLint projectionMatrixLocQuad;

glm::mat4 projectionMatrixQuad;
glm::mat4 viewingMatrixQuad;
glm::mat4 modelingMatrixQuad;

// ############################# Quad transformation Matricis End #############################

// ############################# Cube transformation Matricis Start #############################
GLint modelingMatrixLocCube;
GLint viewingMatrixLocCube;
GLint projectionMatrixLocCube;

glm::mat4 projectionMatrixCube;
glm::mat4 viewingMatrixCube;
glm::mat4 modelingMatrixCube;

// ############################# Cube transformation Matricis End #############################

// ############################# Global variables for bunny, quad, and cube shading programs Start #############################
const int bunnyProgram = 0;
const int quadProgram = 1;
const int cubeProgram = 2;
GLuint gProgram[3];
// ############################# Global variables for bunny, quad, and cube shading programs End #############################

// ############################# Data structures for bunny, quad, and cube Start #############################
struct Vertex
{
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) {}
	GLfloat x, y, z;
};

struct Texture
{
	Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) {}
	GLfloat u, v;
};

struct Normal
{
	Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) {}
	GLfloat x, y, z;
};

struct Face
{
	Face(int v[], int t[], int n[])
	{
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	GLuint vIndex[3], tIndex[3], nIndex[3];
};

// ############################# Data structures for bunny, quad, and cube End #############################

// vector<Vertex> gVertices;
// vector<Texture> gTextures;
// vector<Normal> gNormals;

// ############################# Global variables for bunny, quad, and cube Start #############################
vector<Texture> gTexturesBunny;
vector<Face> gFacesBunny;
vector<Vertex> gVerticesBunny;
vector<Normal> gNormalsBunny;

vector<Texture> gTexturesQuad;
vector<Face> gFacesQuad;
vector<Vertex> gVerticesQuad;
vector<Normal> gNormalsQuad;

vector<Texture> gTexturesCube;
vector<Face> gFacesCube;
vector<Vertex> gVerticesCube;
vector<Normal> gNormalsCube;

// ############################# Global variables for bunny, quad, and cube End #############################

GLuint gVertexAttribBufferBunny, gIndexBufferBunny;
GLuint gVertexAttribBufferQuad, gIndexBufferQuad;
GLuint gVertexAttribBufferCube, gIndexBufferCube;

GLint gInVertexLoc, gInNormalLoc;
uint gVertexDataSizeInBytes, gNormalDataSizeInBytes;

/*
This function parses the obj file and stores the vertices, normals, and faces in the global variables (gVertices, gNormals, and gFaces).
Those global variables are then used to create the vertex buffer object (VBO) and the index buffer object (IBO) in initVBO().
*/
bool ParseObj(const string &fileName, vector<Texture> &gTextures, vector<Vertex> &gVertices, vector<Normal> &gNormals, vector<Face> &gFaces)
{
	fstream myfile;

	// Open the input
	myfile.open(fileName.c_str(), std::ios::in);

	if (myfile.is_open())
	{
		string curLine;

		while (getline(myfile, curLine))
		{
			stringstream str(curLine);
			GLfloat c1, c2, c3;
			GLuint index[9];
			string tmp;

			if (curLine.length() >= 2)
			{
				if (curLine[0] == 'v')
				{
					if (curLine[1] == 't') // texture
					{
						str >> tmp; // consume "vt"
						str >> c1 >> c2;
						gTextures.push_back(Texture(c1, c2));
					}
					else if (curLine[1] == 'n') // normal
					{
						str >> tmp; // consume "vn"
						str >> c1 >> c2 >> c3;
						gNormals.push_back(Normal(c1, c2, c3));
					}
					else // vertex
					{
						str >> tmp; // consume "v"
						str >> c1 >> c2 >> c3;
						gVertices.push_back(Vertex(c1, c2, c3));
					}
				}
				else if (curLine[0] == 'f') // face
				{
					str >> tmp; // consume "f"
					char c;
					int vIndex[3], nIndex[3], tIndex[3];
					str >> vIndex[0];
					str >> c >> c; // consume "//"
					str >> nIndex[0];
					str >> vIndex[1];
					str >> c >> c; // consume "//"
					str >> nIndex[1];
					str >> vIndex[2];
					str >> c >> c; // consume "//"
					str >> nIndex[2];

					assert(vIndex[0] == nIndex[0] &&
						   vIndex[1] == nIndex[1] &&
						   vIndex[2] == nIndex[2]); // a limitation for now

					// make indices start from 0
					for (int c = 0; c < 3; ++c)
					{
						vIndex[c] -= 1;
						nIndex[c] -= 1;
						tIndex[c] -= 1;
					}

					gFaces.push_back(Face(vIndex, tIndex, nIndex));
				}
				else
				{
					cout << "Ignoring unidentified line in obj file: " << curLine << endl;
				}
			}

			// data += curLine;
			if (!myfile.eof())
			{
				// data += "\n";
			}
		}

		myfile.close();
	}
	else
	{
		return false;
	}

	/*
	for (int i = 0; i < gVertices.size(); ++i)
	{
		Vector3 n;

		for (int j = 0; j < gFaces.size(); ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				if (gFaces[j].vIndex[k] == i)
				{
					// face j contains vertex i
					Vector3 a(gVertices[gFaces[j].vIndex[0]].x,
							  gVertices[gFaces[j].vIndex[0]].y,
							  gVertices[gFaces[j].vIndex[0]].z);

					Vector3 b(gVertices[gFaces[j].vIndex[1]].x,
							  gVertices[gFaces[j].vIndex[1]].y,
							  gVertices[gFaces[j].vIndex[1]].z);

					Vector3 c(gVertices[gFaces[j].vIndex[2]].x,
							  gVertices[gFaces[j].vIndex[2]].y,
							  gVertices[gFaces[j].vIndex[2]].z);

					Vector3 ab = b - a;
					Vector3 ac = c - a;
					Vector3 normalFromThisFace = (ab.cross(ac)).getNormalized();
					n += normalFromThisFace;
				}

			}
		}

		n.normalize();

		gNormals.push_back(Normal(n.x, n.y, n.z));
	}
	*/

	assert(gVertices.size() == gNormals.size());

	return true;
}

// ############################# Reading shader files and creating shader objects Start #############################
bool ReadDataFromFile(
	const string &fileName, ///< [in]  Name of the shader file
	string &data)			///< [out] The contents of the file
{
	fstream myfile;

	// Open the input
	myfile.open(fileName.c_str(), std::ios::in);

	if (myfile.is_open())
	{
		string curLine;

		while (getline(myfile, curLine))
		{
			data += curLine;
			if (!myfile.eof())
			{
				data += "\n";
			}
		}

		myfile.close();
	}
	else
	{
		return false;
	}

	return true;
}

GLuint createVS(const char *shaderName)
{
	string shaderSource;

	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource))
	{
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar *shader = (const GLchar *)shaderSource.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &shader, &length);
	glCompileShader(vs);

	char output[1024] = {0};
	glGetShaderInfoLog(vs, 1024, &length, output);
	printf("VS compile log: %s\n", output);

	return vs;
}

GLuint createFS(const char *shaderName)
{
	string shaderSource;

	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource))
	{
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar *shader = (const GLchar *)shaderSource.c_str();

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &shader, &length);
	glCompileShader(fs);

	char output[1024] = {0};
	glGetShaderInfoLog(fs, 1024, &length, output);
	printf("FS compile log: %s\n", output);

	return fs;
}

void initShaders()
{
	// Create the programs

	gProgram[0] = glCreateProgram();
	gProgram[1] = glCreateProgram();
	gProgram[2] = glCreateProgram();

	// Create the shaders for both programs

	GLuint vs0 = createVS("vert0.glsl");
	GLuint fs0 = createFS("frag0.glsl");

	GLuint vs1 = createVS("vert1.glsl");
	GLuint fs1 = createFS("frag1.glsl");

	GLuint vs2 = createVS("vert2.glsl");
	GLuint fs2 = createFS("frag2.glsl");

	// Attach the shaders to the programs

	glAttachShader(gProgram[0], vs0);
	glAttachShader(gProgram[0], fs0);

	glAttachShader(gProgram[1], vs1);
	glAttachShader(gProgram[1], fs1);

	glAttachShader(gProgram[2], vs2);
	glAttachShader(gProgram[2], fs2);

	// Link the programs

	glLinkProgram(gProgram[0]);
	GLint status;
	glGetProgramiv(gProgram[0], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	glLinkProgram(gProgram[1]);
	glGetProgramiv(gProgram[1], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	glLinkProgram(gProgram[2]);
	glGetProgramiv(gProgram[2], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	// Get the locations of the uniform variables from both programs

	modelingMatrixLocBunny = glGetUniformLocation(gProgram[bunnyProgram], "modelingMatrix");
	viewingMatrixLocBunny = glGetUniformLocation(gProgram[bunnyProgram], "viewingMatrix");
	projectionMatrixLocBunny = glGetUniformLocation(gProgram[bunnyProgram], "projectionMatrix");

	modelingMatrixLocQuad = glGetUniformLocation(gProgram[quadProgram], "modelingMatrix");
	viewingMatrixLocQuad = glGetUniformLocation(gProgram[quadProgram], "viewingMatrix");
	projectionMatrixLocQuad = glGetUniformLocation(gProgram[quadProgram], "projectionMatrix");

	modelingMatrixLocCube = glGetUniformLocation(gProgram[cubeProgram], "modelingMatrix");
	viewingMatrixLocCube = glGetUniformLocation(gProgram[cubeProgram], "viewingMatrix");
	projectionMatrixLocCube = glGetUniformLocation(gProgram[cubeProgram], "projectionMatrix");
}

// ############################# Reading shader files and creating shader objects End #############################

void initVBO()
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	assert(vao > 0);
	glBindVertexArray(vao);
	cout << "vao = " << vao << endl;

	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &gVertexAttribBufferBunny);
	glGenBuffers(1, &gIndexBufferBunny);

	// cout << "gVertexAttribBufferBunny = " << gVertexAttribBufferBunny << endl;

	assert(gVertexAttribBufferBunny > 0 && gIndexBufferBunny > 0);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBufferBunny);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferBunny);

	gVertexDataSizeInBytes = gVerticesBunny.size() * 3 * sizeof(GLfloat);
	gNormalDataSizeInBytes = gNormalsBunny.size() * 3 * sizeof(GLfloat);
	int indexDataSizeInBytes = gFacesBunny.size() * 3 * sizeof(GLuint);

	GLfloat *vertexData = new GLfloat[gVerticesBunny.size() * 3];
	GLfloat *normalData = new GLfloat[gNormalsBunny.size() * 3];
	GLuint *indexData = new GLuint[gFacesBunny.size() * 3];

	float minX = 1e6, maxX = -1e6;
	float minY = 1e6, maxY = -1e6;
	float minZ = 1e6, maxZ = -1e6;

	for (int i = 0; i < gVerticesBunny.size(); ++i)
	{
		vertexData[3 * i] = gVerticesBunny[i].x;
		vertexData[3 * i + 1] = gVerticesBunny[i].y;
		vertexData[3 * i + 2] = gVerticesBunny[i].z;

		minX = std::min(minX, gVerticesBunny[i].x);
		maxX = std::max(maxX, gVerticesBunny[i].x);
		minY = std::min(minY, gVerticesBunny[i].y);
		maxY = std::max(maxY, gVerticesBunny[i].y);
		minZ = std::min(minZ, gVerticesBunny[i].z);
		maxZ = std::max(maxZ, gVerticesBunny[i].z);
	}

	std::cout << "minX = " << minX << std::endl;
	std::cout << "maxX = " << maxX << std::endl;
	std::cout << "minY = " << minY << std::endl;
	std::cout << "maxY = " << maxY << std::endl;
	std::cout << "minZ = " << minZ << std::endl;
	std::cout << "maxZ = " << maxZ << std::endl;

	for (int i = 0; i < gNormalsBunny.size(); ++i)
	{
		normalData[3 * i] = gNormalsBunny[i].x;
		normalData[3 * i + 1] = gNormalsBunny[i].y;
		normalData[3 * i + 2] = gNormalsBunny[i].z;
	}

	for (int i = 0; i < gFacesBunny.size(); ++i)
	{
		indexData[3 * i] = gFacesBunny[i].vIndex[0];
		indexData[3 * i + 1] = gFacesBunny[i].vIndex[1];
		indexData[3 * i + 2] = gFacesBunny[i].vIndex[2];
	}

	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	// done copying to GPU memory; can free now from CPU memory
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	// ############################# Quad VBO Start #############################
	// glGenVertexArrays(1, &vao);
	// assert(vao > 0);
	// glBindVertexArray(vao);
	// cout << "vao = " << vao << endl;

	// assert(glGetError() == GL_NONE);

	glGenBuffers(1, &gVertexAttribBufferQuad);
	glGenBuffers(1, &gIndexBufferQuad);

	assert(gVertexAttribBufferQuad > 0 && gIndexBufferQuad > 0);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBufferQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferQuad);

	gVertexDataSizeInBytes = gVerticesQuad.size() * 3 * sizeof(GLfloat);
	gNormalDataSizeInBytes = gNormalsQuad.size() * 3 * sizeof(GLfloat);
	indexDataSizeInBytes = gFacesQuad.size() * 3 * sizeof(GLuint);

	vertexData = new GLfloat[gVerticesQuad.size() * 3];
	normalData = new GLfloat[gNormalsQuad.size() * 3];
	indexData = new GLuint[gFacesQuad.size() * 3];

	for (int i = 0; i < gVerticesQuad.size(); ++i)
	{
		vertexData[3 * i] = gVerticesQuad[i].x;
		vertexData[3 * i + 1] = gVerticesQuad[i].y;
		vertexData[3 * i + 2] = gVerticesQuad[i].z;
	}

	for (int i = 0; i < gNormalsQuad.size(); ++i)
	{
		normalData[3 * i] = gNormalsQuad[i].x;
		normalData[3 * i + 1] = gNormalsQuad[i].y;
		normalData[3 * i + 2] = gNormalsQuad[i].z;
	}

	for (int i = 0; i < gFacesQuad.size(); ++i)
	{
		indexData[3 * i] = gFacesQuad[i].vIndex[0];
		indexData[3 * i + 1] = gFacesQuad[i].vIndex[1];
		indexData[3 * i + 2] = gFacesQuad[i].vIndex[2];
	}

	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	// done copying to GPU memory; can free now from CPU memory
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	// ############################# Quad VBO End #############################

	// ############################# Cube VBO Start #############################
	glGenBuffers(1, &gVertexAttribBufferCube);
	glGenBuffers(1, &gIndexBufferCube);

	assert(gVertexAttribBufferCube > 0 && gIndexBufferCube > 0);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBufferCube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferCube);

	gVertexDataSizeInBytes = gVerticesCube.size() * 3 * sizeof(GLfloat);
	gNormalDataSizeInBytes = gNormalsCube.size() * 3 * sizeof(GLfloat);
	indexDataSizeInBytes = gFacesCube.size() * 3 * sizeof(GLuint);

	vertexData = new GLfloat[gVerticesCube.size() * 3];
	normalData = new GLfloat[gNormalsCube.size() * 3];
	indexData = new GLuint[gFacesCube.size() * 3];

	for (int i = 0; i < gVerticesCube.size(); ++i)
	{
		vertexData[3 * i] = gVerticesCube[i].x;
		vertexData[3 * i + 1] = gVerticesCube[i].y;
		vertexData[3 * i + 2] = gVerticesCube[i].z;
	}

	for (int i = 0; i < gNormalsCube.size(); ++i)
	{
		normalData[3 * i] = gNormalsCube[i].x;
		normalData[3 * i + 1] = gNormalsCube[i].y;
		normalData[3 * i + 2] = gNormalsCube[i].z;
	}

	for (int i = 0; i < gFacesCube.size(); ++i)
	{
		indexData[3 * i] = gFacesCube[i].vIndex[0];
		indexData[3 * i + 1] = gFacesCube[i].vIndex[1];
		indexData[3 * i + 2] = gFacesCube[i].vIndex[2];
	}

	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	// done copying to GPU memory; can free now from CPU memory
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	// ############################# Cube VBO End #############################
}

void init()
{
	// Parsing objects from obj files
	ParseObj("bunny.obj", gTexturesBunny, gVerticesBunny, gNormalsBunny, gFacesBunny);
	ParseObj("quad.obj", gTexturesBunny, gVerticesQuad, gNormalsQuad, gFacesQuad);
	ParseObj("cube.obj", gTexturesBunny, gVerticesCube, gNormalsCube, gFacesCube);

	glEnable(GL_DEPTH_TEST); // enable depth-testing

	initShaders();

	initVBO();
}

void drawModel(vector<Face> &gFaces)
{
	// glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));

	glDrawElements(GL_TRIANGLES, gFaces.size() * 3, GL_UNSIGNED_INT, 0);
}

void display()
{
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// ############################# Draw the bunny Start #############################

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBufferBunny);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferBunny);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVerticesBunny.size() * 3 * sizeof(GLfloat)));

	// Handle bunny hopping
	static float y_value = 0;
	static bool isGoingUp = true;

	// Compute the modeling matrix
	// glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, -10.f));
	// glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));
	// glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-180. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0));
	// glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(0.0, 0.0, 1.0));
	// modelingMatrix = matT * matRz * matR; // starting from right side, rotate around Y to turn back, then rotate around Z some more at each frame, then translate.

	// Model matrix to make the bunny jump up and down
	glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, -8.f, -10.f));
	// Rotate the bunny aroudd the Y axis 90 degrees
	glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), -M_PI / 2., glm::vec3(0.0, 1.0, 0.0));
	// Translate the bunny up and down
	glm::mat4 matTy = glm::translate(glm::mat4(1.0), glm::vec3(0.f, y_value, 0.f));
	if (isGoingUp)
	{
		if (y_value >= 3)
		{
			isGoingUp = false;
		}
		y_value += 0.1;
	}
	else
	{
		if (y_value <= -1)
		{
			isGoingUp = true;
		}
		y_value -= 0.1;
	}

	modelingMatrixBunny = matT * matR * matTy;

	// or... (care for the order! first the very bottom one is applied)
	// modelingMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, -3.f));
	// modelingMatrix = glm::rotate(modelingMatrix, angleRad, glm::vec3(0.0, 0.0, 1.0));
	// modelingMatrix = glm::rotate<float>(modelingMatrix, (-180. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0));

	// Set the active program and the values of its uniform variables
	glUseProgram(gProgram[bunnyProgram]);
	glUniformMatrix4fv(projectionMatrixLocBunny, 1, GL_FALSE, glm::value_ptr(projectionMatrixBunny));
	glUniformMatrix4fv(viewingMatrixLocBunny, 1, GL_FALSE, glm::value_ptr(viewingMatrixBunny));
	glUniformMatrix4fv(modelingMatrixLocBunny, 1, GL_FALSE, glm::value_ptr(modelingMatrixBunny));

	// Draw the scene
	drawModel(gFacesBunny);

	// ############################# Draw the bunny End #############################

	// ############################# Draw the quad Start #############################

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBufferQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferQuad);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVerticesQuad.size() * 3 * sizeof(GLfloat)));

	// angle
	glm::mat4 matT2 = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, -10.f));
	glm::mat4 matS2 = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));
	glm::mat4 matR2 = glm::rotate<float>(glm::mat4(1.0), (-180. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0));
	// glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(0.0, 0.0, 1.0));

	modelingMatrixQuad = matT2 * matS2 * matR2;

	glUseProgram(gProgram[quadProgram]);
	glUniformMatrix4fv(projectionMatrixLocQuad, 1, GL_FALSE, glm::value_ptr(projectionMatrixQuad));
	glUniformMatrix4fv(viewingMatrixLocQuad, 1, GL_FALSE, glm::value_ptr(viewingMatrixQuad));
	glUniformMatrix4fv(modelingMatrixLocQuad, 1, GL_FALSE, glm::value_ptr(modelingMatrixQuad));

	drawModel(gFacesQuad);

	// ############################# Draw the Red CUBE 1 Start #############################

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBufferCube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferCube);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVerticesCube.size() * 3 * sizeof(GLfloat)));

	glm::mat4 matT3 = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 5.f, -10.f));
	glm::mat4 matS3 = glm::scale(glm::mat4(1.0), glm::vec3(1, 1.5, 1));
	// glm::mat4 matR3 = glm::rotate<float>(glm::mat4(1.0), M_PI / 2.0, glm::vec3(0.0, 1.0, 0.0));
	// glm::mat4 matRz = glm::rotate(glm::mat4(1.0), M_PI * 0.2, glm::vec3(0.0, 0.0, 1.0));
	modelingMatrixCube = matT3 * matS3;

	glUseProgram(gProgram[cubeProgram]);
	glUniformMatrix4fv(projectionMatrixLocCube, 1, GL_FALSE, glm::value_ptr(projectionMatrixCube));
	glUniformMatrix4fv(viewingMatrixLocCube, 1, GL_FALSE, glm::value_ptr(viewingMatrixCube));
	glUniformMatrix4fv(modelingMatrixLocCube, 1, GL_FALSE, glm::value_ptr(modelingMatrixCube));

	drawModel(gFacesCube);

	// ############################# Draw the Red CUBE 2 Start #############################

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBufferCube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferCube);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVerticesCube.size() * 3 * sizeof(GLfloat)));

	matT3 = glm::translate(glm::mat4(1.0), glm::vec3(5.f, 5.f, -10.f));
	matS3 = glm::scale(glm::mat4(1.0), glm::vec3(1, 1.5, 1));
	// glm::mat4 matR3 = glm::rotate<float>(glm::mat4(1.0), M_PI / 2.0, glm::vec3(0.0, 1.0, 0.0));
	// glm::mat4 matRz = glm::rotate(glm::mat4(1.0), M_PI * 0.2, glm::vec3(0.0, 0.0, 1.0));
	modelingMatrixCube = matT3 * matS3;

	glUseProgram(gProgram[cubeProgram]);
	glUniformMatrix4fv(projectionMatrixLocCube, 1, GL_FALSE, glm::value_ptr(projectionMatrixCube));
	glUniformMatrix4fv(viewingMatrixLocCube, 1, GL_FALSE, glm::value_ptr(viewingMatrixCube));
	glUniformMatrix4fv(modelingMatrixLocCube, 1, GL_FALSE, glm::value_ptr(modelingMatrixCube));

	drawModel(gFacesCube);
}

void reshape(GLFWwindow *window, int w, int h)
{
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;

	gWidth = w;
	gHeight = h;

	glViewport(0, 0, w, h);

	// Use perspective projection
	float fovyRad = (float)(90.0 / 180.0) * M_PI;
	projectionMatrixBunny = glm::perspective(fovyRad, w / (float)h, 1.0f, 100.0f);
	projectionMatrixQuad = glm::perspective(fovyRad, w / (float)h, 1.0f, 100.0f);
	projectionMatrixCube = glm::perspective(fovyRad, w / (float)h, 1.0f, 100.0f);

	// Assume default camera position and orientation (camera is at
	// (0, 0, 0) with looking at -z direction and its up vector pointing
	// at +y direction)
	//
	// viewingMatrix = glm::mat4(1);
	viewingMatrixBunny = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	viewingMatrixQuad = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	viewingMatrixCube = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		glShadeModel(GL_FLAT);
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		glShadeModel(GL_SMOOTH);
	}
	else if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void mainLoop(GLFWwindow *window)
{
	while (!glfwWindowShouldClose(window))
	{
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main(int argc, char **argv) // Create Main Function For Bringing It All Together
{

	// ############################ Some Necessary Initializations Start ############################
	GLFWwindow *window;
	if (!glfwInit())
	{
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width = 1000, height = 800;
	window = glfwCreateWindow(width, height, "Bunny Run", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	char rendererInfo[512] = {0};
	strcpy(rendererInfo, (const char *)glGetString(GL_RENDERER)); // Use strcpy_s on Windows, strcpy on Linux
	strcat(rendererInfo, " - ");								  // Use strcpy_s on Windows, strcpy on Linux
	strcat(rendererInfo, (const char *)glGetString(GL_VERSION));  // Use strcpy_s on Windows, strcpy on Linux
	glfwSetWindowTitle(window, rendererInfo);
	// ############################ Some Necessary Initializations End ############################

	init();

	glfwSetKeyCallback(window, keyboard);
	glfwSetWindowSizeCallback(window, reshape);

	reshape(window, width, height); // need to call this once ourselves
	mainLoop(window);				// this does not return unless the window is closed

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void x_z(int ll)
{
	std::cout << "This is sublime text" << std::endl;
}
