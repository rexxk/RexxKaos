
        SECTION .strapcode

        bits 32

; - Read sectors from disk
;   AL - amount of sectors to Read

ReadSector:

		mov		ah, 2

        mov     ecx, cylinder
		mov		ch, [cylinder]
		mov		cl, [sector]
		mov		dh, [head]
		mov		dl, [driveNumber]

		int		0x13

        ret



; LBA2CHS
;  - Convert LBA to CHS location

LBA2CHS:

		xor		dx, dx

		div		word [bpbSectorsPerTrack]
		inc		dl
		mov		word [sector], dx

		xor		dx, dx
		div		word [bpbNumberOfHeads]
		mov		byte [head], dl

		mov		byte [cylinder], al

		ret

        SECTION .strapdata

sector:			dd	0
head:			dd	0
cylinder:		dd	0

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
