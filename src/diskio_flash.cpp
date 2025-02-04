/*--------------------------------------------------------------------------
File   : diskio_flash.cpp

Author : Hoang Nguyen Hoan          Aug. 30, 2016

Desc   : Generic flash disk I/O driver class

Copyright (c) 2016, Motsai, all rights reserved

Permission to use, copy, modify, and distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright
notice and this permission notice appear in all copies, and none of the
names : I-SYST or its contributors may be used to endorse or
promote products derived from this software without specific prior written
permission.

For info or contributing contact : nh.hoang at motsai dot com

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------------
Modified by          Date              Description

----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "istddef.h"
#include "diskio_flash.h"
#include "idelay.h"
#include "coredev/spi.h"

FlashDiskIO::FlashDiskIO() : DiskIO()
{
	vpWaitCB = NULL;
	vpInterf = NULL;
}

bool FlashDiskIO::Init(const FLASHDISKIO_CFG &Cfg, DeviceIntrf * const pInterf,
                       DISKIO_CACHE_DESC * const pCacheBlk, int NbCacheBlk)
{
    if (pInterf == NULL)
        return false;

    if (Cfg.pInitCB)
    {
        if (Cfg.pInitCB(Cfg.DevNo, pInterf) == false)
            return false;
    }

    if (Cfg.pWaitCB)
    	vpWaitCB = Cfg.pWaitCB;

    vDevNo          = Cfg.DevNo;
    vSectSize		= Cfg.SectSize;
    vBlkSize		= Cfg.BlkSize;
    if (Cfg.WriteSize == 0)
        vWriteSize = DISKIO_SECT_SIZE;
    else
        vWriteSize = Cfg.WriteSize;
    vTotalSize      = Cfg.TotalSize;
    vAddrSize       = Cfg.AddrSize;
    vRdCmd			= Cfg.RdCmd;
	vWrCmd			= Cfg.WrCmd;
    vpInterf        = pInterf;

    if (pInterf->Type() == DEVINTRF_TYPE_QSPI)
    {
    	SPIDEV *dev = *(SPI*)pInterf;
    	QuadSPISetMemSize(dev, vTotalSize);
    }

    if (Cfg.DevIdSize > 0 && (int)Cfg.DevIdSize > 0)
    {
    	int rtry = 5;

    	do {
    		uint32_t d = ReadId(Cfg.DevIdSize);
    		if (d == Cfg.DevId)
    		{
    			break;
    		}
    	} while (rtry-- > 0);

    	if (rtry <= 0)
    	{
    		return false;
    	}
    }

    if (pCacheBlk && NbCacheBlk > 0)
    {
        SetCache(pCacheBlk, NbCacheBlk);
    }

	return true;
}

uint32_t FlashDiskIO::ReadId(int Len)
{
	uint32_t id = -1;

	if (Len > 0)
	{
		id = 0;

		if (vpInterf->Type() == DEVINTRF_TYPE_QSPI)
		{
			vpInterf->StartRx(vDevNo);
			QuadSPISendCmd(*(SPI*)vpInterf, FLASH_CMD_READID, -1, 0, Len, 0);
			vpInterf->RxData((uint8_t*)&id, Len);
			vpInterf->StopRx();
		}
		else
		{
			uint8_t cmd = FLASH_CMD_READID;
#if 1
			vpInterf->Read(vDevNo, &cmd, 1, (uint8_t*)&id, Len);
#else
			vpInterf->StartRx(vDevNo);
			vpInterf->TxData(&cmd, 1);
			vpInterf->RxData((uint8_t*)&id, Len);
			vpInterf->StopRx();
#endif
		}

	}

    return id;
}

uint8_t FlashDiskIO::ReadStatus()
{
    uint8_t d;

	if (vpInterf->Type() == DEVINTRF_TYPE_QSPI)
    {
		vpInterf->StartRx(vDevNo);
    	QuadSPISendCmd(*(SPI*)vpInterf, FLASH_CMD_READSTATUS, -1, 0, 1, 0);
		vpInterf->RxData((uint8_t*)&d, 1);
		vpInterf->StopRx();
    }
    else
    {
		d = FLASH_CMD_READSTATUS;
		vpInterf->StartRx(vDevNo);
		vpInterf->TxData(&d, 1);
		vpInterf->RxData(&d, 1);
		vpInterf->StopRx();
    }

    return d;
}

bool FlashDiskIO::WaitReady(uint32_t Timeout, uint32_t usRtyDelay)
{
    uint8_t d;

    do {
    	d = ReadStatus();

        if (!(d & FLASH_STATUS_WIP))
            return true;

        if (usRtyDelay > 0)
        {
            if (vpWaitCB)
            	vpWaitCB(vDevNo, vpInterf);
            else
            	usDelay(usRtyDelay);
        }

    } while (Timeout-- > 0);

    return false;
}

void FlashDiskIO::WriteDisable()
{
	if (vpInterf->Type() == DEVINTRF_TYPE_QSPI)
    {
		vpInterf->StartTx(vDevNo);
    	QuadSPISendCmd(*(SPI*)vpInterf, FLASH_CMD_WRDISABLE, -1, 0, 0, 0);
		vpInterf->StopTx();
    }
    else
    {
    	uint8_t d = FLASH_CMD_WRDISABLE;
    	vpInterf->Tx(vDevNo, &d, 1);
    }
}

bool FlashDiskIO::WriteEnable(uint32_t Timeout)
{
    uint8_t d;

    WaitReady(Timeout);

	if (vpInterf->Type() == DEVINTRF_TYPE_QSPI)
    {
		vpInterf->StartTx(vDevNo);
    	QuadSPISendCmd(*(SPI*)vpInterf, FLASH_CMD_WRENABLE, -1, 0, 0, 0);
		vpInterf->StopTx();
    }
    else
    {
    	d = FLASH_CMD_WRENABLE;
    	vpInterf->Tx(vDevNo, &d, 1);
    }
    return false;
}

void FlashDiskIO::Erase()
{
    uint8_t d;

    WriteEnable();
    WaitReady();

	if (vpInterf->Type() == DEVINTRF_TYPE_QSPI)
    {
		vpInterf->StartTx(vDevNo);
    	QuadSPISendCmd(*(SPI*)vpInterf, FLASH_CMD_BULK_ERASE, -1, 0, 0, 0);
		vpInterf->StopTx();
    }
    else
    {
		d = FLASH_CMD_BULK_ERASE;

		vpInterf->Tx(vDevNo, &d, 1);
    }
    // This is a long wait polling at every second only
    WaitReady(-1, 1000000);
    WriteDisable();
}

/**
 * Erase Flash block.
 *
 * @param   BlkNo   : Starting block number to erase.
 *          NbBlk   : Number of consecutive blocks to erase
 */
