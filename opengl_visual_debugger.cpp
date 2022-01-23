#include <fstream>
#include <iostream>
#include <signal.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <map>
#include "tiny_opengl3_app.h"
#include "tiny_clock.h"
#include "tiny_chrome_trace_util.h"
#include "tiny_logging.h"
#include <iostream>
#include "imgui.h"
#include "opengl3/imgui_impl_opengl3.h"
#include "imnodes.h"
#include "imgui.h"

#include "win32_shared_memory.h"
#include <iostream>

using namespace visdebug;


bool keep_running = true;


#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"


bool releasedA = false;
TinyKeyboardCallback default_keyboard_callback = 0;

TinyMouseMoveCallback default_mouse_move_callback = 0;
TinyMouseButtonCallback default_mouse_button_callback = 0;

float g_MouseWheel = 0;
float gMouseX = 0;
float gMouseY = 0;
int g_MousePressed[3] = { 0,0,0 };

void MyMouseMoveCallback(float x, float y)
{
    gMouseX = x;
    gMouseY = y;
    if (default_mouse_move_callback)
        default_mouse_move_callback(x, y);
}
void MyMouseButtonCallback(int button, int state, float x, float y)
{
    gMouseX = x;
    gMouseY = y;
    g_MousePressed[button] = state;
    if (default_mouse_button_callback)
        default_mouse_button_callback(button, state, x, y);
}


float noteToFreq(int note) {
    float a = 440.; // frequency of A (coomon value is 440Hz)
    return (a / 3.) * pow(2., ((note - 9.) / 12.));
}
std::map<char, int> key2note = {
    {'a',0},
        {'w',1},
    {'s',2},
        {'e',3},
    {'d',4},
    {'f',5},
        {'t',6},
    {'g',7},
        {'y',8},
    {'h',9},
        {'u',10},
    {'j',11},
    {'k',12},
};

int octave = 1;

void MyKeyboardCallback(int keycode, int state)
{

    if ((keycode == 'a') && (state == 0))
    {
        releasedA = true;
    }
   
    ImGuiIO& io = ImGui::GetIO();
    if (state == 1)
        io.KeysDown[keycode] = true;
    if (state == 0)
        io.KeysDown[keycode] = false;

    if (state==1 &&
        (
            (keycode >= 'a' && keycode <= 'z') ||
            (keycode >= '0' && keycode <= '9') ||
            (keycode == ' ') ||
            (keycode == '!') || (keycode == '@') || (keycode == '#') || (keycode == '$') || (keycode == '%') || (keycode == '.') ||
            (keycode == ',') || (keycode == '\"') || (keycode == '\'') || (keycode == '~') || (keycode == '-') || (keycode == '=') ||
            (keycode == '+') || (keycode == '[') || (keycode == ']') || (keycode == '?') || (keycode == '*') || (keycode == '(') ||
            (keycode == ')') || (keycode == '^') || (keycode == '&')
        )
       )
    {
        io.AddInputCharacter(keycode);
    }
    // Modifiers are not reliable across systems
    if (keycode == TINY_KEY_ALT)
        io.KeyAlt = state;
    if (keycode == TINY_KEY_CONTROL)
        io.KeyCtrl = state;
    if (keycode == TINY_KEY_SHIFT)
        io.KeyShift = state;

//#ifdef _WIN32
    io.KeySuper = false;
//#else
//    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
//#endif

    if (default_keyboard_callback)
    {
        default_keyboard_callback(keycode, state);
    }



}


std::vector<float> wav_data;


