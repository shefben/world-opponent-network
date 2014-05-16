//----------------------------------------------------------------------------
// CRC32.h
//----------------------------------------------------------------------------
#ifndef __CRC32_H__
#define __CRC32_H__

//----------------------------------------------------------------------------
// Prototypes.
//----------------------------------------------------------------------------
void GenerateCRCTable(void);
unsigned long UpdateCRC(unsigned long crc_accum,
						const char *data_blk_ptr,
						int data_blk_size);
unsigned long CalcFileCRC(LPCTSTR sFile, unsigned long nSeed = 0xFFFFFFFF);
unsigned long AbortableCalcFileCRC(LPCTSTR sFile, bool& bAborted, unsigned long nSeed = 0xFFFFFFFF);

#endif
