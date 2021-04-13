#include "midi_helper.hpp"

MidiHelper::MidiHelper() {
	try {
		midiIn = new RtMidiIn();
		midiOut = new RtMidiOut();
	} catch (RtMidiError &err) {
		err.printMessage();
		exit(1);
	}
}
MidiHelper::~MidiHelper() {
	if (midiIn->isPortOpen()) {
		midiIn->closePort();
		delete midiIn;
	}
	if (midiOut->isPortOpen()) {
		midiOut->closePort();
		delete midiOut;
	}
}


MidiEvent MidiHelper::getMessage() {
	std::vector<unsigned char> data;
	double timestamp = midiIn->getMessage(&data);

	try {
		MidiEvent event = MidiEvent(timestamp, data);
		return event;
	} catch (std::runtime_error &err) {
		return MidiEvent();
	}

}

//* Retrieve list of devices *//
std::vector<std::string> MidiHelper::getInputDevices() { return getDevices(midiIn); }
std::vector<std::string> MidiHelper::getOutputDevices() { return getDevices(midiOut); }
std::vector<std::string> MidiHelper::getDevices(RtMidi *midi) {
	int port_count = midi->getPortCount();
	std::vector<std::string> devices;

	for (int i = 0; i < port_count; i++) {
		try {
			devices.push_back( midi->getPortName(i) );
		} catch (RtMidiError &err) {
			err.printMessage();
			exit(1);
		}
	}

	return devices;
}


//* Set an input/output device *//
void MidiHelper::setOutputDevice(int port_id) { return setDevice(midiOut, port_id); }
std::string MidiHelper::setInputDevice(int port_id) { 
	setDevice(midiIn, port_id);
	std::string device_name = midiIn->getPortName(port_id);

	for (int i = 0; i < midiOut->getPortCount(); i++) {
		if (device_name.compare(midiOut->getPortName(i)) == 0) {
			setOutputDevice(i);
		}
	}

	return device_name;
}

void MidiHelper::setDevice(RtMidi *midi, int &port_id) {
	if (port_id >= midi->getPortCount() || port_id < 0) {
		throw std::range_error("Port number out of range!");
	}
	midi->openPort(port_id);
}

//* Get number of devices *//
int MidiHelper::getOutputPortCount() { return midiOut->getPortCount(); }
int MidiHelper::getInputPortCount() { return midiIn->getPortCount(); }

//* Get the id of a device based on its name *//
int MidiHelper::getInputDeviceId(std::string device_name) { return getDeviceId(midiIn, device_name); }
int MidiHelper::getOutputDeviceId(std::string device_name) { return getDeviceId(midiOut, device_name); }
int MidiHelper::getDeviceId(RtMidi *midi, std::string device_name) {
	for (int i = 0; i < midi->getPortCount(); i++) {
		if (device_name.compare(midi->getPortName(i)) == 0) {
			return i;
		}
	}
	return -1;
}


void MidiHelper::sendFeedback(const MidiEvent &event) {
	std::vector<unsigned char> msg = event.getFeedback();
	midiOut->sendMessage(&msg);
}