#include "Camera.h"
#include <string>
#include <stdexcept>
#include <cstring>
#include <errno.h>

using namespace std;

static string stringerr();

Camera::Camera(int width, int height) : width(width), height(height) {
}

Camera::~Camera() {
	close();
}

void Camera::close() {
}

void Camera::readImage(uint8_t *buffer) {

}

static string stringerr() {
	return string(strerror(errno));
}

