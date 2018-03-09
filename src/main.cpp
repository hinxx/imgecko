// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "imgui_memory_editor.h"

#include "usbgecko.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>

static bool Connecting = false;
static bool Connected = false;
static bool doDump = false;
static bool doExit = false;
static uint8_t buffer[0x100000];
static char startBuf[64] = "80351000";
static char endBuf[64] = "80352000";
static char countBuf[64] = "1000";
static uint32_t framerate = 0;
static uint32_t bufrate = 0;
static uint32_t startAddr = 0x80000000;
static uint32_t count = 0;

//////////////////////////////////////////////////////////////////////////////


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
		Connected = false;
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

		Connected = true;
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

void *print_message_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);

     bool ret;

     while (! doExit)
     {
    	 if (! Connected)
    	 {
    		 ret = Connect();
    		 sleep(1);
    	 }
    	 if (Connected && doDump)
    	 {
//     		uint32_t bytesToRead = 0x10000;

     		clock_t begin = clock();

//     		startAddr = strtol(startBuf, NULL, 16);
////     		uint32_t endAddr = strtol(endBuf, NULL, 16);
//     		count = strtol(countBuf, NULL, 16);
     		printf("%s: start address 0x%08X\n", __func__, startAddr);
     		printf("%s: end address 0x%08X\n", __func__, startAddr+count);
     		ret = GeckoDump(startAddr, startAddr+count, buffer);

     		clock_t end = clock();
     		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
     		printf("%s: time spent for %d bytes: %f\n", __func__, count, time_spent);

     		bufrate++;
     		printf("%s: frame rate %d, buffer rate %d, %f\n", __func__, framerate, bufrate, bufrate/(framerate/60.0));

//     		GeckoPoke08()
     		//doDump = false;
    		usleep(50000);
    	 }
		usleep(10000);
     }
}

//////////////////////////////////////////////////////////////////////////////


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}
//
//static int TextEditCallback(ImGuiTextEditCallbackData* data)
//{
////    UserData* user_data = (UserData*)data->UserData;
//	printf("%s: called.., data %p\n", __func__, data);
//}

static void MemEditWriteFn(uint8_t* data, size_t off, uint8_t d)
{
	data[off] = (uint8_t)d;
	printf("%s: called.., data[%X] %X\n", __func__, off, d);
//	GeckoPoke08();
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(800, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    gl3wInit();

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	pthread_t thread1;
	const char *message1 = "Thread 1";
	int  iret1;
    /* Create independent threads each of which will execute function */

     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
     if(iret1)
     {
         fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
         exit(EXIT_FAILURE);
     }
     printf("pthread_create() for thread 1 returns: %d\n",iret1);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        // 1. Show a simple window.
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
        {
            static float f = 0.0f;
            static int counter = 0;
            ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
        if (show_demo_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowDemoWindow(&show_demo_window);
        }

		ImGui::Begin("GeckoWindow");
		//mem_edit_2.DrawContents((unsigned char *)window, 0x1000, 0);
        ImGui::Text("Hello from GECKO window!");

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AlwaysInsertMode;
        ImGui::InputText("Start", startBuf, 9, flags);
        //ImGui::InputText("End", endBuf, 9, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::InputText("Count", countBuf, 9, flags);

 		startAddr = strtol(startBuf, NULL, 16);
//     		uint32_t endAddr = strtol(endBuf, NULL, 16);
 		count = strtol(countBuf, NULL, 16);

        if (ImGui::Button("Start Dump")) {
			framerate = 0;
			bufrate = 0;
        	doDump = true;
        }

        if (ImGui::Button("Stop Dump")) {
        	doDump = false;
        }

		static MemoryEditor mem_edit_2;
		ImGui::Begin("MyWindow");
		mem_edit_2.WriteFn = MemEditWriteFn;
		mem_edit_2.DrawContents((unsigned char *)buffer, 0x10000, (size_t)startAddr);
		ImGui::End();

		ImGui::End();

		framerate++;

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */
    doExit = true;
	pthread_join( thread1, NULL);

     // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}
