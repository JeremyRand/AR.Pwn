/*

AR.Pwn Hook Program
Copyright 2015, Team SNARC / VECLabs
Defines camera specific date for ArDroneV1

*/
static const char * const CAM1_DEVICE = "/dev/video0";

static const size_t CAM1_WIDTH = 640;
static const size_t CAM1_HEIGHT = 480;

static const char * const CAM2_DEVICE = "/dev/video1";

static const size_t CAM2_WIDTH = 176;
static const size_t CAM2_HEIGHT = 144;

const size_t getBufferSize(size_t width, size_t height)
{
    return width * height * 3 / 2;
}