int main(int argc, char* argv[]) {
    
    Win32SharedMemory shmem;
	//only the server initializes the shared memory!
	bool allowCreation = true;
	int key = SHARED_MEMORY_KEY;
	int size = sizeof(SharedMemoryBlock);
	SharedMemoryBlock* shared_mem_block = static_cast<SharedMemoryBlock*>(shmem.allocate(key, size, allowCreation));
	if (shared_mem_block)
	{
		std::cout << "server" << std::endl;
		//make sure there isn't already a shared memory server running
		if (shared_mem_block->version_number_ == SHARED_MEMORY_VERSION_NUMBER)
		{
			std::cout << "shared memory server is already running, terminating" << std::endl;
		} else
		{
			shared_mem_block->num_client_commands_ = 0;
			shared_mem_block->num_processed_client_commands_ = 0;
			shared_mem_block->num_server_status_ = 0;
			shared_mem_block->num_processed_server_status_ = 0;
			shared_mem_block->version_number_ = SHARED_MEMORY_VERSION_NUMBER;
			
            TinyOpenGL3App app("visual debugger", 1024, 768);
        
            //TinyChromeUtilsStartTimings();

            TinyClock clock;
            clock.reset();
            double prev_time = clock.get_time_seconds();
       
            app.set_up_axis(2);
    
            default_mouse_move_callback = app.m_window->get_mouse_move_callback();
            app.m_window->set_mouse_move_callback(MyMouseMoveCallback);
            default_mouse_button_callback = app.m_window->get_mouse_button_callback();
            app.m_window->set_mouse_button_callback(MyMouseButtonCallback);
            default_keyboard_callback = app.m_window->get_keyboard_callback();
            app.m_window->set_keyboard_callback(MyKeyboardCallback);
    
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
        
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

            io.KeyMap[ImGuiKey_Tab] = TINY_KEY_TAB;
            io.KeyMap[ImGuiKey_LeftArrow] = TINY_KEY_LEFT_ARROW;
            io.KeyMap[ImGuiKey_RightArrow] = TINY_KEY_RIGHT_ARROW;
            io.KeyMap[ImGuiKey_UpArrow] = TINY_KEY_UP_ARROW;
            io.KeyMap[ImGuiKey_DownArrow] = TINY_KEY_DOWN_ARROW;
            io.KeyMap[ImGuiKey_PageUp] = TINY_KEY_PAGE_UP;
            io.KeyMap[ImGuiKey_PageDown] = TINY_KEY_PAGE_DOWN;
            io.KeyMap[ImGuiKey_Home] = TINY_KEY_HOME;
            io.KeyMap[ImGuiKey_End] = TINY_KEY_END;
            io.KeyMap[ImGuiKey_Insert] = TINY_KEY_INSERT;
            io.KeyMap[ImGuiKey_Delete] = TINY_KEY_DELETE;
            io.KeyMap[ImGuiKey_Backspace] = TINY_KEY_BACKSPACE;
            io.KeyMap[ImGuiKey_Space] = '`';// ' ';
            io.KeyMap[ImGuiKey_Enter] = TINY_KEY_RETURN;
            io.KeyMap[ImGuiKey_Escape] = TINY_KEY_ESCAPE;
            io.KeyMap[ImGuiKey_KeyPadEnter] = 0;
        
            io.KeyMap[ImGuiKey_A] = 'a';
            io.KeyMap[ImGuiKey_C] = 'c';
            io.KeyMap[ImGuiKey_V] = 'v';
            io.KeyMap[ImGuiKey_X] = 'x';
            io.KeyMap[ImGuiKey_Y] = 'y';
            io.KeyMap[ImGuiKey_Z] = 'z';


            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            if (1)
            {
              float SCALE = 2.0f;
              ImFontConfig cfg;
              cfg.SizePixels = 13 * SCALE;
              ImGui::GetIO().Fonts->AddFontDefault(&cfg)->DisplayOffset.y = SCALE;
              ImGui::GetStyle().ScaleAllSizes(SCALE);
            }
            //ImGui::StyleColorsClassic();
            const char* glsl_version = "#version 330";

            // Setup Platform/Renderer bindings
            ImGui_ImplOpenGL3_Init(glsl_version);
            bool res = ImGui_ImplOpenGL3_CreateDeviceObjects();

            imnodes::Initialize();
            //imnodes::SetNodeGridSpacePos(1, ImVec2(200.0f, 200.0f));

            // Setup style

            imnodes::StyleColorsDark();
            //imnodes::StyleColorsClassic();



            while (!app.m_window->requested_exit() && keep_running) 
            {
                B3_PROFILE("mainloop");
				//check if there is any unprocessed commands
				if (shared_mem_block->num_client_commands_ > shared_mem_block->num_processed_client_commands_)
				{
					//process command
					const SharedMemoryCommand& client_cmd = shared_mem_block->client_command_;
					switch (client_cmd.type)
					{
						case CMD_DEBUG_AUDIO:
						{
							int num_float_values = client_cmd.debugArguments.num_float_values;

							std::cout << "received audio data: " << num_float_values << " float values" << std::endl;
                            wav_data.resize(num_float_values);
                            for (int i=0;i<num_float_values;i++)
                            {
                                wav_data[i] = shared_mem_block->float_values_[i];
                            }
                            
							shared_mem_block->server_status_.status_code = STAT_DEBUG_AUDIO_COMPLETED;
							shared_mem_block->num_server_status_++;
							break;
						}
						case CMD_TERMINATE_SERVER:
						{
							std::cout << "terminate server command received" << std::endl;
							shared_mem_block->server_status_.status_code = STAT_TERMINATE_SERVER_COMPLETED;
							shared_mem_block->num_server_status_++;
							keep_running = false;
							break;
						};
						default:
						{
							std::cout << "Unknown command" << std::endl;
						}
					};

					shared_mem_block->num_processed_client_commands_++;
				}
                
                int upAxis = 2;

                DrawGridData data;
                data.upAxis = 2;

                float width = (float)app.m_window->get_width();
                float height = (float)app.m_window->get_height();

                io.DisplaySize = ImVec2((float)width, (float)height);
                io.DisplayFramebufferScale = ImVec2(app.m_window->get_retina_scale(), app.m_window->get_retina_scale());
                double t = clock.get_time_seconds();
                float dt = t - prev_time;
                prev_time = t;
                io.DeltaTime = (float)dt;
                io.MousePos = ImVec2((float)gMouseX, (float)gMouseY);
                io.RenderDrawListsFn = ImGui_ImplOpenGL3_RenderDrawData;// ImGui_ImplBullet_RenderDrawLists;

                for (int i = 0; i < 3; i++)
                {
                    io.MouseDown[i] = g_MousePressed[i];
                }
                io.MouseWheel = g_MouseWheel;

                ImGui::NewFrame();

                ImGui::SetNextWindowSize(ImVec2(800,300));
                if(ImGui::Begin("wave window"))
                {
                    //ImGui::Checkbox("Animate",&animate);

                    // Plots can display overlay texts
                    // (in this example, we will display an average value)
                    if (wav_data.size())
                    {
                        float average = 0.0f;
                        int arrlen = (int)wav_data.size();
                        for(int n = 0; n < arrlen; n++)
                            average += wav_data[n];
                        average /= (float)arrlen;
                        char overlay[32];
                        sprintf(overlay,"len %d",arrlen);
                        //ImGui::PlotLines("Lines",wav_data,arrlen,0,overlay,-1.0f,1.0f,ImVec2(0,180.0f));
                        ImGui::PlotLines("",&wav_data[0],arrlen,0,overlay,-1.0f,1.0f,ImVec2(0,180.0f));
                    } else
                    {
                        ImGui::Text("No Data Received");
                    }
                    
                }
                ImGui::End();
       
                ImGui::Render();
                ImGui::EndFrame();
                {
                    app.swap_buffer();
                }
            }    

		}

	} else
	{
		std::cout << "Cannot create shared memory" << std::endl;
	}
	
	shmem.release(key, size);


#if 0
    int num_samples = 1024;
    wav_data.resize(num_samples);
    for (int i=0;i<num_samples;i++)
    {
        wav_data[i] = 0.8*sin(3.14*2*float(i)/1024);
    }
#endif

    
  //TinyChromeUtilsStopTimingsAndWriteJsonFile("performance_profile.json");

  return 0;
}

