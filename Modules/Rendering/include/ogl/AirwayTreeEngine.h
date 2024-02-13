#include "glad/glad.h"
#include "mmOpenGLErrorCheck.h"

#include <string>
#include <vector>
#include <numeric>

#include <glm/glm.hpp>

class AirwayTree
{
public:
	AirwayTree(std::string filename);
	~AirwayTree();

	void drawAirwayTreeGeometry();
	glm::vec3 getGeometryMeanLocation();

private:
	void initAirwayTreeGeometry(std::string filename);
	std::vector<GLfloat> dataX;
	std::vector<GLfloat> dataY;
	std::vector<GLfloat> dataZ;
	std::vector<GLfloat> dataPosAttribs;
	std::vector<GLuint> dataIndices;
	std::vector<GLfloat> dataNormAttribs;
	std::vector<GLfloat> dataVertices; 
	unsigned int vertexArray;
	unsigned int vertexBuffer;
	unsigned int indexBuffer;
	int numVerts;
	int numNormVerts;
};