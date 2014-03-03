//! @author Guillaume Jacob <gjacob@aldebaran-robotics.com>, 2014

#include <iostream>
#include <OpenNI.h>
#include <string>
#include <sys/time.h>

#include "OniSampleUtilities.h"

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

int main(int argc, char * argv[])
{
  std::string path("recording.oni");
  int duration = -1;
  if(argc == 2)
  {
    duration = 1000*atoi(argv[1]); // convert to ms
  }

  if(duration > 0)
  {
    std::cout << "Will start a recording in " << path
              << " of " << duration/1000 << " seconds" << std::endl;
  }
  else
  {
    std::cout << "Will start a recording in " << path << " until keyboard hit" << std::endl;
  }

  // 1 - Initialize OpenNI
  openni::Status rc = openni::OpenNI::initialize();
  if (rc != openni::STATUS_OK)
  {
    std::cerr << "Initialize failed: "
              << openni::OpenNI::getExtendedError() << std::endl;
    return 1;
  }

  // 2 - Open device
  openni::Device device;
  rc = device.open(openni::ANY_DEVICE);
  if (rc != openni::STATUS_OK)
  {
    std::cerr << "Couldn't open device: "
              << openni::OpenNI::getExtendedError() << std::endl;
    return 2;
  }

  // 3 - Create stream
  openni::VideoStream depth;
  if (device.getSensorInfo(openni::SENSOR_DEPTH) != NULL)
  {
    rc = depth.create(device, openni::SENSOR_DEPTH);
    if (rc != openni::STATUS_OK)
    {
      std::cerr << "Couldn't create depth stream: "
                << openni::OpenNI::getExtendedError() << std::endl;
      return 3;
    }
  }

  // 4 - Create recording
  openni::Recorder rec;
  rec.create(path.c_str());
  rec.attach(depth, false);

  // 5 - Start stream
  rc = depth.start();
  if (rc != openni::STATUS_OK)
  {
    std::cerr << "Couldn't start the depth stream: "
              << openni::OpenNI::getExtendedError() << std::endl;
    return 4;
  }

  // 6 - Start recording
  rec.start();

  timeval tv_init;
  gettimeofday(&tv_init, 0);

  bool running = true;
  int recordingTime = 0; // in milliseconds
  while(running)
  {
    Sleep(100); // in milliseconds

    timeval tv;
    gettimeofday(&tv, 0);
    recordingTime = (tv.tv_sec-tv_init.tv_sec)*1000+(tv.tv_usec-tv_init.tv_usec)/1000;

    // stop condition
    if(duration > 0 && recordingTime >= duration)
      running = false;
    if(duration <= 0 && wasKeyboardHit())
      running = false;
  }

  std::cout << "Recording stopped (" << recordingTime << " ms recorded)" << std::endl;


  // 7 - Stop recording
  rec.stop();

  // 8 - Cleanup
  rec.destroy();
  depth.stop();
	depth.destroy();
	device.close();
	openni::OpenNI::shutdown();

	return 0;
}
