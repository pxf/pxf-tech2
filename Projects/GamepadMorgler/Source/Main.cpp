#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Random.h>
#include <Pxf/Base/Memory.h>

extern "C" {
#include <gamepad/Gamepad.h>
}

#include <ctime>

using namespace Pxf;

bool verbose = true;

bool on_button_down(void * sender, const char * eventID, void * eventData, void * context) {
	struct Gamepad_buttonEvent * event = (Gamepad_buttonEvent*)eventData;
	if (verbose) {
		Message("unk", "Button %u down (%d) on device %u at %f\n", event->buttonID, (int) event->down, event->device->deviceID, event->timestamp);
	}
	return true;
}

bool on_button_up(void * sender, const char * eventID, void * eventData, void * context) {
	struct Gamepad_buttonEvent * event = (Gamepad_buttonEvent*)eventData;
	if (verbose) {
		Message("unk", "Button %u up (%d) on device %u at %f\n", event->buttonID, (int) event->down, event->device->deviceID, event->timestamp);
	}
	return true;
}

bool on_axis_moved(void * sender, const char * eventID, void * eventData, void * context) {
	struct Gamepad_axisEvent * event = (Gamepad_axisEvent*)eventData;
	if (verbose) {
		Message("unk", "Axis %u moved to %f on device %u at %f\n", event->axisID, event->value, event->device->deviceID, event->timestamp);
	}
	return true;
}

bool on_device_attached(void * sender, const char * eventID, void * eventData, void * context) {
	struct Gamepad_device * device = (Gamepad_device*)eventData;
	if (verbose) {
		Message("unk", "Device ID %u attached (vendor = 0x%X; product = 0x%X)\n", device->deviceID, device->vendorID, device->productID);
	}
	device->eventDispatcher->registerForEvent(device->eventDispatcher, GAMEPAD_EVENT_BUTTON_DOWN, on_button_down, device);
	device->eventDispatcher->registerForEvent(device->eventDispatcher, GAMEPAD_EVENT_BUTTON_UP, on_button_up, device);
	device->eventDispatcher->registerForEvent(device->eventDispatcher, GAMEPAD_EVENT_AXIS_MOVED, on_axis_moved, device);
	return true;
}

bool on_device_removed(void * sender, const char * eventID, void * eventData, void * context) {
	struct Gamepad_device * device = (Gamepad_device*)eventData;
	if (verbose) {
		Message("unk", "Device ID %u removed\n", device->deviceID);
	}
	return true;
}

int main(int argc, char* argv[])
{
	Kernel* kernel = Kernel::GetInstance();
	unsigned controller = kernel->CreateTag("gamepad");


	Gamepad_eventDispatcher()->registerForEvent(Gamepad_eventDispatcher(), GAMEPAD_EVENT_DEVICE_ATTACHED, on_device_attached, NULL);
	Gamepad_eventDispatcher()->registerForEvent(Gamepad_eventDispatcher(), GAMEPAD_EVENT_DEVICE_REMOVED, on_device_removed, NULL);
	Gamepad_init();
	
	unsigned int num_devices = Gamepad_numDevices();
	
	/* Poll for new devices */
	Gamepad_detectDevices();
	
	Gamepad_device* devinfo = 0;
	for(int i = 0; i < num_devices; i++)
	{
		devinfo = Gamepad_deviceAtIndex(i);
		kernel->Log(controller|Logger::IS_DEBUG, "id: %d, deviceID: %d, description: %s\n"
												 "vendorID: %d, productID: %d, numAxes: %d, numButtons: %d\n"
												 , i
												 , devinfo->deviceID
												 , devinfo->description
												 , devinfo->vendorID
												 , devinfo->productID
												 , devinfo->numAxes
												 , devinfo->numButtons);
												 
		for(int i = 0; i < devinfo->numAxes; i ++)
			kernel->Log(controller|Logger::IS_DEBUG, "state axis %d: %.4f", i, devinfo->axisStates[i]);
			
		for(int i = 0; i < devinfo->numButtons; i ++)
			kernel->Log(controller|Logger::IS_DEBUG, "state button %d: %d", i, devinfo->buttonStates[i]);
	}
	
	while(true)
	{
		Gamepad_processEvents();
	}
	
	Gamepad_shutdown();
	return 0;
}

