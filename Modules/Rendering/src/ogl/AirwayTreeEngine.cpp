#include "AirwayTreeEngine.h"
#include <iostream>

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
//#include <vtkPolyDataNormals.h>
/* Render Example
#include <vtkPLYReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
*/

AirwayTree::AirwayTree(std::string filename)
{
	initAirwayTreeGeometry(filename);
}

AirwayTree::~AirwayTree()
{
	glDeleteVertexArrays(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
}

void AirwayTree::drawAirwayTreeGeometry()
{
	mmOpenGLClearErrorMacro();

	// Binding the Vertex Array for drawing airway tree
	glBindVertexArray(vertexArray);
	// TODO: bind the vertex buffer and index buffers

	// drawing point clouds
	// glDrawArrays(GL_POINTS, 0, numVerts);

	// drawing triangles
	glDrawElements(GL_TRIANGLES, static_cast<GLuint>(dataIndices.size()), GL_UNSIGNED_INT, 0);

	mmOpenGLCheckErrorMacro("Error with drawing airway tree Geometry - OpenGL");
}

glm::vec3 AirwayTree::getGeometryMeanLocation()
{
	float averageX = std::accumulate(dataX.begin(), dataX.end(), 0.0) / dataX.size();
	float averageY = std::accumulate(dataY.begin(), dataY.end(), 0.0) / dataY.size();
	float averageZ = std::accumulate(dataZ.begin(), dataZ.end(), 0.0) / dataZ.size();

	return glm::vec3(averageX, averageY, averageZ);

	/* Mean values so I can translate to 0,0,0 in world - ROIMTEXT */
	/* X = 92, Y = 74, Z = 127 */

	// 	std::cout << "Mean X: " << averageX << "Mean Y: " << averageY << "Mean Z: " << averageZ << std::endl;
}

// TODO = fix assumption: vtk poly data is drawn only as triangles
void AirwayTree::initAirwayTreeGeometry(std::string filename)
{
	// TODO: Avoid vector.push_back() - it seems relatively slow.
	// 	Find and read in vtk poly data
	vtkSmartPointer<vtkPolyDataReader> polyDataReader = vtkSmartPointer<vtkPolyDataReader>::New();
	polyDataReader->SetFileName(filename.c_str());
	polyDataReader->Update();

	// Extract the Poly Data - in this case I am finding the x,y,z points
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData = polyDataReader->GetOutput();

	/* VERTEX POSITIONS */
	// Extract all points into a float vector, polyData Get Point returns double
	dataPosAttribs.clear();
	dataX.clear();
	dataY.clear();
	dataZ.clear();
	numVerts = static_cast<int>(polyData->GetNumberOfPoints());
	for (int i = 0; i < numVerts; i++)
	{
		double p[3];
		polyData->GetPoint(i, p);
		dataPosAttribs.push_back(static_cast<GLfloat>(p[0]));
		dataPosAttribs.push_back(static_cast<GLfloat>(p[1]));
		dataPosAttribs.push_back(static_cast<GLfloat>(p[2]));
		dataX.push_back(static_cast<GLfloat>(p[0]));
		dataY.push_back(static_cast<GLfloat>(p[1]));
		dataZ.push_back(static_cast<GLfloat>(p[2]));
	}

	
	/* INDICIES AND NORMALS */
	// Indices - for drawing triangles/poly (vertex) in order
	// Normals - for each triangle 

	// Initialize vector that stores normal vectors for each vertex on each triangle
	// Afterwards, the normal vectors will be averaged for each vertex
	std::vector< std::vector<float> > normTempAccumulated;
	for (int i = 0; i < numVerts; i++)
	{
		normTempAccumulated.push_back(std::vector<GLfloat>());
	}

	vtkSmartPointer<vtkCellArray> polyCells = vtkSmartPointer<vtkCellArray>::New();
	polyCells = polyData->GetPolys();
	dataIndices.clear();
	vtkIdType numCells = polyData->GetNumberOfCells();
	vtkIdType cellLocation = 0; // the index into the cell array
	for (vtkIdType i = 0; i < numCells; i++)
	{
		vtkIdType numIds;
		const vtkIdType *pointsIds;
		polyCells->GetCell(cellLocation, numIds, pointsIds);
		cellLocation += 1 + numIds;

		//calculate the normal for each triangle
		double p[3];
		polyData->GetPoint(pointsIds[0], p);
		glm::vec3 pointI = glm::vec3(static_cast<GLfloat>(p[0]), static_cast<GLfloat>(p[1]), static_cast<GLfloat>(p[2]));
		polyData->GetPoint(pointsIds[1], p);
		glm::vec3 pointJ = glm::vec3(static_cast<GLfloat>(p[0]), static_cast<GLfloat>(p[1]), static_cast<GLfloat>(p[2]));
		polyData->GetPoint(pointsIds[2], p);
		glm::vec3 pointK = glm::vec3(static_cast<GLfloat>(p[0]), static_cast<GLfloat>(p[1]), static_cast<GLfloat>(p[2]));

		glm::vec3 normVec = glm::normalize(glm::cross((pointJ - pointI), (pointK - pointI)));

		// Push each vertex index in order into a vector
		// Push normal vector into each vertex of the triangle
		for (vtkIdType j = 0; j < numIds; j++)
		{
			dataIndices.push_back(static_cast<GLuint>(pointsIds[j]));
			normTempAccumulated[pointsIds[j]].push_back(normVec.x);
			normTempAccumulated[pointsIds[j]].push_back(normVec.y);
			normTempAccumulated[pointsIds[j]].push_back(normVec.z);
		}
		// alert if any polygon is not a triangle (if assumption fails)
		if (numIds > 3)
		{
			std::cout << "More than 3 vertices: " << numIds << "for cell: "<< cellLocation << std::endl;
		}
	}
	
	// Normal Averaging for each vertex
	// cycle through each vertex
	dataNormAttribs.clear();
	for (int i = 0; i < numVerts; i++)
	{
		int sizeVert = static_cast<int>(normTempAccumulated[i].size());
		
		// If a vertex has more than one normal vector, average
		// Note: if a vertex is not part of a drawn triangle, set normal vector to 0
		if ((sizeVert > 3) && (sizeVert % 3 == 0)) // averaging
		{
			float x = 0;
			float y = 0;
			float z = 0;			
			for (int j = 0; j < (sizeVert-2); j += 3)
			{				
				x += normTempAccumulated[i][j];
				y += normTempAccumulated[i][j+1];
				z += normTempAccumulated[i][j+2];
			}
			x /= sizeVert;
			y /= sizeVert;
			z /= sizeVert;
			// TODO: MULTIPLIES ARE ALOT CHEAPER!
			glm::vec3 normalizeTemp = glm::normalize(glm::vec3(x, y, z));
			dataNormAttribs.push_back(normalizeTemp.x);
			dataNormAttribs.push_back(normalizeTemp.y);
			dataNormAttribs.push_back(normalizeTemp.z);
		}
		else if (sizeVert == 0) // Vertex is not part of drawn triangle
		{
			dataNormAttribs.push_back(0);
			dataNormAttribs.push_back(0);
			dataNormAttribs.push_back(0);
		}
		else if (sizeVert == 3) // No averaging needed, only based on one triangle
		{
			dataNormAttribs.push_back(normTempAccumulated[i][0]);
			dataNormAttribs.push_back(normTempAccumulated[i][1]);
			dataNormAttribs.push_back(normTempAccumulated[i][2]);
		}
		else
		{
			std::cout << "Triangle Assumption fails - polygon shape is not a triangle" << std::endl;
		}
	}

	// Combine all attributes into one vector
	// such that each vertex has[X, Y, Z NX, NY, NZ]
	// assumption: both Norm and Pos are same size (needed for specifying VAO/VBOs)
	// assumption: both are floats
	// bounds are fragile for now? (could be more elegant)
	dataVertices.clear();
	for (int i = 0; i < dataPosAttribs.size() - 2; i += 3)
	{
		dataVertices.push_back(dataPosAttribs[i]);
		dataVertices.push_back(dataPosAttribs[i + 1]);
		dataVertices.push_back(dataPosAttribs[i + 2]);
		dataVertices.push_back(dataNormAttribs[i]);
		dataVertices.push_back(dataNormAttribs[i + 1]);
		dataVertices.push_back(dataNormAttribs[i + 2]);
	}

	/* OPEN GL */
	mmOpenGLClearErrorMacro();

	// Create VAO to bind vertex buffer and index buffer to
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	// Configure the Vertex Array's Buffer
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, dataVertices.size() * sizeof(GLfloat), &dataVertices[0], GL_STATIC_DRAW);

	/* Setting up the attributes */
	// TODO: Create Position and Normal Buffers and bind these to VAO instead of one buffer containing both Position and Normal
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0x0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	
	// Creating an Index Buffer
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataIndices.size() * sizeof(GLuint), &dataIndices[0], GL_STATIC_DRAW);

	mmOpenGLCheckErrorMacro("Error with initializing airway geometry - OpenGL");

	// 	std::cout << polyData->GetNumberOfPoints() << std::endl;
	// 	std::cout << polyData->GetNumberOfCells() << std::endl;
	// 	std::cout << polyData->GetNumberOfLines() << std::endl;
	// 	std::cout << polyData->GetNumberOfPieces() << std::endl;
	// 	std::cout << polyData->GetNumberOfPolys() << std::endl;
	// 	std::cout << polyData->GetNumberOfStrips() << std::endl;
	// 	std::cout << polyData->GetNumberOfVerts() << std::endl;
	// 	std::cout << std::endl;
	// 	std::cout << polyDataNorms->GetNumberOfPoints() << std::endl;
	// 	std::cout << polyDataNorms->GetNumberOfCells() << std::endl;
	// 	std::cout << polyDataNorms->GetNumberOfLines() << std::endl;
	// 	std::cout << polyDataNorms->GetNumberOfPieces() << std::endl;
	// 	std::cout << polyDataNorms->GetNumberOfPolys() << std::endl;
	// 	std::cout << polyDataNorms->GetNumberOfStrips() << std::endl;
	// 	std::cout << polyDataNorms->GetNumberOfVerts() << std::endl;

	// Normals - for lighting - I am filtering the polyData to create a polyData with the normal vector at each point - NX,NY,NZ
	// This appears to be normalizing the position vector, rather than a normal vector at the position
	// 	vtkSmartPointer<vtkPolyDataNormals> polyNorms = vtkSmartPointer<vtkPolyDataNormals>::New();
	// 	polyNorms->SetInputData(polyData);
	// 	polyNorms->ComputePointNormalsOn();
	// 	polyNorms->ComputeCellNormalsOff();
	// 	polyNorms->SplittingOff();
	// 	polyNorms->ConsistencyOn();
	// 	polyNorms->FlipNormalsOn();
	// 	polyNorms->Update();
	// 
	// 	vtkSmartPointer<vtkPolyData> polyDataNorms = vtkSmartPointer<vtkPolyData>::New();
	// 	polyDataNorms = polyNorms->GetOutput();
	// 	dataNormAttribs.clear();
	// 	numNormVerts = static_cast<int>(polyDataNorms->GetNumberOfPoints());
	// 	for (int i = 0; i < numVerts; i++)
	// 	{
	// 		double p[3];
	// 		polyDataNorms->GetPoint(i, p);
	// 		glm::vec3 pTemp = glm::normalize(glm::vec3(p[0], p[1], p[2]));
	// 		dataNormAttribs.push_back(static_cast<float>(pTemp.x));
	// 		dataNormAttribs.push_back(static_cast<float>(pTemp.y));
	// 		dataNormAttribs.push_back(static_cast<float>(pTemp.z));
	// 	}
	// 	

	// Vertex Positions - for triangles
// 	vtkSmartPointer<vtkPoints> pointsData = vtkSmartPointer<vtkPoints>::New();
// 	pointsData = polyData->GetPoints();	

	// Find and read in the .ply file - EXAMPLE
// 	std::string inputFilename(DATA_DIR "/red_pepper.ply");
// 	vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
// 	reader->SetFileName(inputFilename.c_str());
// 	reader->Update();
// 
// 	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
// 	mapper->SetInputConnection(reader->GetOutputPort());
// 
// 	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
// 	actor->SetMapper(mapper);
// 
// 	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
// 	vtkSmartPointer<vtkRenderWindow> renderWindow =vtkSmartPointer<vtkRenderWindow>::New();
// 	renderWindow->AddRenderer(renderer);
// 	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
// 	renderWindowInteractor->SetRenderWindow(renderWindow);
// 
// 	renderer->AddActor(actor);
// 	renderer->SetBackground(0.1804, 0.5451, 0.3412); // Sea green
// 
// 	renderWindow->Render();
// 	renderWindowInteractor->Start();
	
}

