/*
	Simple example to open a maximum of 4 devices - write some data then read it back.
	Shows one method of using list devices also.
	Assumes the devices have a loopback connector on them and they also have a serial number

	To build use the following gcc statement 
	(assuming you have the d2xx library in the /usr/local/lib directory).
	gcc -o simple main.c -L. -lftd2xx -Wl,-rpath /usr/local/lib
*/

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ftd2xx.h>
#include "imgecko.h"

static bool Connecting;
//static bool SteppingOut;
//static bool SteppingUntil;
//static bool SearchingDisassembly;

bool Connect()
{
	if (Connecting)
	{
		Connecting = false;
//		CUSBGecko.Text = "Connect to Gecko";
		return false;
	}

	bool retry = true;
	bool success = false;
	int attempt = 0;
//
//	if (GeckoIsConnected())
//	{
////		StatusCap.Text = "Disconnecting!";
////		try {gecko.Disconnect();}
////		catch {}
////		Application.DoEvents();
////		System.Threading.Thread.Sleep(500);
//		printf("%s: disconnecting\n", __func__);
//		GeckoDisconnect();
//		sleep(1);
//	}

	while (retry && !success)
	{
		attempt++;
//		StatusCap.Text = "Connection attempt: " + attempt.ToString();
		printf("%s: connection attempt %d\n", __func__, attempt);
//		Application.DoEvents();
//		try
//		{
//			if (!gecko.Connect())
//			throw new Exception();
		if (! GeckoConnect()) {
			return false;
		}
		int failAttempt = 0;
		Connecting = true;
//			CUSBGecko.Text = "Cancel Connection";
		while (GeckoStatus() == WiiStatusUnknown)
		{
//				gecko.sendfail();
			GeckoSendFail();
			failAttempt++;
			if (failAttempt > 10 || !Connecting)
			{
				if (!Connecting)
				{
					retry = false;
				}
				Connecting = false;
//					throw new Exception();
			}
//				Application.DoEvents();
		}
		Connecting = false;

//			if (gecko.status() == WiiStatus.Loader)
		if (GeckoStatus() == WiiStatusLoader)
		{
//				DialogResult dr = MessageBox.Show("No game has been loaded yet!\nGecko dotNET requires a running game!\n\nShould a game be automatically loaded!", "Gecko dotNET", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);
			printf("%s: No game has been loaded yet!\n", __func__);

//				char ans;
//				ans = getc(stdin);

//				if (dr == DialogResult.Yes)
//				gecko.Hook();
//				if (dr == DialogResult.Cancel)
//				{
//					Close();
//					return;
//				}
//				while (gecko.status() == WiiStatus.Loader)
			while (GeckoStatus() == WiiStatusLoader)
			{
//					StatusCap.Text = "Waiting for game!";
				printf("%s: waiting for game..\n", __func__);
//					Application.DoEvents();
//					System.Threading.Thread.Sleep(100);
				sleep(1);
			}
		}

		success = true;
//		}
//		catch
//		{
//			if (attempt % 3 != 0)
//			continue;
//			retry =
//			MessageBox.Show("Connection to the USB Gecko has failed!\n" +
//					"Do you want to retry?", "Connection issue",
//					MessageBoxButtons.YesNo, MessageBoxIcon.Warning) ==
//			DialogResult.Yes;
//		}
	}

//	EnableMainControls(success);
//	ResSrch.Enabled = false;
//	RGame.Enabled = success;
//	PGame.Enabled = success;
//	OpenNotePad.Enabled = success;

//	try
//	{
	if (success)
	{
//			CUSBGecko.Text = "Reconnect to Gecko";
//			StatusCap.Text = "Ready!";
		printf("%s: READY!\n", __func__);

		// 6 bytes for Game ID + 1 for null + 1 for Version
		// Apparently, some WiiWare/VC stuff only has 4 bytes for Game ID, but still 1 null + 1 Version too
		const int bytesToRead = 8;
//			MemoryStream ms = new MemoryStream();
//			gecko.Dump(0x80001800, 0x80001800 + bytesToRead, ms);
		uint8_t buffer[16];
		GeckoDump(0x80001800, 0x80001800 + bytesToRead, buffer);


//
//			String name = "";
//			Byte[] buffer = new Byte[bytesToRead];
//			ms.Seek(0, SeekOrigin.Begin);
//			ms.Read(buffer, 0, bytesToRead);
//			name = Encoding.ASCII.GetString(buffer);
//			String rname = "";
//			// Don't read version digit into name
//			int i;// Declare out of for loop scope so we can test the index later
//			// Go to bytesToRead - 2
//			// Should end at 4 for VC games and 6 for Wii games
//			for (i = 0; i < bytesToRead - 2; i++)
//			if (name[i] != (char)0)
//			rname += name[i];
//			else
//			break;
//
//			// first time loading a game, or game changed; reload GCT files
//			bool gamenameChanged = gamename != rname;
//			gamename = rname;
//
//			int gameVer = ((int)(name[i + 1])) + 1;
//
//			this.Text = "Gecko dotNET (" + gamename;
//			if (gameVer != 1)
//			{
//				this.Text += " version " + (gameVer).ToString();
//			}
//			this.Text += ")";
//
//			if (gamenameChanged)
//			{
//				GCTLoadCodes();
//			}
//
//			GameNameStored = false;
//			DisconnectButton.Enabled = true;

		return true;
	}
	else
	{
//			DisconnectButton.Enabled = false;
//			CUSBGecko.Text = "Connect to Gecko";
//			StatusCap.Text = "No USB Gecko connection availible!";
//
//			this.Text = "Gecko dotNET";
		printf("%s: no connection to USB Gecko!\n", __func__);
		return false;
	}
//	}
//	catch (EUSBGeckoException exc)
//	{
//		exceptionHandling.HandleException(exc);
//	}
}

int main()
{
//	GeckoConnect();
//	sleep(0.1);
//
//	while (1) {
//
//		//Initialize USB Gecko
//		bool ret = GeckoReset();
//		if (ret) {
//			// raw 1
////			uint8_t cmd_status = 0x50;
////			GeckoWrite(&cmd_status, 1);
////			uint8_t status = -1;
////			GeckoRead(&status, 1);
//
//			// WiiStatus GeckoStatus()
//			WiiStatus status = GeckoStatus();
//			printf("status = %x\n", status);
//		}
//
//		sleep(2);
//	}
//
//	GeckoDisconnect();
//
	bool ret = Connect();

	sleep(2);

	/* here, do your time-consuming job */

	if (ret) {
//		uint8_t buffer[0x100000];
//		uint32_t bytesToRead = 0x10000;
//
//		clock_t begin = clock();
//
//		ret = GeckoDump(0x80000000, 0x80000000 + bytesToRead, buffer);
//
//		clock_t end = clock();
//		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
//		printf("%s: time spent for %d bytes: %f\n", __func__, bytesToRead, time_spent);


		// pause the game
//		GeckoReset();
		printf("Pausing the game for 2 seconds..\n");
//		GeckoPauseCmd();
		GeckoSafePauseCmd();
		sleep(2);
//		GeckoReset();
		printf("Resuming the game..\n");
		GeckoSafeResumeCmd();

	}

}
