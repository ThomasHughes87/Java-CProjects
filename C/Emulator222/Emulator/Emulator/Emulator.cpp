

#include "stdafx.h"
#include <winsock2.h>

#pragma comment(lib, "wsock32.lib")


#define STUDENT_NUMBER    "12304098"

#define IP_ADDRESS_SERVER "127.0.0.1"

#define PORT_SERVER 0x1984 // We define a port that we are going to use.
#define PORT_CLIENT 0x1985 // We define a port that we are going to use.

#define WORD  unsigned short
#define DWORD unsigned long
#define BYTE  unsigned char

#define MAX_FILENAME_SIZE 500
#define MAX_BUFFER_SIZE   500

SOCKADDR_IN server_addr;
SOCKADDR_IN client_addr;

SOCKET sock;  // This is our socket, it is the handle to the IO address to read/write packets

WSADATA data;

char InputBuffer[MAX_BUFFER_SIZE];

char hex_file[MAX_BUFFER_SIZE];
char trc_file[MAX_BUFFER_SIZE];

//////////////////////////
//   Registers          //
//////////////////////////

#define FLAG_I  0x40
#define FLAG_Z  0x20
#define FLAG_P  0x10
#define FLAG_V  0x08
#define FLAG_N  0x02
#define FLAG_C  0x01
#define REGISTER_A	3
#define REGISTER_B	2
#define REGISTER_D	1
#define REGISTER_C	0
//#define REGISTER_P  4
WORD BaseRegister;
BYTE PageRegister;

BYTE Registers[4];
BYTE Flags;
WORD ProgramCounter;
WORD StackPointer;


////////////
// Memory //
////////////

#define MEMORY_SIZE	65536

BYTE Memory[MEMORY_SIZE];

#define TEST_ADDRESS_1  0x01FA
#define TEST_ADDRESS_2  0x01FB
#define TEST_ADDRESS_3  0x01FC
#define TEST_ADDRESS_4  0x01FD
#define TEST_ADDRESS_5  0x01FE
#define TEST_ADDRESS_6  0x01FF
#define TEST_ADDRESS_7  0x0200
#define TEST_ADDRESS_8  0x0201
#define TEST_ADDRESS_9  0x0202
#define TEST_ADDRESS_10  0x0203
#define TEST_ADDRESS_11  0x0204
#define TEST_ADDRESS_12  0x0205


///////////////////////
// Control variables //
///////////////////////

bool memory_in_range = true;
bool halt = false;


///////////////////////
// Disassembly table //
///////////////////////

char opcode_mneumonics[][14] =
{
	"NOP impl     ",
	"LODS  #      ",
	"LODS abs     ",
	"LODS zpg     ",
	"LODS (ind)   ",
	"LODS pag     ",
	"LODS bas     ",
	"ILLEGAL     ",
	"DEP impl     ",
	"ILLEGAL     ",
	"LDAA  #      ",
	"LDAA abs     ",
	"LDAA zpg     ",
	"LDAA (ind)   ",
	"LDAA pag     ",
	"LDAA bas     ",

	"HALT impl    ",
	"LDZ  #       ",
	"LDZ abs      ",
	"LDZ zpg      ",
	"LDZ (ind)    ",
	"LDZ pag      ",
	"LDZ bas      ",
	"ILLEGAL     ",
	"INP impl     ",
	"ILLEGAL     ",
	"LDAB  #      ",
	"LDAB abs     ",
	"LDAB zpg     ",
	"LDAB (ind)   ",
	"LDAB pag     ",
	"LDAB bas     ",

	"ILLEGAL     ",
	"CAS impl     ",
	"CLC impl     ",
	"STS abs      ",
	"STS zpg      ",
	"STS (ind)    ",
	"STS pag      ",
	"STS bas      ",
	"DEZ impl     ",
	"JPA abs      ",
	"JPA zpg      ",
	"JPA (ind)    ",
	"JPA pag      ",
	"INC abs      ",
	"INCA A,A     ",
	"INCB B,B     ",

	"ADIA  #      ",
	"TSA impl     ",
	"SEC impl     ",
	"STZ abs      ",
	"STZ zpg      ",
	"STZ (ind)    ",
	"STZ pag      ",
	"STZ bas      ",
	"INZ impl     ",
	"JCC abs      ",
	"JCC zpg      ",
	"JCC (ind)    ",
	"JCC pag      ",
	"DEC abs      ",
	"DECA A,A     ",
	"DECB B,B     ",

	"ADIB  #      ",
	"ABA impl     ",
	"CLI impl     ",
	"ILLEGAL     ",
	"ILLEGAL     ",
	"JPR abs      ",
	"JPR zpg      ",
	"JPR (ind)    ",
	"JPR pag      ",
	"JCS abs      ",
	"JCS zpg      ",
	"JCS (ind)    ",
	"JCS pag      ",
	"RRC abs      ",
	"RRCA A,A     ",
	"RRCB B,B     ",

	"SBIA  #      ",
	"SBA impl     ",
	"STI impl     ",
	"ILLEGAL     ",
	"ILLEGAL     ",
	"ADC A,C      ",
	"ADC A,D      ",
	"ADC B,C      ",
	"ADC B,D      ",
	"JNE abs      ",
	"JNE zpg      ",
	"JNE (ind)    ",
	"JNE pag      ",
	"RL abs       ",
	"RLA A,A      ",
	"RLB B,B      ",

	"SBIB  #      ",
	"AAB impl     ",
	"STV impl     ",
	"ILLEGAL     ",
	"RET impl     ",
	"SBC A,C      ",
	"SBC A,D      ",
	"SBC B,C      ",
	"SBC B,D      ",
	"JEQ abs      ",
	"JEQ zpg      ",
	"JEQ (ind)    ",
	"JEQ pag      ",
	"SHL abs      ",
	"SHLA A,A     ",
	"SHLB B,B     ",

	"CPIA  #      ",
	"SAB impl     ",
	"CLV impl     ",
	"SWI impl     ",
	"ILLEGAL     ",
	"ADD A,C      ",
	"ADD A,D      ",
	"ADD B,C      ",
	"ADD B,D      ",
	"JVC abs      ",
	"JVC zpg      ",
	"JVC (ind)    ",
	"JVC pag      ",
	"ASR abs      ",
	"ASRA A,A     ",
	"ASRB B,B     ",

	"CPIB  #      ",
	"TAP impl     ",
	"CMC impl     ",
	"RTI impl     ",
	"ILLEGAL     ",
	"SUB A,C      ",
	"SUB A,D      ",
	"SUB B,C      ",
	"SUB B,D      ",
	"JVS abs      ",
	"JVS zpg      ",
	"JVS (ind)    ",
	"JVS pag      ",
	"SHR abs      ",
	"SHRA A,A     ",
	"SHRB B,B     ",

	"ORIA  #      ",
	"TPA impl     ",
	"CMV impl     ",
	"ILLEGAL     ",
	"ILLEGAL     ",
	"CMP A,C      ",
	"CMP A,D      ",
	"CMP B,C      ",
	"CMP B,D      ",
	"JMI abs      ",
	"JMI zpg      ",
	"JMI (ind)    ",
	"JMI pag      ",
	"NOT abs      ",
	"NOTA A,A     ",
	"NOTB B,B     ",

	"ORIB  #      ",
	"MV A,A       ",
	"MV A,B       ",
	"MV A,C       ",
	"MV A,D       ",
	"ORA A,C      ",
	"ORA A,D      ",
	"ORA B,C      ",
	"ORA B,D      ",
	"JPL abs      ",
	"JPL zpg      ",
	"JPL (ind)    ",
	"JPL pag      ",
	"NEG abs      ",
	"NEGA A,0     ",
	"NEGB B,0     ",

	"ANIA  #      ",
	"MV B,A       ",
	"MV B,B       ",
	"MV B,C       ",
	"MV B,D       ",
	"AND A,C      ",
	"AND A,D      ",
	"AND B,C      ",
	"AND B,D      ",
	"JPE abs      ",
	"JPE zpg      ",
	"JPE (ind)    ",
	"JPE pag      ",
	"ROL abs      ",
	"ROLA A,A     ",
	"ROLB B,B     ",

	"ANIB  #      ",
	"MV C,A       ",
	"MV C,B       ",
	"MV C,C       ",
	"MV C,D       ",
	"EOR A,C      ",
	"EOR A,D      ",
	"EOR B,C      ",
	"EOR B,D      ",
	"JPO abs      ",
	"JPO zpg      ",
	"JPO (ind)    ",
	"JPO pag      ",
	"RR abs       ",
	"RRA A,A      ",
	"RRB B,B      ",

	"ILLEGAL     ",
	"MV D,A       ",
	"MV D,B       ",
	"MV D,C       ",
	"MV D,D       ",
	"BT A,C       ",
	"BT A,D       ",
	"BT B,C       ",
	"BT B,D       ",
	"ILLEGAL     ",
	"ILLEGAL     ",
	"ILLEGAL     ",
	"ILLEGAL     ",
	"CLR abs      ",
	"CLRA A,0     ",
	"CLRB B,0     ",

	"LD  #,C      ",
	"LD abs,C     ",
	"LD zpg,C     ",
	"LD (ind),C   ",
	"LD pag,C     ",
	"LD bas,C     ",
	"STA abs      ",
	"STA zpg      ",
	"STA (ind)    ",
	"STA pag      ",
	"STA bas      ",
	"PUSH  ,A     ",
	"PUSH  ,B     ",
	"PUSH  ,s     ",
	"PUSH  ,C     ",
	"PUSH  ,D     ",

	"LD  #,D      ",
	"LD abs,D     ",
	"LD zpg,D     ",
	"LD (ind),D   ",
	"LD pag,D     ",
	"LD bas,D     ",
	"STB abs      ",
	"STB zpg      ",
	"STB (ind)    ",
	"STB pag      ",
	"STB bas      ",
	"POP A,       ",
	"POP B,       ",
	"POP s,       ",
	"POP C,       ",
	"POP D,       ",

};

