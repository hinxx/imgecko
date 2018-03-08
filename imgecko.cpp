/*
 * imgecko.cpp
 *
 *  Created on: Mar 7, 2018
 *      Author: hinxx
 */

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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

///////////////////////////////////////////////////////////////////////////
//https://stackoverflow.com/questions/7775991/how-to-get-hexdump-of-a-structure-data
static void _hexdump(const char *desc, void *addr, int len)
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}

static bool _is_big_endian()
{
	union
	{
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };

	return (bint.c[0] == 1);
}

static bool _is_little_endian()
{
	return (! _is_big_endian());
}

static uint16_t _swap16(uint16_t input)
{
//	if (BitConverter.IsLittleEndian)
	if (_is_little_endian()) {
		return 	((uint16_t)(
				((0xFF00 & input) >> 8) |
				((0x00FF & input) << 8)));
	}
	return input;
}

static uint32_t _swap32(uint32_t input)
{
//	if (BitConverter.IsLittleEndian)
	if (_is_little_endian()) {
		return 	((uint32_t)(
				((0xFF000000 & input) >> 24) |
				((0x00FF0000 & input) >> 8) |
				((0x0000FF00 & input) << 8) |
				((0x000000FF & input) << 24)));
	}
	return input;
}

static uint64_t _swap64(uint64_t input)
{
//	if (BitConverter.IsLittleEndian)
	if (_is_little_endian()) {
		return 	((uint64_t)(
				((0xFF00000000000000 & input) >> 56) |
				((0x00FF000000000000 & input) >> 40) |
				((0x0000FF0000000000 & input) >> 24) |
				((0x000000FF00000000 & input) >> 8) |
				((0x00000000FF000000 & input) << 8) |
				((0x0000000000FF0000 & input) << 24) |
				((0x000000000000FF00 & input) << 40) |
				((0x00000000000000FF & input) << 56)));
	}
	return input;
}


///////////////////////////////////////////////////////////////////////////

bool GeckoIsConnected() {
	return connected;
}

bool GeckoConnect() {
	char pcBufLD[64];
//	int	iNumDevs = 0;
//	char cBufLD[MAX_DEVICES][64];

	if (connected) {
		GeckoDisconnect();
		printf("%s: disconnecting\n", __func__);
		sleep(1);
	}

	connected = false;

	uint32_t ftdiDeviceCount = 0;
	FT_STATUS ftStatus = FT_OK;

//	ftStatus = PFTDI.GetNumberOfDevices(ref ftdiDeviceCount);
//	ftStatus = FT_ListDevices(pcBufLD, &ftdiDeviceCount, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);
	ftStatus = FT_ListDevices(&ftdiDeviceCount, NULL, FT_LIST_NUMBER_ONLY);
	//Check if device query works
	if (ftStatus != FT_OK) {
		GeckoDisconnect();
		//throw new EUSBGeckoException(EUSBErrorCode.FTDIQueryError);
		return false;
	}
	//Check if devices available
	if (ftdiDeviceCount == 0) {
		GeckoDisconnect();
		//throw new EUSBGeckoException(EUSBErrorCode.noFTDIDevicesFound);
		return false;
	}
	//Open USB Gecko
//	ftStatus = PFTDI.OpenBySerialNumber("GECKUSB0");
	sprintf(pcBufLD, "%s", "GECKUSB0");
	ftStatus = FT_OpenEx(pcBufLD, FT_OPEN_BY_SERIAL_NUMBER, &handle);

	if (ftStatus != FT_OK) {
		// Don't disconnect if there's nothing connected
		GeckoDisconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.noUSBGeckoFound);
		return false;
	}

	//Set Timeouts to 2 seconds
//	ftStatus = PFTDI.SetTimeouts(2000,2000);
	ftStatus = FT_SetTimeouts(handle, 2000, 2000);
	if (ftStatus != FT_OK) {
		GeckoDisconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDITimeoutSetError);
		return false;
	}
//#if !MONO
	unsigned char LatencyTimer = 2;

//	ftStatus = PFTDI.SetLatencyTimer(LatencyTimer);
	ftStatus = FT_SetLatencyTimer(handle, LatencyTimer);
	if (ftStatus != FT_OK) {
		GeckoDisconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDITimeoutSetError);
		return false;
	}
