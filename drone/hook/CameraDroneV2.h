/*

AR.Pwn Hook Program
Copyright 2015, Team SNARC / VECLabs
Defines camera specific date for ArDroneV2

*/
static const char * const CAM1_DEVICE = "/dev/video1";

static const size_t CAM1_WIDTH = 1280;
static const size_t CAM1_HEIGHT = 720;

static const char * const CAM2_DEVICE = "/dev/video2";

static const size_t CAM2_WIDTH = 320;
static const size_t CAM2_HEIGHT = 240;

/** AR Drone v2 uses YUV images with two channel */
const int getBufferSize(size_t width, size_t height)
{
    return width * height * 2;
}
