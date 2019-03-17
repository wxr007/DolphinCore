#include "define/define.h"
#include "mem/lookaside_alloc.h"

//#include "logging/logging/zLogger.h"

using namespace std;//方法和类 都包含在了命令空间 std 里面

//TODO: visual studio cldinitseg.cpp 49 进行了初始化 不是在类的cpp 进行初始化这样是不好的
//静态变量必须初始化才能使用 最好在类的cpp 进行初始化  我是visual studio
std::allocator< char > CSimpleAllocator::_ty_alloc_0_128;
safe_lookaside_allocator< char[256], 64 > CSimpleAllocator::_ty_alloc_128;
safe_lookaside_allocator< char[512], 48 > CSimpleAllocator::_ty_alloc_256;
safe_lookaside_allocator< char[512*2], 32 > CSimpleAllocator::_ty_alloc_512;
safe_lookaside_allocator< char[512*3], 16 > CSimpleAllocator::_ty_alloc_512x2;
safe_lookaside_allocator< char[512*4], 16 > CSimpleAllocator::_ty_alloc_512x3;
safe_lookaside_allocator< char[512*5], 16 > CSimpleAllocator::_ty_alloc_512x4;
CSimpleAllocator CSimpleAllocator::_ty_alloc;

size_t gLookasideAllocSize = 0;


char* CSimpleAllocator::getmem(int n)
{
	return allocate(n);
}
void CSimpleAllocator::freemem(void* p)
{
	deallocate(p);
}

char* CSimpleAllocator::allocate(int n)
{
	int nbufsize = (int)ROUNDNUM2(n + 1, 4);
	uint8_t* pret = NULL;

	if(nbufsize <= 128)
	{
		pret = (uint8_t*)(_ty_alloc_0_128.allocate((uint8_t)nbufsize, NULL));
		*pret = (uint8_t)nbufsize;
	}
	else if(nbufsize <= 256)
	{
		pret = (uint8_t *)LOOKASIDE_GETMEM(_ty_alloc_128);
		*pret = (uint8_t)(128 + 1);
	}
	else if(nbufsize <= 512)
	{
		pret = (uint8_t *)LOOKASIDE_GETMEM(_ty_alloc_256);
		*pret = (uint8_t)(128 + 2);
	}
	else if(nbufsize <= 512 * 2)
	{
		pret = (uint8_t *)LOOKASIDE_GETMEM(_ty_alloc_512);
		*pret = (uint8_t)(128 + 3);
	}
	else if(nbufsize <= 512 * 3)
	{
		pret = (uint8_t *)LOOKASIDE_GETMEM(_ty_alloc_512x2);
		*pret = (uint8_t)(128 + 4);
	}
	else if(nbufsize <= 512 * 4)
	{
		pret = (uint8_t *)LOOKASIDE_GETMEM(_ty_alloc_512x3);
		*pret = (uint8_t)(128 + 5);
	}
	else if(nbufsize <= 512 * 5)
	{
		pret = (uint8_t *)LOOKASIDE_GETMEM(_ty_alloc_512x4);
		*pret = (uint8_t)(128 + 6);
	}
	else
	{
		pret = (uint8_t *)malloc(nbufsize);
		*pret = (uint8_t)(0xff);
	}

	pret++;
	return (char*)pret;
}

void CSimpleAllocator::deallocate(void* p)
{
	uint8_t* pret = (uint8_t*)p;
	pret--;
	int nbufsize = (uint8_t)(*pret);

	switch(nbufsize)
	{
	case(128+1):
		_ty_alloc_128.freemem(pret);
		break;
	case(128+2):
		_ty_alloc_256.freemem(pret);
		break;
	case(128+3):
		_ty_alloc_512.freemem(pret);
		break;
	case(128+4):
		_ty_alloc_512x2.freemem(pret);
		break;
	case(128+5):
		_ty_alloc_512x3.freemem(pret);
		break;
	case(128+6):
		_ty_alloc_512x4.freemem(pret);
		break;
	case 0xff:
		free(pret);
		break;
	default:
		{
			if(nbufsize <= 128)
			{
				_ty_alloc_0_128.deallocate((char*)pret, nbufsize);
			}
			else
			{
//				g_logger.error("CSimpleAllocator::deallocate:错误的指针!");
			}
		}
		break;
	}
}