//#endif

	//Set Transfer rate
	//ftStatus = PFTDI.InTransferSize(0x10000);
	ftStatus = FT_SetUSBParameters(handle, 0x10000, 0);
	if (ftStatus != FT_OK) {
		GeckoDisconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDITimeoutSetError);
		return false;
	}

	//Initialize USB Gecko
	if (GeckoReset()) {
//		System.Threading.Thread.Sleep(150);
		connected = true;
		return true;
	}
	else
	return false;
}

void GeckoDisconnect()
{
	connected = false;
	FT_Close(handle);
}

bool GeckoReset()
{
	FT_STATUS ftStatus = FT_OK;
	//Reset device
	ftStatus = FT_ResetDevice(handle);
	if (ftStatus != FT_OK) {
		GeckoDisconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDIResetError);
		return false;
	}

	//Purge RX buffers
//	ftStatus = PFTDI.Purge(FT_PURGE_RX);
	ftStatus = FT_Purge(handle, FT_PURGE_RX);
	if (ftStatus != FT_OK) {
		GeckoDisconnect();
//		throw new EUSBGeckoException(EUSBErrorCode.FTDIPurgeRxError);
		return false;
	}
	//Purge TX buffers
//	ftStatus = PFTDI.Purge(FT_PURGE_TX);
	ftStatus = FT_Purge(handle, FT_PURGE_TX);
	if (ftStatus != FT_OK) {
		GeckoDisconnect();
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
		printf("GeckoRead(): FATAL read %d/%d\n", bytes_read, nobytes);
		return CMD_FatalError;
	}

	printf("GeckoRead(): read %d/%d\n", bytes_read, nobytes);
	_hexdump("GeckoRead()", recbyte, bytes_read);
	return CMD_OK;
}

FTDICommand GeckoWrite(uint8_t *sendbyte, uint32_t nobytes)
{
	uint32_t bytes_written = 0;

	_hexdump("GeckoWrite()", sendbyte, nobytes);
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
		printf("GeckoWrite(): FATAL wrote %d/%d\n", bytes_written, nobytes);
		return CMD_FatalError;
	}

	printf("GeckoWrite(): wrote %d/%d\n", bytes_written, nobytes);
	return CMD_OK;
}

//Allows sending a basic one byte command to the Wii
FTDICommand GeckoSendRawCommand(const uint8_t id) {
	return GeckoWrite((uint8_t *)&id, 1);
}

