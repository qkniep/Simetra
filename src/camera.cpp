#include "camera.hpp"

#include <glm/gtc/type_ptr.hpp>


Camera::Camera(GLfloat x, GLfloat y, GLfloat z)
		: xPos(x), yPos(y), zPos(z)
		, fov(45.f), aspectRatio(16.f/9.f) {
	update();
}


void Camera::move(GLfloat dx, GLfloat dy, GLfloat dz) {
	xPos += dx;
	yPos += dy;
	zPos += dz;
	update();
}


glm::mat4 Camera::getMVP() {
	return mvp;
}


GLfloat Camera::getZ() {
	return zPos;
}


void Camera::setAspectRatio(GLfloat ratio) {
	aspectRatio = ratio;
	update();
}


void Camera::update() {
	m = glm::mat4(1);
	v = glm::lookAtLH(
		glm::vec3(xPos, yPos, zPos),               // camera position
		glm::vec3(xPos, yPos - .25f, zPos + 1.f),  // position to look at
		glm::vec3(0, 1, 0)                         // up direction
	);
	p = glm::perspectiveLH(glm::radians(fov), aspectRatio, .1f, 1024.f);
	mvp = p * v * m;
}
