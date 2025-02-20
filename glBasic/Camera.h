#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

enum Camera_Movement {
	FORWARD, BACKWARD,
	LEFT, RIGHT,
	UP, DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(Position, Position + Front, Up);
	}

	glm::mat4 GetProjectionMatrix(uint32_t width, uint32_t height, float near = 0.1f, float far = 100.0f) {
		return glm::perspective(glm::radians(Zoom), (float)width / height, near, far);
	}

	glm::vec2 GetGrid(uint32_t width, uint32_t height) const {
		double d = height/2.0/tan(glm::radians(ZOOM/2));
		return (glm::vec2(width, height) / (float)d);
	}

	glm::vec3 GetDirection(glm::vec2 pos, uint32_t width, uint32_t height) const {
		pos.x/=width;
		pos.y=1-pos.y/height;
		glm::vec2 grid = GetGrid(width, height);
		glm::vec3 direction = glm::normalize(Front+(pos.x-0.5f)*grid.x*Right+(pos.y-0.5f)*grid.y*Up);
		return direction;
	}
	glm::vec3 GetPointPosition(glm::vec2 pos, float dist, uint32_t width, uint32_t height) const {
		glm::vec3 direction = GetDirection(pos, width, height);
		dist=dist/glm::dot(direction,Front); 
		return Position + direction * dist;
	}
	void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD) Position += Front * velocity;
		if (direction == BACKWARD)Position -= Front * velocity;
		if (direction == LEFT)Position -= Right * velocity;
		if (direction == RIGHT)Position += Right * velocity;
		if (direction == UP)Position += Up * velocity;
		if (direction == DOWN)Position -= Up * velocity;

	}

	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)Pitch = 89.0f;
			if (Pitch < -89.0f)Pitch = -89.0f;
		}

		updateCameraVectors();

	}

	void ProcessMouseScroll(float yoffset) {
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)Zoom = 1.0f;
		if (Zoom > 45.0f)Zoom = 45.0f;
	}

private:
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