////////////////////////////////////////////////////////////////////////////////
//                           Simulator/Emulator (Start)                       //
////////////////////////////////////////////////////////////////////////////////
BYTE fetch()
{
	BYTE byte = 0;

	if ((ProgramCounter >= 0) && (ProgramCounter <= MEMORY_SIZE))
	{
		memory_in_range = true;
		byte = Memory[ProgramCounter];
		ProgramCounter++;
	}
	else
	{
		memory_in_range = false;
	}
	return byte;
}
void set_flag_z(BYTE inReg)
{
	BYTE data;
	data = inReg;

	if (data == 0x00) // msbit set 
	{
		Flags = Flags | FLAG_Z;
	}
	else
	{
		Flags = Flags & (~FLAG_Z);
	}
}

void set_flag_n(BYTE inReg)
{
	BYTE data;
	data = inReg;
	if ((data & 0x80) == 0x80) //msbit set 
	{
		Flags = Flags | FLAG_N;

	}
	else
	{

		Flags = Flags & (~FLAG_N);
	}
}


void set_flag_v(BYTE in1, BYTE in2, WORD out1)
{
	BYTE reg1in;
	BYTE reg2in;
	BYTE regOut;
	reg1in = in1;
	reg2in = in2;
	regOut = out1;
	if ((((reg1in & 0x80) == 0x80)
		&& ((reg2in & 0x80) == 0x80)
		&& (((BYTE)regOut & 0x80) != 0x80))
		|| (((reg1in & 0x80) != 0x80)
		&& ((reg2in & 0x80) != 0x80)
		&& (((BYTE)regOut & 0x80) == 0x80)))
	{
		Flags = Flags | FLAG_V;
	}
	else
	{
		Flags = Flags & (~FLAG_V);
	}
}
void set_flag_v_wrong(BYTE inReg1, BYTE inReg2, WORD result)
{
	BYTE res = (BYTE)result;
	if (((inReg1 >= 0x80) && (inReg2 >= 0x80) && (res >= 0x80)) ||
		((inReg1 < 0x80) && (inReg2 < 0x80) && (res < 0x80)))
		Flags = Flags | FLAG_V; //set
	else
		Flags = Flags & (~FLAG_V);

}

void set_flag_p(BYTE inReg)
{
	BYTE reg;
	reg = inReg;
	if ((reg & 0x01) != 0) //msbit set 
	{
		Flags = Flags | FLAG_P;

	}
	else
	{


		Flags = Flags & (0xFF - FLAG_P);
	}
}
void set_flag_z16(WORD reg)
{
	if (reg == 0)
		Flags = Flags | FLAG_Z;
	else
		Flags = Flags & (~FLAG_Z);

}
void set_flag_n16(WORD reg)
{
	if ((reg & 0x8000) == 0x8000)
		Flags = Flags | FLAG_N;
	else
		Flags = Flags & (~FLAG_N);

}
void set_flag_c(WORD reg)
{
	WORD result = reg;

	if (result >= 0x100)
	{
		Flags = Flags | FLAG_C;//Set carry flag
	}
	else
	{
		Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
	}
}




