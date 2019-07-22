/* Shared Use License: This file is owned by Derivative Inc. (Derivative) and
 * can only be used, and/or modified for use, in conjunction with 
 * Derivative's TouchDesigner software, and only if you are a licensee who has
 * accepted Derivative's TouchDesigner license or assignment agreement (which
 * also govern the use of this file).  You may share a modified version of this
 * file with another authorized licensee of Derivative's TouchDesigner software.
 * Otherwise, no redistribution or sharing of this file, with or without
 * modification, is permitted.
 */

#include "CPlusPlusCHOPExample.h"


#pragma once
//#include <stdio.h>
//#include <string.h>
//#include <cmath>
//#include <assert.h>
//#include "../include/CUESDK.h"
//#include "../include/CUESDKGlobal.h"
//
//#include "../include/CorsairLedIdEnum.h"
//#include "../include/CorsairKeyIdEnum.h"
//
//#include <vector>
//#include <unordered_map>
//

#include <iostream>
using LedColorsVector = std::vector<CorsairLedColor>;
using AvailableKeys = std::unordered_map<int /*device index*/, LedColorsVector>;



// These functions are basic C function, which the DLL loader can find
// much easier than finding a C++ Class.
// The DLLEXPORT prefix is needed so the compile exports these functions from the .dll
// you are creating
extern "C"
{

DLLEXPORT
void
FillCHOPPluginInfo(CHOP_PluginInfo *info)
{
	// Always set this to CHOPCPlusPlusAPIVersion.
	info->apiVersion = CHOPCPlusPlusAPIVersion;

	// The opType is the unique name for this CHOP. It must start with a 
	// capital A-Z character, and all the following characters must lower case
	// or numbers (a-z, 0-9)
	info->customOPInfo.opType->setString("Customsignal");

	// The opLabel is the text that will show up in the OP Create Dialog
	info->customOPInfo.opLabel->setString("Custom Signal");

	// Information about the author of this OP
	info->customOPInfo.authorName->setString("Author Name");
	info->customOPInfo.authorEmail->setString("email@email.com");

	// This CHOP can work with 0 inputs
	info->customOPInfo.minInputs = 0;

	// It can accept up to 1 input though, which changes it's behavior
	info->customOPInfo.maxInputs = 1;
}

DLLEXPORT
CHOP_CPlusPlusBase*
CreateCHOPInstance(const OP_NodeInfo* info)
{
	// Return a new instance of your class every time this is called.
	// It will be called once per CHOP that is using the .dll
	return new CPlusPlusCHOPExample(info);
}

DLLEXPORT
void
DestroyCHOPInstance(CHOP_CPlusPlusBase* instance)
{
	// Delete the instance here, this will be called when
	// Touch is shutting down, when the CHOP using that instance is deleted, or
	// if the CHOP loads a different DLL
	delete (CPlusPlusCHOPExample*)instance;
}

};




CPlusPlusCHOPExample::CPlusPlusCHOPExample(const OP_NodeInfo* info) : myNodeInfo(info)
{
	CorsairPerformProtocolHandshake();
	

}

CPlusPlusCHOPExample::~CPlusPlusCHOPExample()
{
}

const char*
CPlusPlusCHOPExample::toString(CorsairError error)
{
	switch (error) {
	case CE_Success:
		return "CE_Success";
	case CE_ServerNotFound:
		return "CE_ServerNotFound";
	case CE_NoControl:
		return "CE_NoControl";
	case CE_ProtocolHandshakeMissing:
		return "CE_ProtocolHandshakeMissing";
	case CE_IncompatibleProtocol:
		return "CE_IncompatibleProtocol";
	case CE_InvalidArguments:
		return "CE_InvalidArguments";
	default:
		return "unknown error";
	}
}