void FlashDiskIO::EraseBlock(uint32_t BlkNo, int NbBlk)
{
    uint8_t d[8];
    uint32_t addr = BlkNo * vBlkSize * 1024;
    uint8_t *p = (uint8_t*)&addr;

    d[0] = FLASH_CMD_BLOCK_ERASE;

    for (int k = 0; k < NbBlk; k++)
    {
        WaitReady(-1, 100);

        // Need to re-enable write here, because some flash
        // devices may reset write enable after a write
        // complete
        WriteEnable();

    	if (vpInterf->Type() == DEVINTRF_TYPE_QSPI)
        {
    		vpInterf->StartTx(vDevNo);
    		QuadSPISendCmd(*(SPI*)vpInterf, FLASH_CMD_BLOCK_ERASE, addr, vAddrSize, 0, 0);
    		vpInterf->StopTx();
        }
        else
        {
            for (int i = 1; i <= vAddrSize; i++)
                d[i] = p[vAddrSize - i];
        	vpInterf->Tx(vDevNo, d, vAddrSize + 1);
        }
        addr += vBlkSize * 1024;
    }
    WaitReady(-1, 1000000);
    WriteDisable();
}

/**
 * Erase Flash sector.
 *
 * @param   SectNo   : Starting block number to erase.
 *          NbSect   : Number of consecutive blocks to erase
 */
