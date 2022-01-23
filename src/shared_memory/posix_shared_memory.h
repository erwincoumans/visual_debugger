#ifndef POSIX_SHARED_MEMORY_H
#define POSIX_SHARED_MEMORY_H

#include "shared_memory_interface.h"
namespace visdebug
{

class PosixSharedMemory : public SharedMemoryInterface
{
	struct PosixSharedMemoryInteralData* m_internalData;

public:
	PosixSharedMemory();
	virtual ~PosixSharedMemory();

	virtual void* allocate(int key, int size, bool allowCreation);
	virtual void release(int key, int size);
};

};

#endif  //
