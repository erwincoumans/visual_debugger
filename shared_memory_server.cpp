
#ifdef _WIN32
#include "win32_shared_memory.h"
#else
#include "posix_shared_memory.h"
#endif // _WIN32
#include <iostream>

using namespace visdebug;


bool keep_running = true;

#ifdef _WIN32
#include <Windows.h>

BOOL WINAPI consoleHandler(DWORD signal) {

    if (signal == CTRL_C_EVENT)
	{
		keep_running = false;
        printf("Ctrl-C handled\n"); // do cleanup
	}

    return TRUE;
}
#else
#endif


int main(int argc, char* argv[])
{
#ifdef _WIN32
	 if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        printf("\nERROR: Could not set control handler");
        return 1;
    }
    Win32SharedMemory shmem;
#else

    PosixSharedMemory shmem;
#endif


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

			while (keep_running)
			{
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
			}
		}

	} else
	{
		std::cout << "Cannot create shared memory" << std::endl;
	}

	shmem.release(key, size);
}