void FlashDiskIO::EraseSector(uint32_t SectNo, int NbSect)
{
    uint8_t d[8];
    uint32_t addr = SectNo * vSectSize * 1024;
    uint8_t *p = (uint8_t*)&addr;

    d[0] = FLASH_CMD_SECTOR_ERASE;

    for (int k = 0; k < NbSect; k++)
    {
        WaitReady(-1, 100);

        // Need to re-enable write here, because some flash
        // devices may reset write enable after a write
        // complete
        WriteEnable();

    	if (vpInterf->Type() == DEVINTRF_TYPE_QSPI)
        {
    		vpInterf->StartTx(vDevNo);
    		QuadSPISendCmd(*(SPI*)vpInterf, FLASH_CMD_SECTOR_ERASE, addr, vAddrSize, 0, 0);
    		vpInterf->StopTx();
        }
        else
        {
            for (int i = 1; i <= vAddrSize; i++)
                d[i] = p[vAddrSize - i];
        	vpInterf->Tx(vDevNo, d, vAddrSize + 1);
        }
        addr += vSectSize * 1024;
    }
    WaitReady(-1, 1000000);
    WriteDisable();
}

/**
 * Read one sector from physical device
 */
bool FlashDiskIO::SectRead(uint32_t SectNo, uint8_t *pBuff)
{
   	uint8_t d[9];
    uint32_t addr = SectNo * DISKIO_SECT_SIZE;
    uint8_t *p = (uint8_t*)&addr;
    int cnt = DISKIO_SECT_SIZE;

    // Makesure there is no write access pending
    WaitReady(100000);

    if (vpInterf->Type() == DEVINTRF_TYPE_QSPI)
    {
		vpInterf->StartRx(vDevNo);
    	QuadSPISendCmd(*(SPI*)vpInterf, vRdCmd.Cmd , addr, vAddrSize, DISKIO_SECT_SIZE, vRdCmd.DummyCycle);
		int l = vpInterf->RxData(pBuff, DISKIO_SECT_SIZE);
		vpInterf->StopRx();
    }
    else
    {
    	d[0] = FLASH_CMD_READ;
		while (cnt > 0)
		{
			for (int i = 1; i <= vAddrSize; i++)
				d[i] = p[vAddrSize - i];

			vpInterf->StartRx(vDevNo);
			vpInterf->TxData((uint8_t*)d, vAddrSize + 1);
			int l = vpInterf->RxData(pBuff, DISKIO_SECT_SIZE);
			vpInterf->StopRx();
			if (l <= 0)
				return false;
			cnt -= l;
			addr += l;
			pBuff += l;
		}
    }

    return true;
}

/**
 * Write one sector to physical device
 */
bool FlashDiskIO::SectWrite(uint32_t SectNo, uint8_t *pData)
{
    uint8_t d[9];
    uint32_t addr = SectNo * DISKIO_SECT_SIZE;
    uint8_t *p = (uint8_t*)&addr;

    int cnt = DISKIO_SECT_SIZE;

    if (vpInterf->Type() == DEVINTRF_TYPE_QSPI)
    {
		while (cnt > 0)
		{
			int l = min(cnt, vWriteSize);

			WriteEnable();
			vpInterf->StartTx(vDevNo);

			QuadSPISendCmd(*(SPI*)vpInterf, vWrCmd.Cmd, addr, vAddrSize, l, vWrCmd.DummyCycle);

			l = vpInterf->TxData(pData, l);
			vpInterf->StopTx();
			cnt -= l;
			pData += l;
			addr += l;
		}
    }
    else
    {
		d[0] = FLASH_CMD_WRITE;

		while (cnt > 0)
		{
			for (int i = 1; i <= vAddrSize; i++)
				d[i] = p[vAddrSize - i];

			int l = min(cnt, vWriteSize);

			WaitReady();

			// Some Flash will reset write enable bit at completion
			// when page size is less than 512 bytes.
			// We need to set it again
			WriteEnable();

			vpInterf->StartTx(vDevNo);
			vpInterf->TxData((uint8_t*)d, vAddrSize + 1);
			l = vpInterf->TxData(pData, l);
			vpInterf->StopTx();
			if (l <= 0)
				return false;
			cnt -= l;
			pData += l;
			addr += l;
		}
    }

	WriteDisable();

	return true;
}


