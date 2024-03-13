		org 0x7C00
		bits 16

		[map all boot.map]

%define FAT_SEG		0x1000
%define ROOT_SEG	0x1400
%define FILE_SEG	0x2000


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


; PrintString
;  - Read character from ds:si and print if it is not null termination

PrintString:

		lodsb
		or		al, al
		jz		.printDone

		mov		ah, 0xE
		int		0x10
		jmp		PrintString

.printDone:
		ret

; LBA2CHS
;  - Convert LBA to CHS location

LBA2CHS:
		xor		dx, dx

		div		word [bpbSectorsPerTrack]
		inc		dl
		mov		byte [sector], dl

		xor		dx, dx
		div		word [bpbNumberOfHeads]
		mov		byte [head], dl

		mov		byte [cylinder], al

		ret

; ReadSectors
; - Read sectors from disk
;   AL - amount of sectors to Read

ReadSectors:
		mov		ah, 2

		mov		ch, [cylinder]
		mov		cl, [sector]
		mov		dh, [head]
		mov		dl, [driveNumber]

		int		0x13

		ret


start:
		; Setup segment registers
		xor		ax, ax
		mov		ds, ax
		mov		es, ax

		; Set stack pointer at 0x8FFFF
		mov		ax, 0x8000
		mov		ss, ax
		mov		sp, 0xFFFF

		; Disable interrupts
		cli

		; Store drivenumber
		mov		[driveNumber], dl

		; Print loading string
		mov		si, msgLoading
		call	PrintString

		; BIOS only loads first sector, if there are more,
		; they needs to be loaded here!

		mov		cx, word [bpbReservedSectors]
		dec		cx

		; Load into 0x7E00 and onwards
		mov		bx, 0x7E00
		mov		dx, 1

.loadBootsector:
		cmp		cx, 0
		je		.loadFAT

		push	cx

		mov		ax, dx
		call	LBA2CHS

		mov		al, 1
		call	ReadSectors

		pop		cx

		dec		cx

		add		bx, 0x200
		inc		dx

		jmp		.loadBootsector

.loadFAT:
		; Load FAT entry into 0x10000

		mov		ax, FAT_SEG
		mov		es, ax

		; FAT table 1 starts after the reserved sectors
		mov		ax, [bpbReservedSectors]
		call	LBA2CHS

		; Number of sectors to read
		mov		al, [bpbSectorsPerFAT]

		; Read to es:bx (0x1000:0x0000)
		xor		bx, bx
		call	ReadSectors

		jnc		.findRootDirectory

		hlt

.findRootDirectory:

		; Load root directory into 0x14000

		mov		ax, ROOT_SEG
		mov		es, ax

		; Root directory location = reserved sectors + (sectors per fat * 2)
		mov		ax, [bpbSectorsPerFAT]
		shl		ax, 1
		add		ax, [bpbReservedSectors]
		call	LBA2CHS

		; Sector count equals to max root directories * entry length (32) divided by sector size
		; Here hardcoded to 14
		mov		al, 14

		; Read to es:bx (0x1400:0x0000)
		xor		bx, bx
		call	ReadSectors

		jnc		.findFileEntry

		hlt

.findFileEntry:

		mov		cx, word [bpbMaxRootDirectories]
		xor		di, di

.checkFilename:

		push	cx
		push	di

		mov		si, loaderFilename
		mov		cx, 11
		rep cmpsb

		je		.foundEntry

		pop		di
		pop		cx

		add		di, 0x20
		loop	.checkFilename

		mov		si, msgFileNotFound
		call	PrintString

		jmp		$

.foundEntry:
		pop		di
		pop		cx

		; Find first FAT entry location
		add		di, 0x1A
		mov		ax, word [es:di]

.loadFile:
		; FAT entry to read in ax		

		; Load into 0x2000:0x0000

		mov		bx, FILE_SEG
		mov		es, bx

		xor		bx, bx

