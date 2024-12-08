#ifndef _VBE_H
#define _VBE_H

#include "types.h"

typedef union vbe_ptr {
	 uint32    Ptr32;
	 void __far*  Ptr;
	 union {
		  uint16    Off;
		  uint16    Seg;
	 };
} vbe_ptr;

typedef struct VbeInfoBlock
{
	union        {
		 uint8      SigChr[4];
		 uint32     Sig32;
	}            VbeSignature;
	uint16     VbeVersion;
	vbe_ptr      OemString;
	uint8      Capabilities[4];
	uint16     VideoModePtr_Off;
	uint16     VideoModePtr_Seg;
	uint16     TotalMemory;
	uint16     OemSoftwareRev;
	vbe_ptr      OemVendorName;
	vbe_ptr      OemProductName;
	vbe_ptr      OemProductRev;
	uint16     Reserved[111];
	uint8      OemData[256];
} VbeInfoBlock;


typedef struct ModeInfoBlock
{
	uint16 ModeAttributes;
	uint8  WinAAttributes;
	uint8  WinBAttributes;
	uint16 WinGranularity;
	uint16 WinSize;
	uint16 WinASegment;
	uint16 WinBSegment;
	uint32 WinFuncPtr;
	uint16 BytesPerScanLine;

	uint16 XResolution;
	uint16 YResolution;
	uint8  XCharSize;
	uint8  YCharSize;
	uint8  NumberOfPlanes;
	uint8  BitsPerPixel;
	uint8  NumberOfBanks;
	uint8  MemoryModel;
	uint8  BankSize;
	uint8  NumberOfImagePages;
	uint8  Reserved_page;

	uint8  RedMaskSize;
	uint8  RedFieldPosition;
	uint8  GreenMaskSize;
	uint8  GreenFieldPosition;
	uint8  BlueMaskSize;
	uint8  BlueFieldPosition;
	uint8  RsvdMaskSize;
	uint8  RsvdFieldPosition;
	uint8  DirectColorModeInfo;

	uint32 PhysBasePtr;
	uint32 OffScreenMemOffset;
	uint16 OffScreenMemSize;

	uint16 LinBytesPerScanLine;
	uint8  BnkNumberOfPages;
	uint8  LinNumberOfPages;
	uint8  LinRedMaskSize;
	uint8  LinRedFieldPosition;
	uint8  LinGreenMaskSize;
	uint8  LinGreenFieldPosition;
	uint8  LinBlueMaskSize;
	uint8  LinBlueFieldPosition;
	uint8  LinRsvdMaskSize;
	uint8  LinRsvdFieldPosition;
	uint32 MaxPixelClock;
	uint8  Reserved[189];
} ModeInfoBlock;


#define VBE_RETURN_STATUS_SUPPORTED                      0x4F
#define VBE_RETURN_STATUS_UNSUPPORTED                    0x00

#define VBE_RETURN_STATUS_SUCCESSFULL                    0x00
#define VBE_RETURN_STATUS_FAILED                         0x01
#define VBE_RETURN_STATUS_NOT_SUPPORTED                  0x02
#define VBE_RETURN_STATUS_INVALID                        0x03


VbeInfoBlock* VBEGetGeneralInfo(void);

ModeInfoBlock* VBEGetModeInfo(uint32 mode);

bool VBESetMode(uint32 mode);

bool VBESetup(uint16 w, uint16 h);

#endif