void Group_1(BYTE opcode){
	BYTE LB = 0;
	BYTE HB = 0;
	WORD address = 0;
	WORD data = 0;
	WORD data_16;
	WORD temp_word;
	switch (opcode) {
	case 0x0A: //LDAA Immidiate
		data = fetch();
		Registers[REGISTER_A] = data;

		break;
	case 0x0B://LDAA (abs) - Hex: 0x0B - load a absolute//
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE){
			Registers[REGISTER_A] = Memory[address];
		}

		break;
	case 0x0C://LDAA (zpg) - Hex: 0x0C - load a zero page//
		address += 0x0000 | (WORD)fetch();
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_A] = Memory[address];
		}
		break;
	case 0x0D://LDAA ((ind)) - Hex: 0x0D - load to indirect access memory //
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_A] = Memory[address];
		}
		break;
	case 0x0E://LDAA (pag) - Hex: 0x0E - load using page addressing mode;
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_A] = Memory[address];
		}
		break;
	case 0x0F://LDAA (bas) - Hex: 0x0F - load from base register
		if ((LB = fetch()) >= 0X80) {
			LB = 0x00 - LB;
			address += (BaseRegister - LB);
		}
		else address += (BaseRegister + LB);
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_A] = Memory[address];

		}
		break;
	case 0xE6://STA (abs) - Hex: 0xE6 - storing and absolute
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE){
			Memory[address] = Registers[REGISTER_A];
		}
		break;
	case 0xE7://STA (zpg) - Hex: 0xE7 - storing a zero page
		address += 0x0000 | (WORD)fetch();
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = Registers[REGISTER_A];
		}
		break;
	case 0xE8://STA (ind) - Hex: 0xE8 - sores an indirect
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = Registers[REGISTER_A];
		}

		break;
	case 0xE9://STA (pag) - stores paging mode;
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = Registers[REGISTER_A];
		}
		break;
	case 0xEA://STA (bas) - Hex: 0xEA - Stores  base register
		if ((LB = fetch()) >= 0X80) {
			LB = 0x00 - LB;
			address += (BaseRegister - LB);
		}
		else address += (BaseRegister + LB);
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = Registers[REGISTER_A];

		}
		break;
	case 0xE0: //LD(#) HEX: 0xE0 loding a c register 
		data = fetch();
		Registers[REGISTER_C] = data;
		break;
	case 0xE1://LD (abs) - Hex: 0xE1 - load c absolute//
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_C] = Memory[address];
		}

		break;
	case 0xE2://LD (zpg) - Hex: 0xE2 - loading a zero page
		address += 0x0000 | (WORD)fetch();
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_C] = Memory[address];
		}
		break;
	case 0xE3://LD ((ind)) - Hex: 0xE3 - load indirect access memory //
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_C] = Memory[address];
		}
		break;
	case 0xE4://LD (pag) - Hex: 0xE4 - load page;
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_C] = Memory[address];
		}
		break;
	case 0xE5://LD (bas) - Hex: 0xE5 - load from base register;
		if ((LB = fetch()) >= 0X80) {
			LB = 0x00 - LB;
			address += (BaseRegister - LB);
		}
		else address += (BaseRegister + LB);
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_C] = Memory[address];

		}
		break;
	case 0xF0: //LD(#)  
		data = fetch();
		Registers[REGISTER_D] = data;
		break;
	case 0xF1://LD (abs) 
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_D] = Memory[address];
		}

		break;
	case 0xF2://LD (zpg)
		address += 0x0000 | (WORD)fetch();
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_D] = Memory[address];
		}
		break;
	case 0xF3://LD ((ind))
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_D] = Memory[address];
		}
		break;
	case 0xF4://LD (pag) 
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_D] = Memory[address];
		}
		break;
	case 0xF5://LD (bas) 
		if ((LB = fetch()) >= 0X80) {
			LB = 0x00 - LB;
			address += (BaseRegister - LB);
		}
		else address += (BaseRegister + LB);
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_D] = Memory[address];

		}
		break;
	case 0x01: //LODS \#
		data = fetch();
		data_16 = data;
		data_16 <<= 8;
		data_16 += fetch();
		StackPointer = data_16;
		break;
	case 0x02: //LODS abs
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		data_16 = (WORD)Memory[address] << 8;
		data_16 += (WORD)Memory[address + 1];
		StackPointer = data_16;
		break;
	case 0x03: //LODS zpg
		address += 0x0000 | (WORD)fetch();
		data_16 = (WORD)Memory[address] << 8;
		data_16 += (WORD)Memory[address + 1];
		StackPointer = data_16;
		break;
	case 0x04: //LODS (ind)
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		data_16 = (WORD)Memory[address] << 8;
		data_16 += (WORD)Memory[address + 1];
		StackPointer = data_16;
		break;
	case 0x05: //LODS pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		data_16 = (WORD)Memory[address] << 8;
		data_16 += (WORD)Memory[address + 1];
		StackPointer = data_16;
		break;
	case 0x06: //LODS bas
		if ((LB = fetch()) >= 0x80){
			LB = 0x00 - LB;
			address += (BaseRegister - LB);
		}
		else {
			address += (BaseRegister + LB);
		}
		data_16 = (WORD)Memory[address] << 8;
		data_16 += (WORD)Memory[address + 1];
		StackPointer = data_16;
		break;
	case 0x1A: //LDAB Immidiate
		data = fetch();
		Registers[REGISTER_B] = data;

		break;
	case 0x1B://LDAB (abs) - Hex: 0x1B - load a absolute//
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE){
			Registers[REGISTER_B] = Memory[address];
		}

		break;
	case 0x1C://LDAB (zpg) - Hex: 0x1C - load a zero page//
		address += 0x0000 | (WORD)fetch();
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_B] = Memory[address];
		}
		break;
	case 0x1D://LDAB ((ind)) - Hex: 0x1D - load to indirect access memory //
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_B] = Memory[address];
		}
		break;
	case 0x1E://LDAB (pag) - Hex: 0x1E - load using page addressing mode;
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_B] = Memory[address];
		}
		break;
	case 0x1F://LDAB (bas) - Hex: 0x1F - load from base register
		if ((LB = fetch()) >= 0X80) {
			LB = 0x00 - LB;
			address += (BaseRegister - LB);
		}
		else address += (BaseRegister + LB);
		if (address >= 0 && address < MEMORY_SIZE) {
			Registers[REGISTER_B] = Memory[address];

		}
		break;
	case 0xF6://STB (abs) - Hex: 0xF6 - storing and absolute
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE){
			Memory[address] = Registers[REGISTER_B];
		}
		break;
	case 0xF7://STB (zpg) - Hex: 0xF7 - storing a zero page
		address += 0x0000 | (WORD)fetch();
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = Registers[REGISTER_B];
		}
		break;
	case 0xF8://STB (ind) - Hex: 0xF8 - sores an indirect
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = Registers[REGISTER_B];
		}

		break;
	case 0xF9://STB (pag) - stores paging mode;
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = Registers[REGISTER_B];
		}
		break;
	case 0xFA://STB (bas) - Hex: 0xFA - Stores  base register
		if ((LB = fetch()) >= 0X80) {
			LB = 0x00 - LB;
			address += (BaseRegister - LB);
		}
		else address += (BaseRegister + LB);
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = Registers[REGISTER_B];

		}
		break;
	case 0x23://STS (abs) -  storing 
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE){
			Memory[address] = StackPointer >> 8;
			Memory[address + 1] = StackPointer & 0xff;
		}
		break;
	case 0x24://STS (zpg) -  storing a zero page

		address += 0x0000 | (WORD)fetch();
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = StackPointer >> 8;
			Memory[address + 1] = StackPointer & 0xFF;
		}
		break;
	case 0x25://STS (ind) - sores an indirect
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = StackPointer >> 8;
			Memory[address + 1] = StackPointer & 0xFF;
		}

		break;
	case 0x26://STS (pag) - stores paging mode;
		LB = PageRegister;
		HB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = StackPointer >> 8;
			Memory[address + 1] = StackPointer & 0xFF;
		}
		break;
	case 0x27://STS (bas) -  Stores  base register
		if ((LB = fetch()) >= 0X80) {
			LB = 0x00 - LB;
			address += (BaseRegister - LB);
		}
		else address += (BaseRegister + LB);
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = StackPointer >> 8;
			Memory[address + 1] = StackPointer & 0xFF;


		}
		break;


	case 0x55: //ADC Hex0x55 A-C
		temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_C];
		if ((Flags & FLAG_C) != 0)
		{
			temp_word++;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_A], Registers[REGISTER_C], (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0x56: //ADC Hex0x56 A-D
		temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_D];
		if ((Flags & FLAG_C) != 0)
		{
			temp_word++;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_A], Registers[REGISTER_D], (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0x57: //ADC Hex0x57 B-C
		temp_word = (WORD)Registers[REGISTER_B] + (WORD)Registers[REGISTER_C];
		if ((Flags & FLAG_C) != 0)
		{
			temp_word++;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_B], Registers[REGISTER_C], (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;
	case 0x58: //ADC Hex0x58 B-D
		temp_word = (WORD)Registers[REGISTER_B] + (WORD)Registers[REGISTER_D];
		if ((Flags & FLAG_C) != 0)
		{
			temp_word++;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_B], Registers[REGISTER_D], (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;
	case 0x75: //ADD  A-C
		temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_C];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_A], Registers[REGISTER_C], (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;


	case 0x76: //ADD  A-D
		temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_D];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_A], Registers[REGISTER_D], (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;


		break;
	case 0x77: //ADD  B-C
		temp_word = (WORD)Registers[REGISTER_B] + (WORD)Registers[REGISTER_C];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_B], Registers[REGISTER_C], (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;


		break;
	case 0x78: //ADD  B-D
		temp_word = (WORD)Registers[REGISTER_B] + (WORD)Registers[REGISTER_D];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_B], Registers[REGISTER_D], (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;


		break;
	case 0x85: //SUB  A-C
		temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_C];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_A], Registers[REGISTER_C], (BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_A], ~Registers[REGISTER_C] + 1, (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;


	case 0x86: //SUB  A-D
		temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_D];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_A], Registers[REGISTER_D], (BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_A], ~Registers[REGISTER_D] + 1, (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;


		break;
	case 0x87: //SUB  B-C
		temp_word = (WORD)Registers[REGISTER_B] - (WORD)Registers[REGISTER_C];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_B], Registers[REGISTER_C], (BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_B], ~Registers[REGISTER_C] + 1, (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;


		break;
	case 0x88: //SUB  B-D
		temp_word = (WORD)Registers[REGISTER_B] - (WORD)Registers[REGISTER_D];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_B], Registers[REGISTER_D], (BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_B], ~Registers[REGISTER_D] + 1, (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;


		break;
	case 0x65: //SBC  A-C
		temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_C];
		if ((Flags & FLAG_C) != 0)
		{
			temp_word--;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_A], Registers[REGISTER_C], (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_A], ~Registers[REGISTER_C] + 1, (BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0x66: //SBC  A-D
		temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_D];
		if ((Flags & FLAG_C) != 0)
		{
			temp_word--;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_A], Registers[REGISTER_D], (BYTE)temp_word);

		set_flag_p((BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_A], ~Registers[REGISTER_D] + 1, (BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0x67: //SBC  B-C
		temp_word = (WORD)Registers[REGISTER_B] - (WORD)Registers[REGISTER_C];
		if ((Flags & FLAG_C) != 0)
		{
			temp_word--;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_B], Registers[REGISTER_C], (BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_B], ~Registers[REGISTER_C] + 1, (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;
	case 0x68: //SBC  B-D
		temp_word = (WORD)Registers[REGISTER_B] - (WORD)Registers[REGISTER_D];
		if ((Flags & FLAG_C) != 0)
		{
			temp_word--;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_B], Registers[REGISTER_D], (BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_B], ~Registers[REGISTER_D] + 1, (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;



	case 0x95:// CMP 
		temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_C];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_A], Registers[REGISTER_C], (BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_A], ~Registers[REGISTER_C] + 1, (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);

		break;

	case 0x96://CMP
		temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_D];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_A], Registers[REGISTER_D], (BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_A], ~Registers[REGISTER_D] + 1, (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);

		break;

	case 0x97://CMP
		temp_word = (WORD)Registers[REGISTER_B] - (WORD)Registers[REGISTER_C];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_B], Registers[REGISTER_C], (BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_B], ~Registers[REGISTER_C] + 1, (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);

		break;

	case 0x98://CMP
		temp_word = (WORD)Registers[REGISTER_B] - (WORD)Registers[REGISTER_D];

		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_v(Registers[REGISTER_B], Registers[REGISTER_D], (BYTE)temp_word);
		set_flag_v_wrong(Registers[REGISTER_B], ~Registers[REGISTER_D] + 1, (BYTE)temp_word);
		set_flag_p((BYTE)temp_word);

		break;


	case 0x31: //TSA transfer the status
		Registers[REGISTER_A] = Flags;
		break;
	case 0x21: //CAS transfer acumulator to status
		Flags = Registers[REGISTER_A];
		break;
	case 0x41: // ABA add acum b into a
		temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_B];
		if ((Flags & FLAG_C) != 0)
		{
			temp_word++;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0x51: //SBA subs b into a
		temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_B];

		if ((Flags & FLAG_C) != 0)
		{
			temp_word--;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0x61://AAB adds accumulator A into Accumulator B
		temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_B];

		if ((Flags & FLAG_C) != 0)
		{
			temp_word++;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;
	case 0x71://sab a into b
		temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_B];

		if ((Flags & FLAG_C) != 0)
		{
			temp_word++;
		}
		if (temp_word >= 0x100)
		{
			Flags = Flags | FLAG_C;//Set carry flag
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);//Clear carry flag
		}
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;


	case 0x22://CLC
		Flags = Flags & (0xFF - FLAG_C);//Clear carry flag;
		break;
	case 0x32://SEC
		Flags = Flags | FLAG_C;//Set carry flag;
		break;
	case 0x62://STV
		Flags = Flags | FLAG_V;//overflow flag;
		break;
	case 0x72://CLV
		Flags = Flags & (0xFF - FLAG_V);//Clear overflow flag
		break;
	case 0x42://CLI
		Flags = Flags & (0xFF - FLAG_I);//Clear interupt flag
		break;
	case 0x52://STI
		Flags = Flags | FLAG_I;//interupt flag;
		break;
	case 0x82://CMC
		//Set carry flag
		if (Flags&FLAG_C == 0x00)
			Flags = Flags | FLAG_C;
		else
			Flags = Flags & 0xFE; //~FLAG_C
		break;

	case 0x92://CMV
		if (Flags&FLAG_V == 0x00)
			Flags = Flags | FLAG_V;
		else
			Flags = Flags & 0xFE; //~FLAG_V
		break;





	case 0x11: //LDZ \#
		data = fetch();
		data_16 = data;
		data_16 <<= 8;
		data_16 += fetch();
		BaseRegister = data_16;
		set_flag_z16(BaseRegister);
		set_flag_n16(BaseRegister);
		set_flag_p((BYTE)BaseRegister);

		break;

	case 0x12: //LDZ abs
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		data_16 = (WORD)Memory[address] << 8;
		data_16 += (WORD)Memory[address + 1];
		BaseRegister = data_16;
		set_flag_z16(BaseRegister);
		set_flag_n16(BaseRegister);
		set_flag_p((BYTE)BaseRegister);


		break;
	case 0x13: //LDZ zpg
		address += 0x0000 | (WORD)fetch();
		data_16 = (WORD)Memory[address] << 8;
		data_16 += (WORD)Memory[address + 1];
		BaseRegister = data_16;
		set_flag_z16(BaseRegister);
		set_flag_n16(BaseRegister);
		set_flag_p((BYTE)BaseRegister);
		break;
	case 0x14: //LDZ (ind)
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		data_16 = (WORD)Memory[address] << 8;
		data_16 += (WORD)Memory[address + 1];
		BaseRegister = data_16;
		set_flag_z16(BaseRegister);
		set_flag_n16(BaseRegister);
		set_flag_p((BYTE)BaseRegister);
		break;
	case 0x15: //LDZ pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		data_16 = (WORD)Memory[address] << 8;
		data_16 += (WORD)Memory[address + 1];
		BaseRegister = data_16;
		set_flag_z16(BaseRegister);
		set_flag_n16(BaseRegister);
		set_flag_p((BYTE)BaseRegister);
		break;
	case 0x16: //LDZ bas
		if ((LB = fetch()) >= 0x80){
			LB = 0x00 - LB;
			address += (BaseRegister - LB);
		}
		else {
			address += (BaseRegister + LB);
		}
		data_16 = (WORD)Memory[address] << 8;
		data_16 += (WORD)Memory[address + 1];
		BaseRegister = data_16;
		set_flag_z16(BaseRegister);
		set_flag_n16(BaseRegister);
		set_flag_p((BYTE)BaseRegister);
		break;
	case 0x33://STZ (abs) -  storing 
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE){
			Memory[address] = BaseRegister >> 8;
			Memory[address + 1] = BaseRegister & 0xff;
		}
		break;
	case 0x34://STZ (zpg) -  storing a zero page

		address += 0x0000 | (WORD)fetch();
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = BaseRegister >> 8;
			Memory[address + 1] = BaseRegister & 0xff;
		}
		break;
	case 0x35://STZ (ind) - sores an indirect
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = BaseRegister >> 8;
			Memory[address + 1] = BaseRegister & 0xff;
		}

		break;
	case 0x36://STZ (pag) - stores paging mode;
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = BaseRegister >> 8;
			Memory[address + 1] = BaseRegister & 0xff;
		}
		break;
	case 0x37://STZ (bas) -  Stores  base register

		if (address >= 0 && address < MEMORY_SIZE) {
			Memory[address] = BaseRegister >> 8;
			Memory[address + 1] = BaseRegister & 0xff;


		}
		break;
	case 0xA5://ORA A-C
		temp_word = (WORD)Registers[REGISTER_A] | (WORD)Registers[REGISTER_C];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0xA6://ORA A-D
		temp_word = (WORD)Registers[REGISTER_A] | (WORD)Registers[REGISTER_D];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0xA7://ORA B-C
		temp_word = (WORD)Registers[REGISTER_B] | (WORD)Registers[REGISTER_C];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;
	case 0xA8://ORA B-D
		temp_word = (WORD)Registers[REGISTER_B] | (WORD)Registers[REGISTER_D];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;



	case 0xC5://EOR A-C
		temp_word = (WORD)Registers[REGISTER_A] ^ (WORD)Registers[REGISTER_C];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0xC6://EOR A-D
		temp_word = (WORD)Registers[REGISTER_A] ^ (WORD)Registers[REGISTER_D];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0xC7://EOR B-C
		temp_word = (WORD)Registers[REGISTER_B] ^ (WORD)Registers[REGISTER_C];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;
	case 0xC8://EOR B-D
		temp_word = (WORD)Registers[REGISTER_B] ^ (WORD)Registers[REGISTER_D];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;
	case 0xB5://AND A-C
		temp_word = (WORD)Registers[REGISTER_A] & (WORD)Registers[REGISTER_C];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Flags = Flags & (0xFF - FLAG_V);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0xB6://AND A-D
		temp_word = (WORD)Registers[REGISTER_A] & (WORD)Registers[REGISTER_D];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Flags = Flags & (0xFF - FLAG_V);
		Registers[REGISTER_A] = (BYTE)temp_word;
		break;
	case 0xB7://AND B-C
		temp_word = (WORD)Registers[REGISTER_B] & (WORD)Registers[REGISTER_C];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Flags = Flags & (0xFF - FLAG_V);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;
	case 0xB8://AND B-D
		temp_word = (WORD)Registers[REGISTER_B] & (WORD)Registers[REGISTER_D];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		Flags = Flags & (0xFF - FLAG_V);
		Registers[REGISTER_B] = (BYTE)temp_word;
		break;


	case 0xEB:// push register to stack
		if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)){

			--StackPointer;
			Memory[StackPointer] = Registers[REGISTER_A];

		}
		break;
	case 0xEC:// push register to stack
		if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)){

			--StackPointer;
			Memory[StackPointer] = Registers[REGISTER_B];

		}
		break;
	case 0xED:// push register to stack
		if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)){

			--StackPointer;
			Memory[StackPointer] = Flags;

		}
		break;
	case 0xEE:// push register to stack
		if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)){

			--StackPointer;
			Memory[StackPointer] = Registers[REGISTER_C];

		}
		break;
	case 0xEF:// push register to stack
		if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)){

			--StackPointer;
			Memory[StackPointer] = Registers[REGISTER_D];

		}
		break;
	case 0xFB://POP register A
		if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)){
			Registers[REGISTER_A] = Memory[StackPointer];
			++StackPointer;
		}
		break;
	case 0xFC://POP register B
		if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)){
			Registers[REGISTER_B] = Memory[StackPointer];
			++StackPointer;
		}
		break;
	case 0xFD://POP
		if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)){
			Flags = Memory[StackPointer];
			++StackPointer;
		}
		break;
	case 0xFE://POP register C
		if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)){
			Registers[REGISTER_C] = Memory[StackPointer];
			++StackPointer;
		}
		break;

	case 0xFF://POP register D
		if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)){
			Registers[REGISTER_D] = Memory[StackPointer];
			++StackPointer;
		}
		break;
	case 0x29://abs JPA jump
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		ProgramCounter = address;

		break;
	case 0x2A://zpg JPA jump
		address += 0x0000 | (WORD)fetch();
		ProgramCounter = address;
		break;
	case 0x2B:// ind JPA jump
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		ProgramCounter = address;
		break;
	case 0x2C:// pag JPA jump
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		ProgramCounter = address;
		break;



	case 0x45://  JPR(jump to subroutine abs
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if ((StackPointer >= 2) && (StackPointer < MEMORY_SIZE)){
			--StackPointer;
			Memory[StackPointer] = (BYTE)(ProgramCounter & 0xFF);
			--StackPointer;
			Memory[StackPointer] = (BYTE)((ProgramCounter >> 8) & 0xFF);

		}
		ProgramCounter = address;
		break;
	case 0x46://  JPR zpg
		address += 0x0000 | (WORD)fetch();
		if ((StackPointer >= 2) && (StackPointer < MEMORY_SIZE)){
			--StackPointer;
			Memory[StackPointer] = (BYTE)(ProgramCounter & 0xFF);
			--StackPointer;
			Memory[StackPointer] = (BYTE)((ProgramCounter >> 8) & 0xFF);

		}
		ProgramCounter = address;
		break;

		break;
	case 0x47://  JPR ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if ((StackPointer >= 2) && (StackPointer < MEMORY_SIZE)){
			--StackPointer;
			Memory[StackPointer] = (BYTE)(ProgramCounter & 0xFF);
			--StackPointer;
			Memory[StackPointer] = (BYTE)((ProgramCounter >> 8) & 0xFF);

		}
		ProgramCounter = address;
		break;

		break;
	case 0x48://  JPR pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if ((StackPointer >= 2) && (StackPointer < MEMORY_SIZE)){
			--StackPointer;
			Memory[StackPointer] = (BYTE)(ProgramCounter & 0xFF);
			--StackPointer;
			Memory[StackPointer] = (BYTE)((ProgramCounter >> 8) & 0xFF);


		}

		ProgramCounter = address;


		break;
	case 0x59: // JNE: abs Jump on result not zero.
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if ((Flags&FLAG_Z) != FLAG_Z)
		{
			ProgramCounter = address;
		}
		break;

	case 0x5A://  JNE zpg
		address += 0x0000 | (WORD)fetch();
		if ((Flags&FLAG_Z) != FLAG_Z)
		{
			ProgramCounter = address;
		}

		break;
	case 0x5B://  JNE ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_Z) != FLAG_Z)
		{
			ProgramCounter = address;
		}

		break;
	case 0x5C://  JNE pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_Z) != FLAG_Z)
		{
			ProgramCounter = address;
		}
		break;
	case 0x69: // JEQ: abs Jump on result not zero.
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if ((Flags&FLAG_Z) == FLAG_Z)
		{
			ProgramCounter = address;
		}
		break;

	case 0x6A://  JEQ zpg
		address += 0x0000 | (WORD)fetch();
		if ((Flags&FLAG_Z) == FLAG_Z)
		{
			ProgramCounter = address;
		}

		break;
	case 0x6B://  JEQ ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_Z) == FLAG_Z)
		{
			ProgramCounter = address;
		}

		break;
	case 0x6C://  JEQ pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_Z) == FLAG_Z)
		{
			ProgramCounter = address;
		}
		break;
	case 0x39: // JCC: abs 
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_C) == 0);
			{
				ProgramCounter = address;
			}
		}

		break;

	case 0x3A://  JCC zpg
		address += 0x0000 | (WORD)fetch();
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_C) == 0);
			{
				ProgramCounter = address;
			}
		}
		break;
	case 0x3B://  JCC ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE){
			if ((Flags&FLAG_C) == 0);
			{
				ProgramCounter = address;
			}
		}

		break;
	case 0x3C://  JCC pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_C) == 0);
			{
				ProgramCounter = address;
			}
		}
		break;
	case 0x79: // JVC: abs 
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_V) == 0);
			{
				ProgramCounter = address;
			}
		}

		break;

	case 0x7A://  JVC zpg
		address += 0x0000 | (WORD)fetch();
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_V) == 0);
			{
				ProgramCounter = address;
			}
		}
		break;
	case 0x7B://  JVC ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE){
			if ((Flags&FLAG_V) == 0);
			{
				ProgramCounter = address;
			}
		}

		break;
	case 0x7C://  JVC pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_V) == 0);
			{
				ProgramCounter = address;
			}
		}
		break;
	case 0x49: // JCS: abs 
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE){
			if ((Flags&FLAG_C) == 0);
			{
				ProgramCounter = address;
			}
		}

		break;

	case 0x4A://  JCS zpg
		address += 0x0000 | (WORD)fetch();
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_C) == 1);
			{
				ProgramCounter = address;
			}
		}
		break;
	case 0x4B://  JCS ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_C) == 1);
			{
				ProgramCounter = address;
			}
		}

		break;
	case 0x4C://  JCS pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_C) == 1);
			{
				ProgramCounter = address;
			}
		}
		break;

	case 0x89: // JVS: abs 
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE){
			if ((Flags&FLAG_V) == 0);
			{
				ProgramCounter = address;
			}
		}

		break;

	case 0x8A://  JVS zpg
		address += 0x0000 | (WORD)fetch();
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_V) == 1);
			{
				ProgramCounter = address;
			}
		}
		break;
	case 0x8B://  JVS ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_V) == 1);
			{
				ProgramCounter = address;
			}
		}

		break;
	case 0x8C://  JVS pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (Memory[address] > 0 && address < MEMORY_SIZE)
		{
			if ((Flags&FLAG_V) == 1);
			{
				ProgramCounter = address;
			}
		}
		break;
	case 0x99: // JMI: abs Jump on negative result.
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if ((Flags&FLAG_N) == 1)
		{
			ProgramCounter = address;
		}
		break;

	case 0x9A://  JMI zpg
		address += 0x0000 | (WORD)fetch();
		if ((Flags&FLAG_N) == 1)
		{
			ProgramCounter = address;
		}

		break;
	case 0x9B://  JMI ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_N) == 1)
		{
			ProgramCounter = address;
		}

		break;
	case 0x9C://  JMI pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_N) == 1)
		{
			ProgramCounter = address;
		}
		break;

	case 0xA9: // JPL: abs Jump on postive result.
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if ((Flags&FLAG_N) == 0)
		{
			ProgramCounter = address;
		}
		break;

	case 0xAA://  JPL zpg
		address += 0x0000 | (WORD)fetch();
		if ((Flags&FLAG_N) == 0)
		{
			ProgramCounter = address;
		}

		break;
	case 0xAB://  JPL ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_N) == 0)
		{
			ProgramCounter = address;
		}

		break;
	case 0xAC://  JPL pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_N) == 0)
		{
			ProgramCounter = address;
		}
		break;
	case 0xB9: // JPE: abs Jump on result even.
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if ((Flags&FLAG_P) == 0)
		{
			ProgramCounter = address;
		}
		break;

	case 0xBA://  JPE jump on result even
		address += 0x0000 | (WORD)fetch();
		if ((Flags&FLAG_P) == 0)
		{
			ProgramCounter = address;
		}

		break;
	case 0xBB://  JPE ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_P) == 0)
		{
			ProgramCounter = address;
		}

		break;
	case 0xBC://  JPE pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_P) == 0)
		{
			ProgramCounter = address;
		}
		break;
	case 0xC9: // JPO: abs Jump on result odd.
		HB = fetch();
		LB = fetch();
		address += ((WORD)HB << 8) + (WORD)LB;
		if ((Flags&FLAG_P) == 1)
		{
			ProgramCounter = address;
		}
		break;

	case 0xCA://  JPO 
		address += 0x0000 | (WORD)fetch();
		if ((Flags&FLAG_P) == 1)
		{
			ProgramCounter = address;
		}

		break;
	case 0xCB://  JPO ind
		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		HB = Memory[address];
		LB = Memory[address + 1];
		address = (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_P) == 1)
		{
			ProgramCounter = address;
		}

		break;
	case 0xCC://  JPO pag
		HB = PageRegister;
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if ((Flags&FLAG_P) == 1)
		{
			ProgramCounter = address;
		}
		break;







	case 0x64: //RET return, opposite of JPR

		if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 2)){
			HB = Memory[StackPointer];
			StackPointer++;
			LB = Memory[StackPointer];
			StackPointer++;
			ProgramCounter = ((WORD)HB << 8) + (WORD)LB;
		}

		break;
	case 0x81://TAP
		PageRegister = Registers[REGISTER_A];
		Flags = Flags & (0xFF - FLAG_C);

		break;
	case 0x91://TPA
		Registers[REGISTER_A] = PageRegister;
		Flags = Flags & (0xFF - FLAG_C);

		break;


	case 0x2D:// INC abs
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE){

			++Memory[address];
			set_flag_p(Memory[address]);
			set_flag_n(Memory[address]);
			set_flag_z(Memory[address]);

		}
		break;
	case 0x2E:// INCA A

		++Registers[REGISTER_A];
		set_flag_p(Registers[REGISTER_A]);
		set_flag_n(Registers[REGISTER_A]);
		set_flag_z(Registers[REGISTER_A]);
		break;
	case 0x2F:// INCA B 

		++Registers[REGISTER_B];
		set_flag_p(Registers[REGISTER_B]);
		set_flag_n(Registers[REGISTER_B]);
		set_flag_z(Registers[REGISTER_B]);



		break;
	case 0x3D:// DEC ABS

		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE){

			--Memory[address];
			set_flag_p(Memory[address]);
			set_flag_n(Memory[address]);
			set_flag_z(Memory[address]);

		}
		break;
	case 0x3E:// DECA

		--Registers[REGISTER_A];
		set_flag_p(Registers[REGISTER_A]);
		set_flag_n(Registers[REGISTER_A]);
		set_flag_z(Registers[REGISTER_A]);

		break;
	case 0x3F:// DECB

		--Registers[REGISTER_B];
		set_flag_p(Registers[REGISTER_B]);
		set_flag_n(Registers[REGISTER_B]);
		set_flag_z(Registers[REGISTER_B]);
		break;


	case 0x18:// INP

		++PageRegister;

		set_flag_z(PageRegister);

	case 0x08:// DEP

		--PageRegister;

		set_flag_z(PageRegister);
		break;

	case 0x28:// DEZ

		--BaseRegister;

		set_flag_z(BaseRegister);
		break;
	case 0x38:// INZ

		++BaseRegister;

		set_flag_z(BaseRegister);



		break;
	case 0xD5://BT A-C
		temp_word = (WORD)Registers[REGISTER_A] & (WORD)Registers[REGISTER_C];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		/*	Flags = Flags & (0xFF - FLAG_V);*/

		break;
	case 0xD6://BT A-D
		temp_word = (WORD)Registers[REGISTER_A] & (WORD)Registers[REGISTER_D];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		/*Flags = Flags & (0xFF - FLAG_V);*/

		break;
	case 0xD7://BT B-C
		temp_word = (WORD)Registers[REGISTER_B] & (WORD)Registers[REGISTER_C];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		/*Flags = Flags & (0xFF - FLAG_V);*/

		break;
	case 0xD8://BT B-D
		temp_word = (WORD)Registers[REGISTER_B] & (WORD)Registers[REGISTER_D];
		set_flag_n((BYTE)temp_word);
		set_flag_z((BYTE)temp_word);
		set_flag_p((BYTE)temp_word);
		/*Flags = Flags & (0xFF - FLAG_V);*/

		break;
	case 0xDD:// CLR clear
		HB = fetch();
		LB = fetch();
		address += (WORD)((WORD)HB << 8) + LB;
		if (address >= 0 && address < MEMORY_SIZE){

			Memory[address] = 0;
			Flags = Flags | FLAG_Z;//Set carry flag
			Flags = Flags & (0xFF - FLAG_N);//Clear carry flag
			Flags = Flags & (0xFF - FLAG_C);

		}
		break;
	case 0xAE://NEGA 2's complement Memory or Accumulato
		Registers[REGISTER_A] = ~Registers[REGISTER_A] + 1;
		set_flag_p(Registers[REGISTER_A]);
		set_flag_n(Registers[REGISTER_A]);
		set_flag_z(Registers[REGISTER_A]);
		break;
	case 0xAF://NEGB 2's complement Memory or Accumulato
		Registers[REGISTER_B] = ~Registers[REGISTER_B] + 1;
		Flags = Flags | FLAG_Z;//Set carry flag
		Flags = Flags & (0xFF - FLAG_N);//Clear carry flag
		Flags = Flags & (0xFF - FLAG_P);
		break;
	case 0x9E://NOTA 
		temp_word = ~Registers[REGISTER_A];
		Registers[REGISTER_A] = ~Registers[REGISTER_A];

		set_flag_c(temp_word);
		set_flag_p(Registers[REGISTER_A]);
		set_flag_n(Registers[REGISTER_A]);
		set_flag_z(Registers[REGISTER_A]);
		break;
	case 0x9F://NOTA 
		temp_word = ~Registers[REGISTER_B];
		Registers[REGISTER_B] = ~Registers[REGISTER_B];

		set_flag_c(temp_word);
		set_flag_p(Registers[REGISTER_B]);
		set_flag_n(Registers[REGISTER_B]);
		set_flag_z(Registers[REGISTER_B]);
		break;
	case 0x9D://NOT
		HB = fetch();
		LB = fetch();

		address = (WORD)((WORD)HB << 8) + LB;
		Memory[address] = ~Memory[address];


		set_flag_c(Memory[address]);
		set_flag_p(Memory[address]);
		set_flag_n(Memory[address]);
		set_flag_z(Memory[address]);

		break;
	case 0xAD://NEG

		HB = fetch();
		LB = fetch();
		address = (WORD)((WORD)HB << 8) + LB;
		Memory[address] = ~Memory[address] + 1;
		set_flag_p(Memory[address]);
		set_flag_n(Memory[address]);
		set_flag_z(Memory[address]);
		break;


	case 0xDE://CLRA clear memory or accu A
		Registers[REGISTER_A] = 0;
		Flags = Flags | FLAG_Z;//Set carry flag
		Flags = Flags & (0xFF - FLAG_N);//Clear carry flag
		Flags = Flags & (0xFF - FLAG_C);
		break;
	case 0xDF://CLRB clear memory or accu B
		Registers[REGISTER_B] = 0;
		Flags = Flags | FLAG_Z;//Set carry flag
		Flags = Flags & (0xFF - FLAG_N);//Clear carry flag
		Flags = Flags & (0xFF - FLAG_C);
		break;
		


	case 0x10:  // HALT impl   

		halt = true;

		break;
	default:  // unimplemented instruction   
		printf("%02x\n2", opcode);
		halt = true;

		break;
	}
}

