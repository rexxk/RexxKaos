		org 0x7C00
		bits 16

entry:
		jmp short start
		nop

; BIOS param block
BIOSParamBlock:
bpbOEMName: 			db	"        "
bpbBytesPerSector: 		dw	0
bpbSectorsPerCluster:	db	0
bpbReservedSectors:		dw	0
bpbNumberOfFATs:		db	0
bpbMaxRootDirectories:	dw	0
bpbTotalSectors:		dw	0
bpbMediaDescriptor:		db	0
bpbSectorsPerFAT:		dw	0
bpbSectorsPerTrack:		dw	0
bpbNumberOfHeads:		dw	0
bpbHiddenSectors:		dd	0
bpbTotalSectorsLarge:	dd	0
bpbDriveNumber:			db	0
bpbFlagsWindowsNT:		db	0
bpbBootSignature:		db	0
bpbVolumeID:			dd	0
bpbVolumeLabel:			db	"           "
bpbFilesystem:			db	"        "




start:
		xor		ax, ax
		mov		ds, ax
		mov		es, ax

		mov		ss, ax
		mov		sp, 0x7C00

		cli
		
		hlt



times 510-($-$$)	db	0

		dw		0xAA55
