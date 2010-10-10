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

#define MIDI_CLOCK      0xf8
#define MIDI_ACTIVE     0xfe
#define MIDI_STATUS_MASK 0x80
#define MIDI_SYSEX      0xf0
#define MIDI_EOX        0xf7
#define MIDI_START      0xFA
#define MIDI_STOP       0xFC
#define MIDI_CONTINUE   0xFB
#define MIDI_F9         0xF9
#define MIDI_FD         0xFD
#define MIDI_RESET      0xFF
#define MIDI_NOTE_ON    0x90
#define MIDI_NOTE_OFF   0x80
#define MIDI_CHANNEL_AT 0xD0
#define MIDI_POLY_AT    0xA0
#define MIDI_PROGRAM    0xC0
#define MIDI_CONTROL    0xB0
#define MIDI_PITCHBEND  0xE0
#define MIDI_MTC        0xF1
#define MIDI_SONGPOS    0xF2
#define MIDI_SONGSEL    0xF3
#define MIDI_TUNE       0xF6

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
	
	Pm_SetFilter(input_stream, PM_FILT_SYSTEMCOMMON);
	
	PmEvent events[64];
	while(true)
	{
		int num = Pm_Read(input_stream, events, 64);
		for(int i = 0; i < num; i++)
		{
			PmMessage message = events[i].message;
			PmTimestamp timestamp = events[i].timestamp;
			unsigned char status = Pm_MessageStatus(message);
			unsigned char data1 = Pm_MessageData1(message);
			unsigned char data2 = Pm_MessageData2(message);
			kernel->Log(midi|Logger::IS_DEBUG, "[%dms] status: 0x%x, data1: %d, data2: %d", timestamp, status, data1, data2);
		}
	}
	
	Pm_Terminate();
	return 0;
}

