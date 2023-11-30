#include "spimcore.h"

// Utility functions, not part of project
int is_word_aligned(unsigned n);
unsigned subset(unsigned word, int start, int end);

// Project functions:

void ALU(unsigned A, unsigned B, char ALUControl,unsigned *ALUresult,char *Zero){
	unsigned Z = 0;

	// Perform ALU operation; store it into temp value Z
	switch (ALUControl){
		// ADD
		case 0:
			Z = A + B;
			break;
		// SUB
		case 1:
			Z = A - B;
			break;
		// SLT --Accounts for signage by using int typecast
		case 2:
			Z = ((int)A) < ((int)B) ? 1 : 0;
			break;
		// SLTU
		case 3:
			Z = (A < B) ? 1 : 0;
			break;
		// AND
		case 4:
			Z = A & B;
			break;
		// OR
		case 5:
			Z = A | B;
			break;
		// SLL
		case 6:
			Z = B << 16;
			break;			
		// NOT
		case 7:
			Z = ~A;
			break;
	}
	
	// Set ALUresult to computed value
	*ALUresult = Z;

	// From it, determine value of zero
	if (*ALUresult == 0) 
		*Zero = 1;
	else
		*Zero = 0;
}

int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction){
	// If not word aligned, flag halt
	if (!is_word_aligned(PC)) {
		return 1;
	}

	// Fetch instruction otherwise
	*instruction = Mem[PC >> 2];

	return 0;
}

void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec){
	// Extract the subset of bits from the instruction pertaining to the different components and store them
	*op = subset(instruction, 26, 31);
	*r1 = subset(instruction, 21, 25);
	*r2 = subset(instruction, 16, 20);
	*r3 = subset(instruction, 11, 15);

	*funct = subset(instruction, 0, 5);
	*offset = subset(instruction, 0, 15);
	*jsec = subset(instruction, 0, 25);
}

int instruction_decode(unsigned op,struct_controls *controls){
	// Initialize all control signals to 0
	controls->ALUOp = 0;
	controls->ALUSrc = 0;
	controls->Branch = 0;
	controls->Jump = 0;
	controls->MemRead = 0;
	controls->MemtoReg = 0;
	controls->MemWrite = 0;
	controls->RegDst = 0;
	controls->RegWrite = 0;

	switch(op) {
		// 0000 0000 r-types (add, sub, &, or, slt, sltu)
		case 0:
			controls->RegDst = 1;
			controls->RegWrite = 1;

			// For all r-types
			controls->ALUOp = 7;
			break;

		// 0010 0011 lw
		case 35:
			controls->ALUSrc = 1;
			controls-> MemtoReg = 1;
			controls->RegWrite = 1;
			controls->MemRead = 1;
			break;

		// 0010 1011 sw
		case 43:
			controls->RegDst = 2;
			controls->ALUSrc = 1;
			controls->MemtoReg = 2;
			controls->MemWrite = 1;
			break;

		// 0000 1000 addi
		case 8:
			controls->ALUSrc = 1;
			controls->RegWrite = 1;

			// add ALUopcode
			controls->ALUOp = 0;
			break;

		// 0000 1111 lui
		case 15:
			controls->ALUOp = 6;
			controls->RegWrite = 1;
			controls->ALUSrc = 1;
			break;

		// 0000 0100 beq
		case 4:
			controls->RegDst = 2;
			controls->MemtoReg = 2;
			controls->Branch = 1;
			controls->ALUSrc = 1;

			// sub ALUopcode
			controls->ALUOp = 1;
			break;

		//0000 1010 slti
		case 10:
			controls->RegWrite = 1;
			controls->ALUSrc = 1;

			// slt ALUopcode
			controls->ALUOp = 2;
			break;

		// 0000 1011 sltiu
		case 11:
			controls->RegWrite = 1;
			controls->ALUSrc = 1;

			// sltu ALUopcode
			controls->ALUOp = 3;
			break;

		// 0000 0010 j
		case 2:
			controls->Jump = 1;
			controls->Branch = 2;
			controls->MemtoReg = 2;
			controls->ALUSrc = 2;
			controls->RegDst = 2;

			// slt ALUopcode
			controls->ALUOp = 2;
			break;
		// Case to assert halt
		default:
			return 1;
	}

	// Else return 0 and deassert Halt
	return 0;
}