AvailableKeys
CPlusPlusCHOPExample:: getAvailableKeys()
{
	AvailableKeys availableKeys;
	for (auto deviceIndex = 0; deviceIndex < CorsairGetDeviceCount(); deviceIndex++) {
		if (const auto ledPositions = CorsairGetLedPositionsByDeviceIndex(deviceIndex)) {
			LedColorsVector keys;
			for (auto i = 0; i < ledPositions->numberOfLed; i++) {
				const auto ledId = ledPositions->pLedPosition[i].ledId;
				keys.push_back(CorsairLedColor{ ledId, 0, 0, 0 });
			}
			availableKeys[deviceIndex] = keys;
		}
	}
	return availableKeys;
}

//void
//CPlusPlusCHOPExample::performLighting(AvailableKeys &availableKeys, std::vector<int> tdColor)
//{
//	const auto timePerFrame = 25;
//	for (auto x = .0; x < 2; x += static_cast<double>(timePerFrame) / 200.) {
//		auto val = static_cast<int>((1 - pow(x - 1, 2)) * 255);
//		for (auto &ledColorsByDeviceIndex : availableKeys) {
//			auto &deviceIndex = ledColorsByDeviceIndex.first;
//			auto &ledColorsVec = ledColorsByDeviceIndex.second;
//			for (auto &ledColor : ledColorsVec) {
//				ledColor.r = tdColor[0];
//				ledColor.g = tdColor[1];
//				ledColor.b = tdColor[2];
//			}
//			CorsairSetLedsColorsBufferByDeviceIndex(deviceIndex, static_cast<int>(ledColorsVec.size()), ledColorsVec.data());
//		}
//		CorsairSetLedsColorsFlushBufferAsync(nullptr, nullptr);
//	}
//}

void
CPlusPlusCHOPExample::performLighting(AvailableKeys &availableKeys, std::vector<int> tdColor)
{
	for (auto &ledColorsByDeviceIndex : availableKeys) {
		auto &deviceIndex = ledColorsByDeviceIndex.first;
		auto &ledColorsVec = ledColorsByDeviceIndex.second;
		for (auto &ledColor : ledColorsVec) {
			ledColor.r = tdColor[0];
			ledColor.g = tdColor[1];
			ledColor.b = tdColor[2];
		}
		CorsairSetLedsColorsBufferByDeviceIndex(deviceIndex, static_cast<int>(ledColorsVec.size()), ledColorsVec.data());
	}
	CorsairSetLedsColorsFlushBufferAsync(nullptr, nullptr);
}


void
CPlusPlusCHOPExample::getGeneralInfo(CHOP_GeneralInfo* ginfo, const OP_Inputs* inputs, void* reserved1)
{
	// This will cause the node to cook every frame
	ginfo->cookEveryFrameIfAsked = false;

	// Note: To disable timeslicing you'll need to turn this off, as well as ensure that
	// getOutputInfo() returns true, and likely also set the info->numSamples to how many
	// samples you want to generate for this CHOP. Otherwise it'll take on length of the
	// input CHOP, which may be timesliced.
	ginfo->timeslice = false;


	ginfo->inputMatchIndex = 0;
}

bool
CPlusPlusCHOPExample::getOutputInfo(CHOP_OutputInfo* info, const OP_Inputs* inputs, void* reserved1)
{

	info->numChannels = 3;

	// Since we are outputting a timeslice, the system will dictate
	// the numSamples and startIndex of the CHOP data
	//info->numSamples = 1;
	//info->startIndex = 0

	// For illustration we are going to output 120hz data
	info->sampleRate = 120;
	return true;
	
}

void
CPlusPlusCHOPExample::getChannelName(int32_t index, OP_String *name, const OP_Inputs* inputs, void* reserved1)
{
	name->setString("chan1");
}