void Group_2_Move(BYTE opcode){
	BYTE destination = opcode >> 4;
	BYTE source = opcode & 0x0F;

	int destReg;
	int sourceReg;


	switch (destination) {
	case 0xA1:
		destReg = REGISTER_A;
		break;
	case 0xA2:
		destReg = REGISTER_B;
		break;
	case 0xA3:
		destReg = REGISTER_C;
		break;
	case 0xA4:
		destReg = REGISTER_D;
		break;
	case 0xB1:
		destReg = REGISTER_A;
		break;
	case 0xB2:
		destReg = REGISTER_B;
		break;
	case 0xB3:
		destReg = REGISTER_C;
		break;
	case 0xB4:
		destReg = REGISTER_D;
		break;
	case 0xC1:
		destReg = REGISTER_A;
		break;
	case 0xC2:
		destReg = REGISTER_B;
		break;
	case 0xC3:
		destReg = REGISTER_C;
		break;
	case 0xC4:
		destReg = REGISTER_D;
		break;
	case 0xD1:
		destReg = REGISTER_A;
		break;
	case 0xD2:
		destReg = REGISTER_B;
		break;
	case 0xD3:
		destReg = REGISTER_C;
		break;
	case 0xD4:
		destReg = REGISTER_D;
		break;

		
		Registers[destReg] = Registers[sourceReg];
	}
}

