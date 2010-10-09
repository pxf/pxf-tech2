#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Random.h>
#include <Pxf/Base/Memory.h>

extern "C" {
#include <portmidi.h>
}

#include <ctime>

using namespace Pxf;

int main(int argc, char* argv[])
{
	Kernel* kernel = Kernel::GetInstance();
	unsigned midi = kernel->CreateTag("midi");

	Pm_Initialize();
	
	int num_devices = Pm_CountDevices();
	PmDeviceID default_input = Pm_GetDefaultInputDeviceID();
	PmDeviceID default_output = Pm_GetDefaultOutputDeviceID();
	
	kernel->Log(midi|Logger::IS_INFORMATION, "Number of devices: %d", num_devices);
	kernel->Log(midi|Logger::IS_INFORMATION, "Default Input Device: %d", default_input);
	kernel->Log(midi|Logger::IS_INFORMATION, "Default Output Device: %d", default_output);
	
	const PmDeviceInfo* devinfo = 0;
	for(int i = 0; i < num_devices; i++)
	{
		devinfo = Pm_GetDeviceInfo(i);
		kernel->Log(midi|Logger::IS_INFORMATION, "%d. if: '%s', id: '%s', inp: %d, out: %d"
					, i
					, devinfo->interf
					, devinfo->name
					, devinfo->input
					, devinfo->output);
	}
	
	PmDeviceID my_oxygen_25_id = 2;
	
	PortMidiStream* input_stream;
	PmError err = Pm_OpenInput(&input_stream    /* Store pointer to input stream here */
						, my_oxygen_25_id /* PmDeviceID */
						, NULL            /* inputDriverInfo - Not used */
						, 64              /* Buffer size */
						, NULL            /* time_proc - null for default*/
						, NULL );         /* timeinfo */
	
	if (err)
	{
		char error_text[4096];
		Pm_GetHostErrorText(error_text, 4095);
		kernel->Log(midi|Logger::IS_CRITICAL, "Error opening stream: %s", Pm_GetErrorText(err));
		kernel->Log(midi|Logger::IS_CRITICAL, "Error opening stream: %s", error_text);
		Pm_Terminate();
		return 0;
	}
	
	
	PmEvent events[64];
	while(true)
	{
		int num = Pm_Read(input_stream, events, 64);
		for(int i = 0; i < num; i++)
		{
			PmMessage message = events[i].message;
			PmTimestamp timestamp = events[i].timestamp;
			kernel->Log(midi|Logger::IS_DEBUG, "msg: %d, ms: %d", message, timestamp);
		}
	}
	
	Pm_Terminate();
	return 0;
}

