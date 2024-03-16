		org 0x7C00
		bits 16

		[map all boot.map]

%define FAT_SEG			0x1000
%define ROOT_SEG		0x1400
%define FILE_SEG		0x2000
%define FILE_ADDRESS	0x20000

%define PAGE_MAP_ADDRESS	0x1000

%define DATA_STRUCT_ADDRESS	0x0500

%define PAGE_PRESENT	(1 << 0)
%define PAGE_WRITE		(1 << 1)

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

; Get memory maps
		int		0x12

		mov		word [MemoryData.LowMemory], ax

		xor		ax, ax
		mov		es, ax

		mov		di, MemoryData.HighMemAddress
		xor		ebx, ebx
		xor		si, si

.getNextMemoryBlock:
		mov		edx, 0x534D4150
		mov		eax, 0xE820
		mov		ecx, 24

		int		0x15

		cmp		ebx, 0x0
		je		.readMemoryBlocksDone

		add		di, cx
		inc		si

		jmp		.getNextMemoryBlock

.readMemoryBlocksDone:
		mov		word [MemoryData.EntryCount], si

; Entring 64-bit longmode
		xor		ax, ax
		mov		es, ax
		mov		edi, PAGE_MAP_ADDRESS

		; Clear page map, 16 kb (0x4000 bytes)
.clearPageMap:
		push	di
		mov		ecx, 0x1000
		xor		eax, eax
		cld
		rep		stosd

		pop		di

		; Build Page Map Level 4
.buildPML:
		; Pointer to PDPT into eax
		lea		eax, [es:di + 0x1000]
		or		eax, PAGE_PRESENT | PAGE_WRITE
		mov		[es:di], eax

		; Build Page Directory Pointer Table
.buildPDPT:
		; Address to Page Directory into eax
		lea		eax, [es:di + 0x2000]
		or		eax, PAGE_PRESENT | PAGE_WRITE
		mov		[es:di + 0x1000], eax

		; Build Page Directory
.buildPD:
		; Address to Page Table into eax
		lea		eax, [es:di + 0x3000]
		or		eax, PAGE_PRESENT | PAGE_WRITE
		mov		[es:di + 0x2000], eax

		push	di
		; Point to page table
		lea		di, [di + 0x3000]
		mov		eax, PAGE_PRESENT | PAGE_WRITE

		; Build the page table
.loopPageTable:
		mov		[es:di], eax
		add		eax, 0x1000
		add		di, 8
		cmp		eax, 0x200000
		jb		.loopPageTable

		pop		di

		; Setup descriptor tables

		; Disable IRQs
		mov		al, 0xFF
		out		0xA1, al
		out		0x21, al

		nop
		nop

		lidt	[idt]

		; Enter long mode

		; Set PAE and PGE bits
		mov		eax, 10100000b
		mov		cr4, eax

		; PML4 into CR3
		mov		edx, edi
		mov		cr3, edx

		mov		ecx, 0xC0000080
		rdmsr

		or		eax, 0x00000100
		wrmsr

		mov		ebx, cr0
		or		ebx, 0x80000001
		mov		cr0, ebx

; End of new 64-bit entry code

		lgdt	[gdtStruct]

		jmp		0x08:longmode

		bits	64

longmode:
		
		mov		ax, 0x10
		mov		ds, ax
		mov		es, ax
		mov		fs, ax
		mov		gs, ax

		mov		ss, ax
		mov		esp, 0x90000

		mov		esi, FILE_ADDRESS

		xor		ecx, ecx

.parseElfImage:
		; Barebone plain simple data readout and copy

		; Program entry address
		mov		eax, [esi+0x18]

		push	rax

		; Start of program header table
		mov		edx, [esi+0x20]

		; Number of program header entries
		mov		cx, word [esi+0x38]
		mov		[ELFProgramHeaderEntries], cx

		; Number of section header entries
		mov		cx, word [esi+0x3C]
		mov		[ELFSectionHeaderEntries], cx

		; Set esi to start of program header table
		mov		esi, FILE_ADDRESS
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
		push	rsi
		mov		esi, ebx
		rep		movsb
		pop		rsi

		mov		cx, word [ELFProgramHeaderEntries] 

		jmp		.programHeaderDone	

.programHeaderDone:

		add		esi, 0x38		; Hardcoded length of program header

		dec		cx

		cmp		cx, 0
		jne		.readProgramHeader

.doneCopyElf:



.jump32bit:

		pop		rax

		push	rbx
		push	DATA_STRUCT_ADDRESS
		push	rdx

		jmp		rax

		
		jmp		$


		ALIGN 4

idt:
.Length:	dw	0
.Base:		dd	0

		ALIGN 4

gdtData:
		dq		0

		; Kernel code descriptor
		dq		0x00209A0000000000

		; Kernel data descriptor
		dq		0x0000920000000000

endGdtData:

gdtStruct:
		; Limit (size of GDT)
		dw		endGdtData - gdtData - 1
		dd		gdtData

ELFProgramHeaderEntries:	dw		0
ELFSectionHeaderEntries:	dw		0


MemoryData:
.LowMemory:			dw		0
.EntryCount:		dw		0
.HighMemAddress:	dq		0