void execute(BYTE opcode)



{

	if (((opcode >= 0xA1) && (opcode <= 0xA4))
		|| ((opcode >= 0xB1) && (opcode <= 0xB4))
		|| ((opcode >= 0xC1) && (opcode <= 0xC4))
		|| ((opcode >= 0xD1) && (opcode <= 0xD4)))
	{
		Group_2_Move(opcode);
	}
	else
	{
		Group_1(opcode);
	}
}

void emulate()
{
	BYTE opcode;

	ProgramCounter = 0;
	halt = false;
	memory_in_range = true;

	printf("                    A  B  C  D  P  Z    SP\n");

	while ((!halt) && (memory_in_range)) {
		printf("%04X ", ProgramCounter);           // Print current address
		opcode = fetch();
		execute(opcode);

		printf("%s  ", opcode_mneumonics[opcode]);  // Print current opcode

		printf("%02X ", Registers[REGISTER_A]);
		printf("%02X ", Registers[REGISTER_B]);
		printf("%02X ", Registers[REGISTER_C]);
		printf("%02X ", Registers[REGISTER_D]);
		printf("%02X ", PageRegister);
		printf("%04X ", BaseRegister);
		printf("%04X ", StackPointer);              // Print Stack Pointer

		if ((Flags & FLAG_I) == FLAG_I)
		{
			printf("I=1 ");
		}
		else
		{
			printf("I=0 ");
		}
		if ((Flags & FLAG_Z) == FLAG_Z)
		{
			printf("Z=1 ");
		}
		else
		{
			printf("Z=0 ");
		}
		if ((Flags & FLAG_P) == FLAG_P)
		{
			printf("P=1 ");
		}
		else
		{
			printf("P=0 ");
		}
		if ((Flags & FLAG_V) == FLAG_V)
		{
			printf("V=1 ");
		}
		else
		{
			printf("V=0 ");
		}
		if ((Flags & FLAG_N) == FLAG_N)
		{
			printf("N=1 ");
		}
		else
		{
			printf("N=0 ");
		}
		if ((Flags & FLAG_C) == FLAG_C)
		{
			printf("C=1 ");
		}
		else
		{
			printf("C=0 ");
		}

		printf("\n");  // New line
	}

	printf("\n");  // New line
}