//Returns the console status
WiiStatus GeckoStatus() {
//	System.Threading.Thread.Sleep(100);
	//Initialise Gecko
	if (! GeckoReset()) {
//	throw new EUSBGeckoException(EUSBErrorCode.FTDIResetError);
		return WiiStatusUnknown;
	}

	//Send status command
	if (GeckoSendRawCommand(cmd_status) != CMD_OK) {
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

//Sends a GCFAIL to the game.. in case the Gecko handler hangs..
// sendfail might solve it!
void GeckoSendFail()
{
	//Only needs to send a cmd_unfreeze to Wii
	//Ignores the reply, send this command multiple times!
	GeckoSendRawCommand(GCFAIL);
}

void GeckoSendRetry()
{
	GeckoSendRawCommand(GCRETRY);
}

void GeckoSendAck()
{
	GeckoSendRawCommand(GCACK);
}

//Pauses the game
void GeckoPauseCmd()
{
	//Only needs to send a cmd_pause to Wii
	if (GeckoSendRawCommand(cmd_pause) != CMD_OK) {
//		throw new EUSBGeckoException(EUSBErrorCode.FTDICommandSendError);
	}
}

// Tries to repeatedly pause the game until it succeeds
void GeckoSafePauseCmd()
{
	bool WasRunning = (GeckoStatus() == WiiStatusRunning);
	while (WasRunning)
	{
		GeckoPauseCmd();
//		System.Threading.Thread.Sleep(100);
		//wait!!
		usleep(100000);
		// Sometimes, the game doesn't actually pause...
		// So loop repeatedly until it does!
		WasRunning = (GeckoStatus() == WiiStatusRunning);
	}
}

//Unpauses the game
void GeckoResumeCmd()
{
	//Only needs to send a cmd_unfreeze to Wii
	if (GeckoSendRawCommand(cmd_unfreeze) != CMD_OK)
	{
//		throw new EUSBGeckoException(EUSBErrorCode.FTDICommandSendError);
	}
}

// Tries repeatedly to resume the game until it succeeds
void GeckoSafeResumeCmd()
{
	bool NotRunning = (GeckoStatus() != WiiStatusRunning);
	int failCounter = 0;
	while (NotRunning && failCounter < 10)
	{
		GeckoResumeCmd();
//		System.Threading.Thread.Sleep(100);
		//wait!
		usleep(100000);
		// Sometimes, the game doesn't actually resume...
		// So loop repeatedly until it does!
//		try
//		{
			NotRunning = (GeckoStatus() != WiiStatusRunning);
//		}
//		catch (FTDIUSBGecko.EUSBGeckoException ex)
//		{
			NotRunning = true;
			failCounter++;
//		}
	}
}

// XXX: not needed ATM!
////Send update on a running process to the parent class
//protected void SendUpdate(UInt32 currentchunk, UInt32 allchunks, UInt32 transferred, UInt32 length, bool okay, bool dump)
//{
//	if (PChunkUpdate != null)
//		PChunkUpdate(currentchunk, allchunks, transferred, length, okay, dump);
//}

//public void Dump(UInt32 startdump, UInt32 enddump, Stream[] saveStream)
bool GeckoDump(uint32_t startdump, uint32_t enddump, uint8_t *bytes)
{
	//Reset connection
	GeckoReset();

//	if (GeckoApp.ValidMemory.rangeCheckId(startdump)
//			!= GeckoApp.ValidMemory.rangeCheckId(enddump)) {
//		enddump =
//				GeckoApp.ValidMemory.ValidAreas[GeckoApp.ValidMemory.rangeCheckId(
//						startdump)].high;
//	}

//	if (!GeckoApp.ValidMemory.validAddress(startdump))
//		return;

	//How many bytes of data have to be transferred
	uint32_t memlength = enddump - startdump;
	printf("%s: start %08X, end %08X, length %08X\n", __func__, startdump, enddump, memlength);

	//How many chunks do I need to split this data into
	//How big ist the last chunk
//	uint32_t fullchunks = memlength / packetsize;
//	uint32_t lastchunk = memlength % packetsize;

	//How many chunks do I need to transfer
//	uint32_t allchunks = fullchunks;
//	if (lastchunk > 0)
//		allchunks++;

//	uint64_t GeckoMemRange = ByteSwap.Swap(
//			(uint64_t)(((uint64_t) startdump << 32) + ((uint64_t) enddump)));
//	if (GeckoWrite(BitConverter.GetBytes(cmd_readmem), 1) != FTDICommand.CMD_OK)
//		throw new EUSBGeckoException(EUSBErrorCode.FTDICommandSendError);
	uint64_t GeckoMemRange = _swap64(
			(uint64_t)(((uint64_t) startdump << 32) + ((uint64_t) enddump)));
	_hexdump("GeckoMemRange", (char *)&GeckoMemRange, 8);

	printf("%s: sending READ MEM command..\n", __func__);
	if (GeckoSendRawCommand(cmd_readmem) != CMD_OK)
	{
//		throw new EUSBGeckoException(EUSBErrorCode.FTDICommandSendError);
		return false;
	}

	//Read reply - expcecting GCACK
	printf("%s: waiting for ACK!\n", __func__);
	int retry = 0;
	while (retry < 10)
	{
//		Byte[] response = new Byte[1];
		uint8_t response = -1;
		if (GeckoRead(&response, 1) != CMD_OK)
		{
//			throw new EUSBGeckoException(EUSBErrorCode.FTDIReadDataError);
			return false;
		}
//		Byte reply = response[0];
		if (response == GCACK)
		{
			printf("%s: got ACK!\n", __func__);
			break;
		}
		else if (response == 9)
		{
//			throw new EUSBGeckoException(EUSBErrorCode.FTDIInvalidReply);
			printf("%s: invalid reply!\n", __func__);
		}
	}

	//Now let's send the dump information
	if (GeckoWrite((uint8_t *)&GeckoMemRange, 8) != CMD_OK)
	{
//		throw new EUSBGeckoException(EUSBErrorCode.FTDICommandSendError);
		return false;
	}

	//We start with chunk 0
//	uint32_t chunk = 0;
	retry = 0;
//	uint32_t buffer_idx = 0;

	// Reset cancel flag
//	bool done = false;
//	CancelDump = false;

	uint32_t len = packetsize;
	uint32_t count = 0;

//	Byte[] buffer = new Byte[packetsize]; //read buffer
	uint8_t buffer[packetsize];
//	while (chunk < fullchunks && !done)
	while (count < memlength)
	{
		len = packetsize;
		if ((memlength - count) < len) {
			len = memlength - count;
		}
		printf("%s: getting chunck of %d bytes\n", __func__, len);

		//No output yet availible
//		SendUpdate(chunk, allchunks, chunk * packetsize, memlength, retry == 0,
//				true);
		//Set buffer
//		FTDICommand returnvalue = GeckoRead(buffer, packetsize);
		FTDICommand returnvalue = GeckoRead(buffer, len);
		if (returnvalue == CMD_ResultError) {
			retry++;
			if (retry >= 3) {
				//Give up, too many retries
//				GeckoWrite(BitConverter.GetBytes(GCFAIL), 1);
//				throw new EUSBGeckoException(EUSBErrorCode.TooManyRetries);
				printf("%s: giving up, too many retries..\n", __func__);
				GeckoSendFail();
				return false;
			}
//			GeckoWrite(BitConverter.GetBytes(GCRETRY), 1);
			printf("%s: retrying ..\n", __func__);
			GeckoSendRetry();
			continue;
		} else if (returnvalue == CMD_FatalError) {
			//Major fail, give it up
//			GeckoWrite(BitConverter.GetBytes(GCFAIL), 1);
//			throw new EUSBGeckoException(EUSBErrorCode.FTDIReadDataError);
			printf("%s: giving up, fatal!\n", __func__);
			GeckoSendFail();
			return false;
		}
		//write received package to output stream
//		foreach (Stream stream in saveStream)
//		{
//			stream.Write(buffer, 0, ((Int32)packetsize));
//		}
//		_hexdump("Full chunk", (char *)buffer, packetsize);

		assert(count <= memlength);
//		memcpy(bytes + buffer_idx, buffer, packetsize);
//		buffer_idx += (chunk * packetsize);
		memcpy(bytes + count, buffer, len);
//		buffer_idx += (chunk * len);
		count += len;
		assert(count <= memlength);

		//reset retry counter
		retry = 0;
		//next chunk
//		chunk++;

//		if (!CancelDump) {
//			//ackowledge package
//			GeckoWrite(BitConverter.GetBytes(GCACK), 1);
//		} else {
//			// User requested a cancel
//			GeckoWrite(BitConverter.GetBytes(GCFAIL), 1);
//			done = true;
//		}

		//ackowledge package
		GeckoSendAck();
	}

#if 0
	//Final package?
	while (!done)
	{
		printf("%s: getting last chunck\n", __func__);

		//No output yet availible
//		SendUpdate(chunk, allchunks, chunk * packetsize, memlength, retry == 0,
//				true);
		//Set buffer
		// buffer = new Byte[lastchunk];
		FTDICommand returnvalue = GeckoRead(buffer, lastchunk);
		if (returnvalue == CMD_ResultError)
		{
			retry++;
			if (retry >= 3)
			{
				//Give up, too many retries
//				GeckoWrite(BitConverter.GetBytes(GCFAIL), 1);
//				throw new EUSBGeckoException(EUSBErrorCode.TooManyRetries);
				printf("%s: giving up, too many retries..\n", __func__);
				GeckoSendFail();
				return false;
			}
//			GeckoWrite(BitConverter.GetBytes(GCRETRY), 1);
			printf("%s: retrying ..\n", __func__);
			GeckoSendRetry();
			continue;
		} else if (returnvalue == CMD_FatalError) {
			//Major fail, give it up
//			GeckoWrite(BitConverter.GetBytes(GCFAIL), 1);
//			throw new EUSBGeckoException(EUSBErrorCode.FTDIReadDataError);
			printf("%s: giving up, fatal!\n", __func__);
			GeckoSendFail();
			return false;
		}
		//write received package to output stream
//		foreach (Stream stream in saveStream)
//		{
//			stream.Write(buffer, 0, ((Int32)lastchunk));
//		}

//		_hexdump("Last chunk", (char *)buffer, lastchunk);

		assert(buffer_idx < memlength);
		memcpy(bytes + buffer_idx, buffer, lastchunk);
		buffer_idx += (chunk * lastchunk);
		assert(buffer_idx < memlength);

		//reset retry counter
		retry = 0;
		//cancel while loop
		done = true;
		//ackowledge package
//		GeckoWrite(BitConverter.GetBytes(GCACK), 1);
		GeckoSendAck();
	}
#endif

//	SendUpdate(allchunks, allchunks, memlength, memlength, true, true);
//}

	printf("%s: success!\n", __func__);

	return true;
}
