/*
 * DrvFs.h
 *
 *  Created on: 2013-8-1
 *      Author: Administrator
 */

#ifndef DRVFS_H_
#define DRVFS_H_

#include "Driver.h"
#include "SPI_SD_driver.h"
#include "tff.h"

class DrvFs : public Driver {
public:
	DrvFs(SPI_SDCard_Device *device);
	virtual ~DrvFs();
	int init(void);
	bool open(char *file_name);
	void close(void);
	void read(void* c, UINT len, UINT *current_len);
	void lseek(DWORD seek);
	void puts(const char *buf);
	void printf (const char *buf, ...);
	void mkdir (const char *dir_name);
	void rename (const char *file_name, const char *file_rename);
	void unlink (const char *file_name);
private:
	SPI_SDCard_Device *_device;
	FATFS _fat;
	FIL _file;
	const static BYTE _OPEN_MODE = FA_OPEN_ALWAYS | FA_READ | FA_WRITE;
};

#endif /* DRVFS_H_ */
