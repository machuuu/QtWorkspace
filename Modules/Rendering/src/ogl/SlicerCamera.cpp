#include <SlicerCamera.h>
#include <iostream>
#include "Logger.h"

// Default Constructor
// Position at (0,0,0) and look in Z direction.
SlicerCamera::SlicerCamera() :
	m_Position(0.0f, 0.0f, 0.0f),
	m_ModelOffset(0.0f, 0.0f, 0.0f),
	m_Front(0.0f, 0.0f, 1.0f),
	m_Up(0.0f, 1.0f, 0.0f),
	m_Right(1.0f, 0.0f, 0.0f),
	m_KeyboardMovementSpeed(0.5f),
	m_MouseMovementSpeed(0.5f),
	m_MouseSensitivity(0.15f),
	m_Zoom(45.0f)
{
}

// Default Destructor
SlicerCamera::~SlicerCamera()
{
}

// Returns the view matrix based on the position, Front Vector and Up Vector
glm::mat4 SlicerCamera::GetViewMatrix()
{
	return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::vec3 SlicerCamera::GetPosition()
{
	return m_Position;
}

glm::vec3 SlicerCamera::GetModelOffset()
{
	return m_ModelOffset;
}

glm::vec3 SlicerCamera::GetFront()
{
	return m_Front;
}

glm::vec3 SlicerCamera::GetUp()
{
	return m_Up;
}

glm::vec3 SlicerCamera::GetRight()
{
	return m_Right;
}

float SlicerCamera::GetKeyboardMovementSpeed()
{
	return m_KeyboardMovementSpeed;
}

float SlicerCamera::GetMouseMovementSpeed()
{
	return m_MouseMovementSpeed;
}

float SlicerCamera::GetMouseSensitivity()
{
	return m_MouseSensitivity;
}

float SlicerCamera::GetZoom()
{
	return m_Zoom;
}

// Returns an 6 element array that describes the Camera's X,Y,Z position along with Yaw, Pitch and Roll.
// VNS - Roll(x) then Pitch(y) then Yaw(z)
//void SlicerCamera::GetCameraParameters(float *cameraParams)
//{
//	glm::mat4 view = GetViewMatrix();
//	glm::mat3 viewRot = glm::mat3(view); // Gets the 3x3 rotation matrix from the view matrix
//
//	// NOTE: I Negated the viewRot so that Yaw, Pitch, and Roll are all 0 when the camera is looking in the direction (0,0,1)
//	//		 Also, it "should" be defined the same way as in the VNS.
//	//       I negated the Yaw in the RotationMatrixToAngles matrix to be consistent with VNS.
//	//			Straight down airway has Yaw, Pitch and Roll approximately 0.
//	//		 TO DO: Understand/clarify the difference between the coordinate representation between VNS and my coordinate described in the header.
//	float Yaw, Pitch, Roll;
//	RotationMatrixToAngles(-viewRot, Yaw, Pitch, Roll);
//
//	cameraParams[XCOORD] = m_Position.x;
//	cameraParams[YCOORD] = m_Position.y;
//	cameraParams[ZCOORD] = m_Position.z;
//	cameraParams[ROLLCOORD] = Roll;
//	cameraParams[PITCHCOORD] = Pitch;
//	cameraParams[YAWCOORD] = Yaw;
//}

void SlicerCamera::SetPosition(glm::vec3 cameraPosition)
{
	m_Position = cameraPosition;
}

void SlicerCamera::SetModelOffset(glm::vec3 modelOffset)
{
	m_ModelOffset = modelOffset;
}

void SlicerCamera::SetFront(glm::vec3 cameraFront)
{
	m_Front = cameraFront;
}

void SlicerCamera::SetUp(glm::vec3 cameraUp)
{
	m_Up = cameraUp;
}

void SlicerCamera::SetRight(glm::vec3 cameraRight)
{
	m_Right = cameraRight;
}

void SlicerCamera::SetKeyboardMovementSpeed(float cameraKeyboardMovementSpeed)
{
	m_KeyboardMovementSpeed = cameraKeyboardMovementSpeed;
}

void SlicerCamera::SetMouseMovementSpeed(float cameraMovementSpeed)
{
	m_MouseMovementSpeed = cameraMovementSpeed;
}

void SlicerCamera::SetMouseSensitivity(float cameraMouseSensitivity)
{
	m_MouseSensitivity = cameraMouseSensitivity;
}

void SlicerCamera::SetZoom(float cameraZoom)
{
	m_Zoom = cameraZoom;
}

void SlicerCamera::SetDefault()
{
	m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_ModelOffset = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Front = glm::vec3(0.0f, 0.0f, 1.0f);
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
	m_KeyboardMovementSpeed = 0.5f;
	m_MouseMovementSpeed = 0.5f;
	m_MouseSensitivity = 0.15f;
	m_Zoom = 45.0f;
}

// Processes input received from a mouse input system.
// Expects the offset value in both the x and y direction
// If Shift is held, rotate around viewing (front) direction
void SlicerCamera::ProcessMouseRotation(float xoffset, float yoffset, bool shiftSelect)
{
	xoffset *= m_MouseSensitivity;
	yoffset *= m_MouseSensitivity;

	if (shiftSelect)
	{
		IncrementFrontRotation(xoffset);
	}
	else
	{
		IncrementUpRotation(xoffset);
		IncrementRightRotation(-yoffset);
	}
}

// Processes input received from a mouse input system.
// Expects the offset value in both the Right and Up direction
// If shift is held, only translate in the Front direction
// NOTE - xoffset and yoffset are projected onto the camera's local x,y,z axis.
void SlicerCamera::ProcessMouseTranslation(float xoffset, float yoffset, bool shiftSelect)
{
	xoffset *= m_MouseMovementSpeed;
	yoffset *= m_MouseMovementSpeed;

	if (shiftSelect)
	{
		IncrementFrontTranslation(xoffset);
	}
	else
	{
		IncrementRightTranslation(xoffset);
		IncrementUpTranslation(yoffset);
	}
}

// Processes input received from a mouse scroll-wheel event.
// Only requires input on the vertical wheel-axis
void SlicerCamera::ProcessMouseScroll(float yoffset)
{
	if (m_Zoom >= 1.0f && m_Zoom <= 45.0f)
		m_Zoom -= yoffset;
	if (m_Zoom <= 1.0f)
		m_Zoom = 1.0f;
	if (m_Zoom >= 45.0f)
		m_Zoom = 45.0f;
}

void SlicerCamera::ProcessKeyboardTranslation(Camera_Movement direction)
{
	if (direction == FORWARD)
		m_ModelOffset += m_Front * m_KeyboardMovementSpeed;
	if (direction == BACKWARD)
		m_ModelOffset -= m_Front * m_KeyboardMovementSpeed;
	if (direction == LEFT)
		m_ModelOffset += m_Right * m_KeyboardMovementSpeed;
	if (direction == RIGHT)
		m_ModelOffset -= m_Right * m_KeyboardMovementSpeed;
	if (direction == UP)
		m_ModelOffset += m_Up * m_KeyboardMovementSpeed;
	if (direction == DOWN)
		m_ModelOffset -= m_Up * m_KeyboardMovementSpeed;
}

// Rotate all Camera Vectors based on Rotation about Camera's Front Vector
void SlicerCamera::IncrementFrontRotation(float inc)
{
	float radInc = glm::radians(inc);

	// Generate rotation matrix about Camera Front
	frontQuat = glm::quat(glm::cos(radInc / 2), m_Front.x*glm::sin(radInc / 2), m_Front.y*glm::sin(radInc / 2), m_Front.z*glm::sin(radInc / 2));
	glm::mat3 quatMat3 = glm::mat3_cast(frontQuat);

	// Rotate Camera Vectors based from increment 
	m_Right = glm::normalize(quatMat3 * m_Right);
	m_Up = glm::normalize(quatMat3 * m_Up);
	m_Front = glm::normalize(quatMat3 * m_Front);
}

// Rotate all Camera Vectors based on Rotation about Camera's Right Vector
void SlicerCamera::IncrementRightRotation(float inc)
{
	float radInc = glm::radians(inc);

	// Generate rotation matrix about Camera Right
	rightQuat = glm::quat(glm::cos(radInc / 2), m_Right.x*glm::sin(radInc / 2), m_Right.y*glm::sin(radInc / 2), m_Right.z*glm::sin(radInc / 2));
	glm::mat3 quatMat3 = glm::mat3_cast(rightQuat);

	// Rotate Camera Vectors based from increment 
	m_Right = glm::normalize(quatMat3 * m_Right);
	m_Up = glm::normalize(quatMat3 * m_Up);
	m_Front = glm::normalize(quatMat3 * m_Front);
}

// Rotate all Camera Vectors based on Rotation about Camera's Up Vector
void SlicerCamera::IncrementUpRotation(float inc)
{
	float radInc = glm::radians(inc);

	// Generate rotation matrix about Camera Up
	upQuat = glm::quat(glm::cos(radInc / 2), m_Up.x*glm::sin(radInc / 2), m_Up.y*glm::sin(radInc / 2), m_Up.z*glm::sin(radInc / 2));
	glm::mat3 quatMat3 = glm::mat3_cast(upQuat);

	// Rotate Camera Vectors about Camera Up
	m_Right = glm::normalize(quatMat3 * m_Right);
	m_Up = glm::normalize(quatMat3 * m_Up);
	m_Front = glm::normalize(quatMat3 * m_Front);
}

void SlicerCamera::IncrementRightTranslation(float inc)
{
	glm::vec3 rightOffset = m_Right * inc;
	m_ModelOffset += rightOffset;
}

void SlicerCamera::IncrementUpTranslation(float inc)
{
	glm::vec3 upOffset = m_Up * inc;
	m_ModelOffset += upOffset;
}

void SlicerCamera::IncrementFrontTranslation(float inc)
{
	glm::vec3 frontOffset = m_Front * inc;
	m_ModelOffset += frontOffset;
}

//	FROM PAPER: "Computing Euler angles from a rotation matrix" by Gregory G. Slabaugh
void SlicerCamera::RotationMatrixToAngles(glm::mat3 rotMatrix, float &Yaw, float &Pitch, float &Roll)
{
	float yaw, pitch, roll;
	if (rotMatrix[2][0] != -1.0f && rotMatrix[2][0] != 1.0f)
	{
		// Change:::I Added negative sign to yaw
		pitch = -std::asin(rotMatrix[2][0]);
		roll = std::atan2(rotMatrix[2][1] / std::cos(pitch), rotMatrix[2][2] / std::cos(pitch));
		yaw = -std::atan2(rotMatrix[1][0] / std::cos(pitch), rotMatrix[0][0] / std::cos(pitch));
	}
	else
	{
		yaw = 0.0f;
		if (rotMatrix[2][0] == -1.0f)
		{
			pitch = PI / 2.0f;
			roll = yaw + std::atan2(rotMatrix[0][1], rotMatrix[0][2]);
		}
		else
		{
			pitch = -PI / 2.0f;
			roll = -yaw + std::atan2(-rotMatrix[0][1], -rotMatrix[0][2]);
		}
	}
	// convert to degrees
	Yaw = glm::degrees(yaw);
	Pitch = glm::degrees(pitch);
	Roll = glm::degrees(roll);
}

void SlicerCamera::CheckCameraVectors()
{
	bool perpendicularFlag1 = true;
	bool perpendicularFlag2 = true;
	bool perpendicularFlag3 = true;

	float UpDotFront = glm::dot(m_Up, m_Front);
	float UpDotRight = glm::dot(m_Up, m_Right);
	float RightDotFront = glm::dot(m_Right, m_Front);

	// Set a flag to false if any of the dot products are not 0 
	if (UpDotFront)
	{
		perpendicularFlag1 = false;
	}
	if (UpDotRight)
	{
		perpendicularFlag2 = false;
	}
	if (RightDotFront)
	{
		perpendicularFlag3 = false;
	}

	if (!perpendicularFlag1 || !perpendicularFlag2 || !perpendicularFlag3)
		qDebug() << "WARNING::Camera Right, Up and Front vectors are not all perpendicular.";
}

void SlicerCamera::FrontRotation(float deg)
{
	IncrementFrontRotation(deg);
}

void SlicerCamera::RightRotation(float deg)
{
	IncrementRightRotation(deg);
}

void SlicerCamera::UpRotation(float deg)
{
	IncrementUpRotation(deg);
}

glm::mat3 SlicerCamera::GetRotationMatrix()
{
	glm::mat3 rotMat = glm::mat3(1.0f);
	rotMat[0] = m_Right;
	rotMat[1] = m_Up;
	rotMat[2] = m_Front;
	return rotMat;
}
