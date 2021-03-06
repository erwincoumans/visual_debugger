
#ifdef _WIN32
#include "win32_shared_memory.h"
#else
#include "posix_shared_memory.h"
#endif
#include <iostream>
#include <math.h>

using namespace visdebug;



int main(int argc, char* argv[])
{
#ifdef _WIN32
	Win32SharedMemory shmem;
#else
    PosixSharedMemory shmem;
#endif
	//only the server initializes the shared memory, not the client!
	bool allowCreation = false;
	int key = SHARED_MEMORY_KEY;
	int size = sizeof(SharedMemoryBlock);
	SharedMemoryBlock* shared_mem_block = static_cast<SharedMemoryBlock*>(shmem.allocate(key, size, allowCreation));
	if (shared_mem_block)
	{
		if (shared_mem_block->version_number_ == SHARED_MEMORY_VERSION_NUMBER)
		{
			std::cout << "client" << std::endl;
			if (shared_mem_block->num_client_commands_ > shared_mem_block->num_processed_client_commands_)
			{
				std::cout << "server is busy processing outstanding commands" << std::endl;
			} else
			{
				//send a server terminate command
				//shared_mem_block->client_commands_[0].type = CMD_TERMINATE_SERVER;
				//shared_mem_block->num_client_commands_++;

				shared_mem_block->client_command_.type = CMD_DEBUG_AUDIO;
				int num_float_values = 1024;

				shared_mem_block->client_command_.debugArguments.num_float_values = num_float_values;

				//send the actual audio data
				for (int i=0;i<num_float_values;i++)
				{
					shared_mem_block->float_values_[i] = 0.8*sin(3.14*2*float(i)/1024);
				}
				shared_mem_block->num_client_commands_++;

				//wait for completion?
				while (shared_mem_block->num_client_commands_ > shared_mem_block->num_processed_client_commands_)
				{
					//todo: yield/sleep while waiting
				}
				if (shared_mem_block->num_server_status_ > shared_mem_block->num_processed_server_status_)
				{
					switch (shared_mem_block->server_status_.status_code)
					{
						case STAT_DEBUG_AUDIO_COMPLETED:
						{
							std::cout << "STAT_DEBUG_AUDIO_COMPLETED" << std::endl;
							break;
						}
						case STAT_DEBUG_AUDIO_FAILED:
						{
							std::cout << "STAT_DEBUG_AUDIO_FAILED" << std::endl;
							break;
						}

						case STAT_TERMINATE_SERVER_COMPLETED:
						{
							std::cout << "STAT_TERMINATE_SERVER_COMPLETED" << std::endl;
							break;
						};
						case STAT_TERMINATE_SERVER_FAILED:
						{
							std::cout << "STAT_TERMINATE_SERVER_FAILED" << std::endl;
							break;
						}
						default:
						{
						}
					}

				}
			}
		} else
		{
			std::cout << "invalid server version, expected " << SHARED_MEMORY_VERSION_NUMBER << " and got " << shared_mem_block->version_number_ << std::endl;
		}
	} else
	{
		std::cout << "Cannot connect to shared memory, is the server running?" << std::endl;
	}

	shmem.release(key, size);
}