////////////////////////////////////////////////////////////////////////////////
//                            Simulator/Emulator (End)                        //
////////////////////////////////////////////////////////////////////////////////


void initialise_filenames() {
	int i;

	for (i = 0; i<MAX_FILENAME_SIZE; i++) {
		hex_file[i] = '\0';
		trc_file[i] = '\0';
	}
}




int find_dot_position(char *filename) {
	int  dot_position;
	int  i;
	char chr;

	dot_position = 0;
	i = 0;
	chr = filename[i];

	while (chr != '\0') {
		if (chr == '.') {
			dot_position = i;
		}
		i++;
		chr = filename[i];
	}

	return (dot_position);
}


int find_end_position(char *filename) {
	int  end_position;
	int  i;
	char chr;

	end_position = 0;
	i = 0;
	chr = filename[i];

	while (chr != '\0') {
		end_position = i;
		i++;
		chr = filename[i];
	}

	return (end_position);
}


bool file_exists(char *filename) {
	bool exists;
	FILE *ifp;

	exists = false;

	if ((ifp = fopen(filename, "r")) != NULL) {
		exists = true;

		fclose(ifp);
	}

	return (exists);
}



void create_file(char *filename) {
	FILE *ofp;

	if ((ofp = fopen(filename, "w")) != NULL) {
		fclose(ofp);
	}
}