void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2){
	// Copy data from registers onto cache
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}

void sign_extend(unsigned offset,unsigned *extended_value){
	// Check if MSB of value to be offset is either 1 or 0 (Shift by 15 places left to only leave this bit)
	unsigned msb = offset >> (16 - 1);
	
	// If MSB is 1, fill left half of extended value with 1's; think 2's complement!
	if (msb == 1)
		*extended_value = offset | 0xffff0000;
	else
		*extended_value = offset & 0x0000ffff;
}

int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero){
	// Halt if instruction invalid
	if (ALUOp>7 || ALUOp<0)
		return 1;

	if (ALUSrc == 1)
		data2 = extended_value;

	unsigned ALUControl = ALUOp;

	// R-type operations using decoded funct value
	if (ALUOp == 7) {
		switch(funct) {
			// add 
			case 32:
				ALUControl = 0;
				break;
			// sub
			case 34:
				ALUControl = 1;
				break;
			// slt
			case 42:
				ALUControl = 2;
				break;
			// sltu
			case 43:
				ALUControl = 3;
				break;
			// and
			case 36:
				ALUControl = 4;
				break;
			// or
			case 37:
				ALUControl = 5;
				break;
			// sll
			case 6:
				ALUControl = 6;
				break;
			// not
			case 39:
				ALUControl = 7;
				break;
			// Halt case
			default:
				return 1;
		}
	}

	// Run ALU function after getting proper ALUControl value
	ALU(data1, data2, ALUControl, ALUresult, Zero);

	return 0;
}

int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem){
	/* Parameters:
	 *
	 * Mem (array) = memory (array)
	 * MemRead = should we read from memory (0 or 1)
	 * MemWrite = should we write to memory (0 or 1)
	 * memdata (pointer) = location to WRITE data from memory to (pointer)
	 * ALUresult = memory read/write destination index (value)
	 *
	 * Notes:
	 *
	 * To check for word-alignment, check if is a multiple of 4
	 * To index from memory, we must shift right by 2 given that we have an array of 32-bit values
	 * Global halt flag is 1
	 */

	if(MemRead == 1){
		// Check if ALUresult follows word alignment
		if (is_word_aligned(ALUresult))
			*memdata = Mem[ALUresult >> 2];

		// If value read from memory is not word-aligned, halt
		else
			return 1;

	}

	if (MemWrite == 1){
		// If data is not word aligned, halt
		if (!is_word_aligned(ALUresult))
			return 1;

		// Otherwise, write value into memory
		Mem[ALUresult >> 2] = data2;
	}

	return 0;
}

void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg){
	/* Parameters:
	 *
	 * Reg = array of registers
	 * r2 = index of register A (value)
	 * r3 = index of register B (value)
	 * RegWrite = should we write to a register? (0 or 1)
	 * RegDst = specifies which register to write to (0 for r2, 1 for r3)
	 * MemtoReg = should we write data from memory? (0 if from ALU, 1 if from memory)
	 * memdata = data from memory to be written (value)
	 * ALUresult = data from ALU to be written (value)
	 */

	// Do not perform a write if RegWrite is deasserted
	if (RegWrite == 0)
		return;

	// from memory
	else if (RegWrite == 1){
		if (MemtoReg == 0) {
			// r-type instruct
			if(RegDst == 1)
				Reg[r3] = ALUresult;

			// i-type instruct
			else
				Reg[r2] = ALUresult;
		}

		else if (MemtoReg == 1){
			Reg[r2] = memdata;
		}
	}
}

void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC){
	// Update the program counter (PC)
	*PC += 4;

	// Account for jump and branch
	if (Jump == '1')
		*PC = (jsec<<2) | (*PC & (15 << 28));

	else if (Zero == 1 && Branch == 1)
		*PC += extended_value<<2;
}

// User-defined for utility, not graded!

int is_word_aligned(unsigned n){
	// A word is aligned if it's a multiple of 4
	return n % 4 == 0;
}

unsigned subset(unsigned value, int start, int end) {
	// do not execute if endpoints invalid
	if (start > end) {
		return 0;
	}

	// get number of bits in subset
	int numBits = (end - start) + 1;

	// mask bits from start to edn
	unsigned mask = (1 << numBits) - 1;

	// shift mask
	mask <<= start;
	value &= mask;

	// shift val
	value >>= start;

	return value;
}
