/*
 * imgecko.cpp
 *
 *  Created on: Mar 7, 2018
 *      Author: hinxx
 */

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <ftd2xx.h>
#include "imgecko.h"

#define MAX_DEVICES		5

static FT_HANDLE handle;
static bool connected = false;


//#region base constants
const uint32_t packetsize = 0xF800;
const uint32_t uplpacketsize = 0xF80;

const uint8_t      cmd_poke08 = 0x01;
const uint8_t      cmd_poke16 = 0x02;
const uint8_t     cmd_pokemem = 0x03;
const uint8_t     cmd_readmem = 0x04;
const uint8_t       cmd_pause = 0x06;
const uint8_t    cmd_unfreeze = 0x07;
const uint8_t  cmd_breakpoint = 0x09;
const uint8_t cmd_breakpointx = 0x10;
const uint8_t    cmd_sendregs = 0x2F;
const uint8_t     cmd_getregs = 0x30;
const uint8_t    cmd_cancelbp = 0x38;
const uint8_t  cmd_sendcheats = 0x40;
const uint8_t      cmd_upload = 0x41;
const uint8_t        cmd_hook = 0x42;
const uint8_t   cmd_hookpause = 0x43;
const uint8_t        cmd_step = 0x44;
const uint8_t      cmd_status = 0x50;
const uint8_t   cmd_cheatexec = 0x60;
const uint8_t cmd_nbreakpoint = 0x89;
const uint8_t     cmd_version = 0x99;

const uint8_t         GCBPHit = 0x11;
const uint8_t           GCACK = 0xAA;
const uint8_t         GCRETRY = 0xBB;
const uint8_t          GCFAIL = 0xCC;
const uint8_t          GCDONE = 0xFF;

const uint8_t        GCNewVer = 0x80;

//private static readonly Byte[] GCAllowedVersions = new Byte[] { GCNewVer };

const uint8_t       BPExecute = 0x03;
const uint8_t          BPRead = 0x05;
const uint8_t         BPWrite = 0x06;
const uint8_t     BPReadWrite = 0x07;
//#endregion

bool Connect() {
	char pcBufLD[64];
//	int	iNumDevs = 0;
//	char cBufLD[MAX_DEVICES][64];

	if (connected) {
		Disconnect();
	}

	connected = false;

	uint32_t ftdiDeviceCount = 0;
	FT_STATUS ftStatus = FT_OK;

//	ftStatus = PFTDI.GetNumberOfDevices(ref ftdiDeviceCount);
	ftStatus = FT_ListDevices(pcBufLD, &ftdiDeviceCount, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);

	//Check if device query works
	if (ftStatus != FT_OK) {
		Disconnect();
		//throw new EUSBGeckoException(EUSBErrorCode.FTDIQueryError);
		return false;
	}
	//Check if devices available
	if (ftdiDeviceCount == 0) {
		Disconnect();
		//throw new EUSBGeckoException(EUSBErrorCode.noFTDIDevicesFound);
		return false;
	}
	//Open USB Gecko
//	ftStatus = PFTDI.OpenBySerialNumber("GECKUSB0");
	sprintf(pcBufLD, "%s", "GECKUSB0");
	ftStatus = FT_OpenEx(pcBufLD, FT_OPEN_BY_SERIAL_NUMBER, &handle);

	if (ftStatus != FT_OK) {
		// Don't disconnect if there's nothing connected
		Disconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.noUSBGeckoFound);
		return false;
	}

	//Set Timeouts to 2 seconds
//	ftStatus = PFTDI.SetTimeouts(2000,2000);
	ftStatus = FT_SetTimeouts(handle, 2000, 2000);
	if (ftStatus != FT_OK) {
		Disconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDITimeoutSetError);
		return false;
	}
//#if !MONO
	unsigned char LatencyTimer = 2;

//	ftStatus = PFTDI.SetLatencyTimer(LatencyTimer);
	ftStatus = FT_SetLatencyTimer(handle, LatencyTimer);
	if (ftStatus != FT_OK) {
		Disconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDITimeoutSetError);
		return false;
	}
