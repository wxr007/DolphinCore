
#include"define/define.h"
#include"compress/zlib/compress.h"
#include"logging/logging/zLogger.h"
#include "define/common.h"

int compresszlib(unsigned char* pIn, uint64_t nInLen, unsigned char* pOut, uint64_t& pOutLen, int level)
{
	FUNCTION_BEGIN;
	z_stream_s zipStream;
	zipStream.zalloc = Z_NULL;
	zipStream.zfree  = Z_NULL;
	zipStream.opaque = Z_NULL;
	zipStream.next_in   = pIn;
	zipStream.avail_in  = UINT32(nInLen);
	zipStream.total_in  = 0;
	zipStream.next_out  = pOut;
	zipStream.avail_out = UINT32(pOutLen);
	zipStream.total_out = 0;
	int err = 0;
	err = deflateInit(&zipStream, level);

	if(err == Z_OK)
	{
		err = deflate(&zipStream, Z_FINISH);

		if(err == Z_STREAM_END)
		{
			pOutLen = zipStream.total_out;
			err = deflateEnd(&zipStream);
		}
		else
		{
			zipStream.avail_in = zipStream.avail_out = 1;
			int n = 0;

			while(n < 10)
			{
				int tmperr = deflate(&zipStream, Z_FINISH);

				if(tmperr == Z_STREAM_END) break;

				n++;
			}
			if(n >= 10)
			{
				g_logger.forceLog(zLogger::zERROR, "compresszlib error");
			}
			else
			{
				deflateEnd(&zipStream);
			}

			err = (err == Z_OK ? Z_BUF_ERROR : err);
		}
	}

	return err;
}


int uncompresszlib(unsigned char* pIn, uint64_t nInLen, unsigned char* pOut, uint64_t& pOutLen)
{
	FUNCTION_BEGIN;
	z_stream_s zipStream;
	zipStream.zalloc = Z_NULL;
	zipStream.zfree  = Z_NULL;
	zipStream.opaque = Z_NULL;
	zipStream.next_in   = pIn;
	zipStream.avail_in  = UINT32(nInLen);
	zipStream.total_in  = 0;
	zipStream.next_out  = pOut;
	zipStream.avail_out = UINT32(pOutLen);
	zipStream.total_out = 0;
	int err;
	err = inflateInit(&zipStream);

	if(err == Z_OK)
	{
		err = inflate(&zipStream, Z_FINISH);

		if(err == Z_STREAM_END)
		{
			pOutLen = zipStream.total_out;
			err = inflateEnd(&zipStream);
		}
		else
		{
			zipStream.avail_in = zipStream.avail_out = 1;
			int n = 0;

			while(n < 10)
			{
				int tmperr = inflate(&zipStream, Z_FINISH);
				if(tmperr == Z_STREAM_END) break;

				n++;
			}
			if(n >= 10)
			{
				g_logger.forceLog(zLogger::zERROR, "uncompresszlib error");
			}
			else
			{
				inflateEnd(&zipStream);
			}

			err = (err == Z_OK ? Z_BUF_ERROR : err);
		}
	}

	return err;
}
