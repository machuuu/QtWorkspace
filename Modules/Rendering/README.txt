**These are legacy notes, to be cleaned up later.**

Libraries:
	GLAD
	OpenGL 
	GLM 0.9.9.0
	Qt5.11.1 - Core, Gui, Widgets
	VTK 8.1.1

Task:
	Replace Qt OpenGL wrappers with regular OpenGL code except QOpenGLContext.
	Adapt Qt5.11.1 OpenGL Cube example

Compile:
	Yes

Completion?
	No

Progress:
	Updated mousePressEvent, mouseReleaseEvent, timerEvent
		- 8/22/18
	Replaced OpenGLShaderProgram with my shader class 
		- 8/23/18
	Added IndexBuffer, VertexBuffer, VertexBufferLayout and VertexArray classes 
		- 8/23/18
	Removed all classes to replace with standard OpenGL code
		- 8/29/18
	Added vtk error check macros
		- 9/4/18
		- GLEW is used instead of GLAD
	Added my macros influenced by vtk while removing vtk from project
		- 9/10/18
		- Using GLAD again
	Able to render a point cloud of the Airway Tree
		- 9/13/18
		- an airway tree flag is used to manage whether to render the cube or airway tree in initializeGL()
			- this is set when choosing either initCubeShader() or initAirwayShader() - this is a bit of a work around.
			- MainWidget constructor will need adjustment
	Added a very simple camera
		- 9/14/18
		- processes forward,backward,left,right movemements using WASD
		- processes camera view by pressing and holding mouse
	Able to render the Airway Tree Surface by drawing Triangles
		- 9/13/18
	Added basic lighting for depth
		- 9/13/18
	Corrected the lighting model
		- 9/17/18
		- normal vectors are now correct for each vertex
		- added attenuation for lighting. Light source is at camera location
	Added support for generating normal vectors
		- If the index list does not contain a vertex position for drawing a triangle
			- set normal vector to <0,0,0>
			- Noticed that when drawing interior airway tree, vertices ~16,500 to 17,300 were not included in the index list from the polygons
		- 9/18/18
	Updated Camera
		- 6DoF movement
		- left click (translation) 
		- right click (rotation)
		- hold shift for rotation about z and translation about z
		- Added keyboard control
			- w forward
			- a left
			- s backward
			- d right
			- q down
			- e up
			- t reset view
		- Camera starts near top of trachea
		- Camera class can return camera parameters
			- Position, Yaw, Pitch and Roll consistent with VNS.
		- 9/27/18
	Camera
		- Camera remains fixed at 0,0,0 and models are translated based on the camera's "model offset"
		- Fixed specular lighting
		- 10/01/18
	Added Main Window Support
		- Airway Tree render
		- Placeholders for the image viewers(slicers)
		- 10/3/18
	Image read
		- Class (CTVolume) which loads 3D CT data from .hdr and .img files
		- 10/8/18
	2D texture slice from 3D texture
		- Able to select, window and apply a colormap to a 2D slice texture from 3D CT texture
		- Simple workgroup update - now renders fast
		- 10/16/18	
	Voxel selection for each slicer
		- Mouse right click selects a voxel from the slicer's texture and updates the other two
		- Holding right click follows the mouse position for updating slicer views
		- Can accommodate zooming and panning on slicers's textures
		- 11/2/18

To Do:

	Use compute shaders for image viewers
		- orthogonal slicer
		- oblique slice / slab

	
To Do Completed:
	1.My shader class does not see the GLAD functions points
		in main.cpp - I load GLAD function pointers
	2.Shader.createShader() needs absolute paths rather than relative to the project
	3.Texture initialization needs absolute path
	4.Cube geometry isn't correct - appears fragmented.
		fixed: cubegeometry vertex v0: x position was not -1.0f
		fixed: macro for draw elements - GL_UNSIGNED_INT
	5.AIRWAY:::Fixed normal attribute for rendering airway tree
	6.AIRWAY:::Fixed the camera movement using quaternions
