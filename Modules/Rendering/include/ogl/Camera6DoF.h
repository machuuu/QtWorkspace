#ifndef CAMERA6DOF_H
#define CAMERA6DOF_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define XCOORD			0
#define YCOORD			1
#define ZCOORD			2
#define ROLLCOORD		3
#define PITCHCOORD		4
#define YAWCOORD		5
#define PI				3.14159265359f

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// The camera is defined using Right, Up, Front Vectors and a Position in Space
// Note: camera is always positioned at 0, 0, 0 and the models are moved in world space (Model Offset)
// "Right" vector represents the camera's z-Axis (Positive Global Z)
// "Up" vector represents the camera's y-Axis (Positive Global Y)
// "Front" Vector represents the camera's x-Axis (Positive Global X)
// Using the mouse and keyboard, the Model Offset, Right, Up, and Front vectors are updated with respect to the global coordinate system

// 				Up(y)
//				|	   Front (x)
//				|	 / 
//				|   /
// 				|  /
// 				| /
// 				|/__________ Right(z)

class Camera6DoF
{
private:
	/* Camera Attributes */
	glm::vec3 m_Position;
	glm::vec3 m_ModelOffset;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	/* Quaternions */
	glm::quat rightQuat;
	glm::quat upQuat;
	glm::quat frontQuat;
	/* Camera options */
	float m_MouseMovementSpeed;
	float m_KeyboardMovementSpeed;
	float m_MouseSensitivity;
	float m_Zoom;

public:
	Camera6DoF();
	~Camera6DoF();
	
	glm::mat4 GetViewMatrix();
	glm::vec3 GetPosition();
	glm::vec3 GetModelOffset();
	glm::vec3 GetFront();
	glm::vec3 GetUp();
	glm::vec3 GetRight();
	glm::mat3 GetRotationMatrix();
	float GetKeyboardMovementSpeed();
	float GetMouseMovementSpeed();
	float GetMouseSensitivity();
	float GetZoom();
	void GetCameraParameters(float *cameraParams);

	void SetPosition(glm::vec3 cameraPosition);
	void SetModelOffset(glm::vec3 modelOffset);
	void SetFront(glm::vec3 cameraFront);
	void SetUp(glm::vec3 cameraUp);
	void SetRight(glm::vec3 cameraRight);
	void SetKeyboardMovementSpeed(float cameraKeyboardMovementSpeed);
	void SetMouseMovementSpeed(float cameraMouseMovementSpeed);
	void SetMouseSensitivity(float cameraMouseSensitivity);
	void SetZoom(float cameraZoom);
	void SetDefault();


	void ProcessMouseRotation(float xoffset, float yoffset, bool zSelect);
	void ProcessMouseTranslation(float xoffset, float yoffset, bool zSelect);
	void ProcessMouseScroll(float yoffset);
	void ProcessKeyboardTranslation(Camera_Movement direction);
	void CheckCameraVectors();

	void FrontRotation(float deg);
	void RightRotation(float deg);
	void UpRotation(float deg);


private:
	void IncrementFrontRotation(float inc);
	void IncrementRightRotation(float inc);
	void IncrementUpRotation(float inc);
	
	void IncrementRightTranslation(float inc);
	void IncrementUpTranslation(float inc);
	void IncrementFrontTranslation(float inc);

	void RotationMatrixToAngles(glm::mat3 rotMatrix, float &Yaw, float &Pitch, float &Roll);

};
#endif