//#endif

	//Set Transfer rate
	//ftStatus = PFTDI.InTransferSize(0x10000);
	ftStatus = FT_SetUSBParameters(handle, 0x10000, 0);
	if (ftStatus != FT_OK) {
		Disconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDITimeoutSetError);
		return false;
	}

	//Initialize USB Gecko
	if (InitGecko()) {
//		System.Threading.Thread.Sleep(150);
		connected = true;
		return true;
	}
	else
	return false;
}

void Disconnect()
{
	connected= false;
	FT_Close(handle);
}

bool InitGecko()
{
	FT_STATUS ftStatus = FT_OK;
	//Reset device
	ftStatus = FT_ResetDevice(handle);
	if (ftStatus != FT_OK) {
		Disconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDIResetError);
		return false;
	}

	//Purge RX buffers
//	ftStatus = PFTDI.Purge(FT_PURGE_RX);
	ftStatus = FT_Purge(handle, FT_PURGE_RX);
	if (ftStatus != FT_OK) {
		Disconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDIPurgeRxError);
		return false;
	}
	//Purge TX buffers
//	ftStatus = PFTDI.Purge(FT_PURGE_TX);
	ftStatus = FT_Purge(handle, FT_PURGE_TX);
	if (ftStatus != FT_OK) {
		Disconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDIPurgeTxError);
	}

	return true;
}

FTDICommand GeckoRead(uint8_t *recbyte, uint32_t nobytes)
{
	uint32_t bytes_read = 0;

	FT_STATUS ftStatus = FT_Read(handle, recbyte, nobytes, &bytes_read);
	printf("GeckoRead() FTstatus %d\n", ftStatus);
	if (ftStatus == FT_OK) {
		if (bytes_read != nobytes) {
			// lost bytes in transmission
			printf("GeckoRead(): ERR read %d/%d\n", bytes_read, nobytes);
			return CMD_ResultError;
		}
	} else {
		// fatal error
		printf("GeckoRead(): ERR read %d/%d\n", bytes_read, nobytes);
		return CMD_FatalError;
	}

	printf("GeckoRead(): read %d/%d\n", bytes_read, nobytes);
	return CMD_OK;
}

FTDICommand GeckoWrite(uint8_t *sendbyte, uint32_t nobytes)
{
	uint32_t bytes_written = 0;

	FT_STATUS ftStatus = FT_Write(handle, sendbyte, nobytes, &bytes_written);
	printf("GeckoWrite() FTstatus %d\n", ftStatus);
	if (ftStatus == FT_OK) {
		if (bytes_written != nobytes) {
			// lost bytes in transmission
			printf("GeckoWrite(): ERR wrote %d/%d\n", bytes_written, nobytes);
			return CMD_ResultError;
		}
	} else {
		// fatal error
		printf("GeckoWrite(): ERR wrote %d/%d\n", bytes_written, nobytes);
		return CMD_FatalError;
	}

	printf("GeckoWrite(): wrote %d/%d\n", bytes_written, nobytes);
	return CMD_OK;
}

//Allows sending a basic one byte command to the Wii
FTDICommand RawCommand(const uint8_t id) {
	return GeckoWrite((uint8_t *)&id, 1);
}

//Returns the console status
WiiStatus GeckoStatus() {
//	System.Threading.Thread.Sleep(100);
	//Initialise Gecko
	if (! InitGecko()) {
//	throw new EUSBGeckoException(EUSBErrorCode.FTDIResetError);
		return WiiStatusUnknown;
	}

	//Send status command
	if (RawCommand(cmd_status) != CMD_OK) {
//	throw new EUSBGeckoException(EUSBErrorCode.FTDICommandSendError);
		return WiiStatusUnknown;
	}

//			System.Threading.Thread.Sleep(10);

	//Read status
//	Byte[] buffer=new Byte[1];
	uint8_t buffer = -1;
	if (GeckoRead(&buffer, 1) != CMD_OK) {
//	throw new EUSBGeckoException(EUSBErrorCode.FTDIReadDataError);
		return WiiStatusUnknown;
	}
	printf("%s: status = %d\n", __func__, buffer);

	//analyse reply
	switch (buffer) {
		case 0: return WiiStatusRunning;
		case 1: return WiiStatusPaused;
		case 2: return WiiStatusBreakpoint;
		case 3: return WiiStatusLoader;
		default: return WiiStatusUnknown;
	}
}
