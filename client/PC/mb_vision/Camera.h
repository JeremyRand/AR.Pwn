#ifndef MB_VISION_CAMERA_H
#define MB_VISION_CAMERA_H

#include <string>
#include <stdint.h>

// format is currently hard-coded to RGB24

class Camera {
	public:
		Camera(int width=160, int height=120);
		~Camera();
		
		int getFd() { return fd; }
		void close();
		void readImage(uint8_t *buffer);
		
		int getWidth() const { return width; }
		int getHeight() const { return height; }
		
	private:
		int width, height;
		int fd;
};

#endif
