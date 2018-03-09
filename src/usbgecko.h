/*
 * usbgecko.h
 *
 *  Created on: Mar 7, 2018
 *      Author: hinxx
 */

#ifndef USBGECKO_H_
#define USBGECKO_H_

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


bool GeckoIsConnected();
bool GeckoConnect();
void GeckoDisconnect();
bool GeckoReset();
WiiStatus GeckoStatus();
FTDICommand GeckoSendRawCommand(const uint8_t id);
void GeckoSendFail();
void GeckoSendRetry();
void GeckoSendAck();
bool GeckoDump(uint32_t startdump, uint32_t enddump, uint8_t *bytes);
bool GeckoPoke08(uint32_t address, uint8_t value);

FTDICommand GeckoWrite(uint8_t *sendbyte, uint32_t nobytes);
FTDICommand GeckoRead(uint8_t *recbyte, uint32_t nobytes);
void GeckoPauseCmd();
void GeckoSafePauseCmd();
void GeckoResumeCmd();
void GeckoSafeResumeCmd();



#endif /* USBGECKO_H_ */