bool getline(FILE *fp, char *buffer) {
	bool rc;
	bool collect;
	char c;
	int  i;

	rc = false;
	collect = true;

	i = 0;
	while (collect) {
		c = getc(fp);

		switch (c) {
		case EOF:
			if (i > 0) {
				rc = true;
			}
			collect = false;
			break;

		case '\n':
			if (i > 0) {
				rc = true;
				collect = false;
				buffer[i] = '\0';
			}
			break;

		default:
			buffer[i] = c;
			i++;
			break;
		}
	}

	return (rc);
}






void load_and_run(int args, _TCHAR** argv) {
	char chr;
	int  ln;
	int  dot_position;
	int  end_position;
	long i;
	FILE *ifp;
	long address;
	long load_at;
	int  code;

	// Prompt for the .hex file

	printf("\n");
	printf("Enter the hex filename (.hex): ");

	if (args == 2){
		ln = 0;
		chr = argv[1][ln];
		while (chr != '\0')
		{
			if (ln < MAX_FILENAME_SIZE)
			{
				hex_file[ln] = chr;
				trc_file[ln] = chr;
				ln++;
			}
			chr = argv[1][ln];
		}
	}
	else {
		ln = 0;
		chr = '\0';
		while (chr != '\n') {
			chr = getchar();

			switch (chr) {
			case '\n':
				break;
			default:
				if (ln < MAX_FILENAME_SIZE)	{
					hex_file[ln] = chr;
					trc_file[ln] = chr;
					ln++;
				}
				break;
			}
		}

	}
	// Tidy up the file names

	dot_position = find_dot_position(hex_file);
	if (dot_position == 0) {
		end_position = find_end_position(hex_file);

		hex_file[end_position + 1] = '.';
		hex_file[end_position + 2] = 'h';
		hex_file[end_position + 3] = 'e';
		hex_file[end_position + 4] = 'x';
		hex_file[end_position + 5] = '\0';
	}
	else {
		hex_file[dot_position + 0] = '.';
		hex_file[dot_position + 1] = 'h';
		hex_file[dot_position + 2] = 'e';
		hex_file[dot_position + 3] = 'x';
		hex_file[dot_position + 4] = '\0';
	}

	dot_position = find_dot_position(trc_file);
	if (dot_position == 0) {
		end_position = find_end_position(trc_file);

		trc_file[end_position + 1] = '.';
		trc_file[end_position + 2] = 't';
		trc_file[end_position + 3] = 'r';
		trc_file[end_position + 4] = 'c';
		trc_file[end_position + 5] = '\0';
	}
	else {
		trc_file[dot_position + 0] = '.';
		trc_file[dot_position + 1] = 't';
		trc_file[dot_position + 2] = 'r';
		trc_file[dot_position + 3] = 'c';
		trc_file[dot_position + 4] = '\0';
	}

	if (file_exists(hex_file)) {
		// Clear Registers and Memory

		Registers[REGISTER_A] = 0;
		Registers[REGISTER_B] = 0;
		Registers[REGISTER_C] = 0;
		Registers[REGISTER_D] = 0;
		PageRegister = 0;
		BaseRegister = 0;
		Flags = 0;
		ProgramCounter = 0;
		StackPointer = 0;

		for (i = 0; i<MEMORY_SIZE; i++) {
			Memory[i] = 0x00;
		}

		// Load hex file

		if ((ifp = fopen(hex_file, "r")) != NULL) {
			printf("Loading file...\n\n");

			load_at = 0;

			while (getline(ifp, InputBuffer)) {
				if (sscanf(InputBuffer, "L=%x", &address) == 1) {
					load_at = address;
				}
				else if (sscanf(InputBuffer, "%x", &code) == 1) {
					if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
						Memory[load_at] = (BYTE)code;
					}
					load_at++;
				}
				else {
					printf("ERROR> Failed to load instruction: %s \n", InputBuffer);
				}
			}

			fclose(ifp);
		}

		// Emulate

		emulate();
	}
	else {
		printf("\n");
		printf("ERROR> Input file %s does not exist!\n", hex_file);
		printf("\n");
	}
}