.readFileSector:
		push	ax

		; Data block starts at sector 31 in a FAT12 environment
		add		ax, word [bpbReservedSectors]
		add		ax, 30
		call	LBA2CHS

		; Read the sector
		mov		al, 1
		call	ReadSectors

		pop		ax

		; Increase buffer pointer
		add		bx, word [bpbBytesPerSector]

.getNextEntry:

		push	bx
		push	ds

		mov		dx, FAT_SEG
		mov		ds, dx

		; Store sector position in bx
		mov		bx, ax
		shr		ax, 1
		add		ax, bx

		; Get next sector into ax
		mov		si, ax
		mov		ax, word [ds:si]

		pop		ds

		test	bx, 1
		jne		.evenEntry

.oddEntry:
		and		ax, 0x0FFF
		jmp		.fatOK

.evenEntry:
		shr		ax, 4

.fatOK:
		; Check for EOF

		pop		bx

		mov		dx, ax

		and		dx, 0x0FF0
		cmp		dx, 0x0FF0
		jne		.readFileSector

		jmp		stage2

		jmp		$



; Data

%define CRLF	13, 10

msgLoading: 	db	"Loading...", CRLF, 0
msgProgress:	db  ".", 0
msgFileNotFound: db "File not found", CRLF, 0

loaderFilename:	db	"KERNEL  SYS"
;loaderFilename:	db	"HELLO   TXT"

sector:			db	0
head:			db	0
cylinder:		db	0
driveNumber:	db	0

times 510-($-$$)	db	0

		dw		0xAA55


; 32-bit promotion code

%include "a20.inc"

stage2:

		call	EnableA20

		lgdt	[gdtStruct]

		mov		eax, cr0
		or		eax, 1
		mov		cr0, eax

		jmp		0x08:pmode

		bits	32

pmode:
		
		mov		ax, 0x10
		mov		ds, ax
		mov		es, ax
		mov		fs, ax
		mov		gs, ax

		mov		ss, ax
		mov		esp, 0x90000

		mov		esi, 0x20000

		xor		ecx, ecx

.parseElfImage:
		; Barebone plain simple data readout and copy

		; Program entry address
		mov		eax, [esi+0x18]

		push	eax

		; Start of program header table
		mov		edx, [esi+0x20]

		; Number of program header entries
		mov		cx, word [esi+0x38]
		mov		[ELFProgramHeaderEntries], cx

		; Number of section header entries
		mov		cx, word [esi+0x3C]
		mov		[ELFSectionHeaderEntries], cx

		; Set esi to start of program header table
		mov		esi, 0x20000
		mov		ebx, esi
		add		esi, edx

		mov		cx, [ELFProgramHeaderEntries]

.readProgramHeader:
		mov		[ELFProgramHeaderEntries], cx

;		push	esi

		mov		eax, [esi]
		cmp		eax, 1

		jne		.programHeaderDone

		; Offset in file
		add		ebx, [esi+0x08]

		; Physical address
		mov		edi, [esi+0x18]

		; File size
		mov		ecx, [esi+0x20]

.copyProgramHeader:
		push	esi
		mov		esi, ebx
		rep		movsb
		pop		esi

		mov		cx, word [ELFProgramHeaderEntries] 

		jmp		.programHeaderDone	

.programHeaderDone:

		add		esi, 0x38		; Hardcoded length of program header

		dec		cx

		cmp		cx, 0
		jne		.readProgramHeader

.doneCopyElf:



.jump32bit:
		mov		bx, BIOSParamBlock
		mov		dl, byte [driveNumber]

		pop		eax

		jmp		eax

		
		jmp		$


gdtData:
		; Null descriptor
		dd		0
		dd		0

		; Kernel code descriptor
		dw		0xFFFF
		dw		0
		db		0
		db		0x9A
		db		0xCF
		db		0

		; Kernel data descriptor
		dw		0xFFFF
		dw		0
		db		0
		db		0x92
		db		0xCF
		db		0

endGdtData:

gdtStruct:
		; Limit (size of GDT)
		dw		endGdtData - gdtData - 1
		dd		gdtData

ELFProgramHeaderEntries:	dw		0
ELFSectionHeaderEntries:	dw		0
