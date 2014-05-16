//----------------------------------------------------------------------------
// CRC32.h
//
// Quick usage notes:
// 
// Call gen_crc_table() before calculating any CRCs
// 
// I read somewhere that the "standard" CRC32 uses 0xFFFFFFFF as the seed
// for CRCs. This is mostly useful if compatability with other apps is desired.
// I seem to remember reading that using zero as a seed produces less random
// CRCs.
//----------------------------------------------------------------------------
#define VC_EXTRALEAN
#include <windows.h>
#include "crc32.h"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define POLYNOMIAL 0x04c11db7L

static BOOL 	     bCrcTableGenerated = FALSE;
static unsigned long crc_table[256];


//----------------------------------------------------------------------------
// Generate the table of CRC remainders for all possible bytes.
//----------------------------------------------------------------------------
void GenerateCRCTable(void)
{
	bCrcTableGenerated = TRUE;

	register int i, j;
	register unsigned long crc_accum;
	for (i = 0;  i < 256;  i++)
	{
		crc_accum = ((unsigned long) i << 24);
		for ( j = 0;  j < 8;  j++ )
		{
			if (crc_accum & 0x80000000L)
				crc_accum = (crc_accum << 1) ^ POLYNOMIAL;
			else
				crc_accum = (crc_accum << 1);
		}
		crc_table[i] = crc_accum;
	}
}


//----------------------------------------------------------------------------
// Update the CRC on the data block one byte at a time.
//----------------------------------------------------------------------------
unsigned long UpdateCRC(unsigned long crc_accum,
						const char *data_blk_ptr,
						int data_blk_size)
{
	register int i, j;
	for (j = 0; j < data_blk_size; j++)
	{
		i = ((int) (crc_accum >> 24) ^ *data_blk_ptr++) & 0xff;
		crc_accum = (crc_accum << 8) ^ crc_table[i];
	}
	return crc_accum;
}


//----------------------------------------------------------------------------
// Calculate the CRC on a file.
//----------------------------------------------------------------------------
unsigned long CalcFileCRC(LPCTSTR sFile, unsigned long nSeed)
{
	if (! bCrcTableGenerated)
		GenerateCRCTable();

	const int MAX_BUF_SIZE = 32768;

	if (! sFile)
		return 0;

	HANDLE hFile = CreateFile(sFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
							  OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE) //lint !e1924
		return 0;

	char pBuf[MAX_BUF_SIZE];
	unsigned long nFileSize = GetFileSize(hFile, NULL);
	unsigned long nCRC = nSeed;
	unsigned long nRead = 0;
	unsigned long nTotalRead = 0;

	while (::ReadFile(hFile, pBuf, MAX_BUF_SIZE, &nRead, NULL) && nRead > 0)
	{
		nTotalRead += nRead;
		nCRC = UpdateCRC(nCRC, pBuf, static_cast<int>(nRead));
	}
	CloseHandle(hFile);

	if (nTotalRead != nFileSize)
		return 0;

	return nCRC;
}

//----------------------------------------------------------------------------
// An Abortable version of the file CRC calculation.
//----------------------------------------------------------------------------
unsigned long AbortableCalcFileCRC(LPCTSTR sFile, bool& bAborted, unsigned long nSeed)
{
	if (! bCrcTableGenerated)
		GenerateCRCTable();

	const int MAX_BUF_SIZE = 32768;

	if (! sFile)
		return 0;

	HANDLE hFile = CreateFile(sFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
							  OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE) //lint !e1924
		return 0;

	char pBuf[MAX_BUF_SIZE];
	unsigned long nFileSize = GetFileSize(hFile, NULL);
	unsigned long nCRC = nSeed;
	unsigned long nRead = 0;
	unsigned long nTotalRead = 0;

	while ((! bAborted) && ::ReadFile(hFile, pBuf, MAX_BUF_SIZE, &nRead, NULL) && nRead > 0)
	{
		nTotalRead += nRead;
		nCRC = UpdateCRC(nCRC, pBuf, static_cast<int>(nRead));
	}
	CloseHandle(hFile);

	if (nTotalRead != nFileSize)
		return 0;

	if (bAborted)
		return 0;
	return nCRC;
}