void building(int args, _TCHAR** argv){
	char buffer[1024];
	load_and_run(args, argv);
	sprintf(buffer, "0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X",
		Memory[TEST_ADDRESS_1],
		Memory[TEST_ADDRESS_2],
		Memory[TEST_ADDRESS_3],
		Memory[TEST_ADDRESS_4],
		Memory[TEST_ADDRESS_5],
		Memory[TEST_ADDRESS_6],
		Memory[TEST_ADDRESS_7],
		Memory[TEST_ADDRESS_8],
		Memory[TEST_ADDRESS_9],
		Memory[TEST_ADDRESS_10],
		Memory[TEST_ADDRESS_11],
		Memory[TEST_ADDRESS_12]
		);
	sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
}



void test_and_mark() {
	char buffer[1024];
	bool testing_complete;
	int  len = sizeof(SOCKADDR);
	char chr;
	int  i;
	int  j;
	bool end_of_program;
	long address;
	long load_at;
	int  code;
	int  mark;
	int  passed;

	printf("\n");
	printf("Automatic Testing and Marking\n");
	printf("\n");

	testing_complete = false;

	sprintf(buffer, "Test Student %s", STUDENT_NUMBER);
	sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));

	while (!testing_complete) {
		memset(buffer, '\0', sizeof(buffer));

		if (recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (SOCKADDR *)&client_addr, &len) != SOCKET_ERROR) {
			printf("Incoming Data: %s \n", buffer);

			//if (strcmp(buffer, "Testing complete") == 1)
			if (sscanf(buffer, "Testing complete %d", &mark) == 1) {
				testing_complete = true;
				printf("Current mark = %d\n", mark);

			}
			else if (sscanf(buffer, "Tests passed %d", &passed) == 1) {
				//testing_complete = true;
				printf("Passed = %d\n", passed);

			}
			else if (strcmp(buffer, "Error") == 0) {
				printf("ERROR> Testing abnormally terminated\n");
				testing_complete = true;
			}
			else {
				// Clear Registers and Memory

				Registers[REGISTER_A] = 0;
				Registers[REGISTER_B] = 0;
				Registers[REGISTER_C] = 0;
				Registers[REGISTER_D] = 0;
				PageRegister = 0;
				BaseRegister = 0;
				Flags = 0;
				ProgramCounter = 0;
				StackPointer = 0;
				for (i = 0; i<MEMORY_SIZE; i++) {
					Memory[i] = 0;
				}

				// Load hex file

				i = 0;
				j = 0;
				load_at = 0;
				end_of_program = false;
				FILE *ofp;
				fopen_s(&ofp, "branch.txt", "a");

				while (!end_of_program) {
					chr = buffer[i];
					switch (chr) {
					case '\0':
						end_of_program = true;

					case ',':
						if (sscanf(InputBuffer, "L=%x", &address) == 1) {
							load_at = address;
						}
						else if (sscanf(InputBuffer, "%x", &code) == 1) {
							if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
								Memory[load_at] = (BYTE)code;
								fprintf(ofp, "%02X\n", (BYTE)code);
							}
							load_at++;
						}
						else {
							printf("ERROR> Failed to load instruction: %s \n", InputBuffer);
						}
						j = 0;
						break;

					default:
						InputBuffer[j] = chr;
						j++;
						break;
					}
					i++;
				}
				fclose(ofp);
				// Emulate

				if (load_at > 1) {
					emulate();
					// Send and store results
					sprintf(buffer, "%02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X",
						Memory[TEST_ADDRESS_1],
						Memory[TEST_ADDRESS_2],
						Memory[TEST_ADDRESS_3],
						Memory[TEST_ADDRESS_4],
						Memory[TEST_ADDRESS_5],
						Memory[TEST_ADDRESS_6],
						Memory[TEST_ADDRESS_7],
						Memory[TEST_ADDRESS_8],
						Memory[TEST_ADDRESS_9],
						Memory[TEST_ADDRESS_10],
						Memory[TEST_ADDRESS_11],
						Memory[TEST_ADDRESS_12]
						);
					sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
				}
			}
		}
	}
}



int _tmain(int argc, _TCHAR* argv[])
{
	char chr;
	char dummy;

	printf("\n");
	printf("Microprocessor Emulator\n");
	printf("UWE Computer and Network Systems Assignment 1\n");
	printf("\n");

	initialise_filenames();

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return(0);

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	if (!sock) {
		// Creation failed! 
	}

	memset(&server_addr, 0, sizeof(SOCKADDR_IN));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
	server_addr.sin_port = htons(PORT_SERVER);

	memset(&client_addr, 0, sizeof(SOCKADDR_IN));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	client_addr.sin_port = htons(PORT_CLIENT);

	chr = '\0';
	while ((chr != 'e') && (chr != 'E'))
	{
		printf("\n");
		printf("Please select option\n");
		printf("L - Load and run a hex file\n");
		printf("T - Have the server test and mark your emulator\n");
		printf("E - Exit\n");
		if (argc == 2){ building(argc, argv); exit(0); }
		printf("Enter option: ");
		chr = getchar();
		if (chr != 0x0A)
		{
			dummy = getchar();  // read in the <CR>
		}
		printf("\n");

		switch (chr)
		{
		case 'L':
		case 'l':
			load_and_run(argc, argv);
			break;

		case 'T':
		case 't':
			test_and_mark();
			break;

		default:
			break;
		}
	}

	closesocket(sock);
	WSACleanup();


	return 0;
}


