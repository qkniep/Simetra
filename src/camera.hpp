#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>


class Camera {
	GLfloat xPos, yPos, zPos;
	GLfloat fov, aspectRatio;
	glm::mat4 m, v, p, mvp;


public:
	Camera(GLfloat x, GLfloat y, GLfloat z);
	void move(GLfloat dx, GLfloat dy, GLfloat dz);
	glm::mat4 getMVP();
	GLfloat getZ();
	void setAspectRatio(GLfloat ratio);

private:
	void update();
};

#endif  // CAMERA_HPP