void
CPlusPlusCHOPExample::execute(CHOP_Output* output,
							  const OP_Inputs* inputs,
							  void* reserved)
{

	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << "\nPress any key tro quit." << std::endl;
		getchar();
		return;
	}

	auto availableKeys = getAvailableKeys();

	if (availableKeys.empty()) {
		std::cout << "No available keys";
		return;
	}
	
	const OP_CHOPInput *cinput = inputs->getInputCHOP(0);	
	
	std::vector<int> tdColorVec(3);							//a vector to hold our color-this is sent to the LEDs

	// Retrieve input channel data and output it scaled to 0-255
	// place the scaled input into the tdColorVec Vector

	for (int i = 0; i < cinput->numChannels-1; i++) {
		
		tdColorVec[i] = int(cinput->getChannelData(i)[0]*255);
		output->channels[i][0] = cinput->getChannelData(i)[0]*255;
	
	};

	// This function sends the tdColorVec to all the LEDs of the keyboard
	performLighting(availableKeys, tdColorVec);
}

int32_t
CPlusPlusCHOPExample::getNumInfoCHOPChans(void * reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 2;
}

void
CPlusPlusCHOPExample::getInfoCHOPChan(int32_t index,
										OP_InfoCHOPChan* chan,
										void* reserved1)
{
	// This function will be called once for each channel we said we'd want to return
	// In this example it'll only be called once.

	if (index == 0)
	{
		chan->name->setString("executeCount");
		chan->value = (float)myExecuteCount;
	}

	if (index == 1)
	{
		chan->name->setString("offset");
		chan->value = (float)myOffset;
	}
}

bool		
CPlusPlusCHOPExample::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
	infoSize->rows = 2;
	infoSize->cols = 2;
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void
CPlusPlusCHOPExample::getInfoDATEntries(int32_t index,
										int32_t nEntries,
										OP_InfoDATEntries* entries, 
										void* reserved1)
{
	char tempBuffer[4096];

	if (index == 0)
	{
		// Set the value for the first column
		entries->values[0]->setString("executeCount");

		// Set the value for the second column
#ifdef _WIN32
		sprintf_s(tempBuffer, "%d", myExecuteCount);
#else // macOS
        snprintf(tempBuffer, sizeof(tempBuffer), "%d", myExecuteCount);
#endif
		entries->values[1]->setString(tempBuffer);
	}

	if (index == 1)
	{
		// Set the value for the first column
		entries->values[0]->setString("offset");

		// Set the value for the second column
#ifdef _WIN32
        sprintf_s(tempBuffer, "%g", myOffset);
#else // macOS
        snprintf(tempBuffer, sizeof(tempBuffer), "%g", myOffset);
#endif
		entries->values[1]->setString( tempBuffer);
	}
}

void
CPlusPlusCHOPExample::setupParameters(OP_ParameterManager* manager, void *reserved1)
{
	// speed
	{
		OP_NumericParameter	np;

		np.name = "Speed";
		np.label = "Speed";
		np.defaultValues[0] = 1.0;
		np.minSliders[0] = -10.0;
		np.maxSliders[0] =  10.0;
		
		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// scale
	{
		OP_NumericParameter	np;

		np.name = "Scale";
		np.label = "Scale";
		np.defaultValues[0] = 1.0;
		np.minSliders[0] = -10.0;
		np.maxSliders[0] =  10.0;
		
		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// shape
	{
		OP_StringParameter	sp;

		sp.name = "Shape";
		sp.label = "Shape";

		sp.defaultValue = "Sine";

		const char *names[] = { "Sine", "Square", "Ramp" };
		const char *labels[] = { "Sine", "Square", "Ramp" };

		OP_ParAppendResult res = manager->appendMenu(sp, 3, names, labels);
		assert(res == OP_ParAppendResult::Success);
	}

	// pulse
	{
		OP_NumericParameter	np;

		np.name = "Reset";
		np.label = "Reset";
		
		OP_ParAppendResult res = manager->appendPulse(np);
		assert(res == OP_ParAppendResult::Success);
	}

}

void 
CPlusPlusCHOPExample::pulsePressed(const char* name, void* reserved1)
{
	if (!strcmp(name, "Reset"))
	{
		myOffset = 0.0;
	}
}

