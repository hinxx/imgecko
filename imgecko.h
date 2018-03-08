/*
 * imgecko.h
 *
 *  Created on: Mar 7, 2018
 *      Author: hinxx
 */

#ifndef IMGECKO_H_
#define IMGECKO_H_

#include <stdint.h>

enum FTDICommand {
	CMD_ResultError,
	CMD_FatalError,
	CMD_OK
};

enum WiiStatus {
	WiiStatusRunning,
	WiiStatusPaused,
	WiiStatusBreakpoint,
	WiiStatusLoader,
	WiiStatusUnknown
};

enum WiiLanguage {
	NoOverride,
	Japanese,
	English,
	German,
	French,
	Spanish,
	Italian,
	Dutch,
	ChineseSimplified,
	ChineseTraditional,
	Korean
};

enum WiiPatches {
	NoPatches,
	PAL60,
	VIDTV,
	PAL60VIDTV,
	NTSC,
	NTSCVIDTV,
	PAL50,
	PAL50VIDTV
};

enum WiiHookType {
	VI,
	WiiRemote,
	GamecubePad
};


bool Connect();
void Disconnect();
bool InitGecko();
WiiStatus GeckoStatus();

FTDICommand GeckoWrite(uint8_t *sendbyte, uint32_t nobytes);
FTDICommand GeckoRead(uint8_t *recbyte, uint32_t nobytes);


#endif /* IMGECKO_H_ */
