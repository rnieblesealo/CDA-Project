#include "spimcore.h"

void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{

}

int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{

}

void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // Extract the subset of bits from the instruction pertaining to the different components and store them
	
	*op = subset(instruction, 26, 31);
	*r1 = subset(instruction, 21, 25);
	*r2 = subset(instruction, 16, 20);
	*r3 = subset(instruction, 11, 15);

	*funct = subset(instruction, 0, 5);
	*offset = subset(instruction, 0, 15);
	*jsec = subset(instruction, 0, 25);
}

int instruction_decode(unsigned op,struct_controls *controls)
{
    
}

void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{

}


void sign_extend(unsigned offset,unsigned *extended_value)
{

}

int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}

void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}

int subset(int word, int start, int end){
	// Returns the subset of bits in words in (start, end) inclusively
    
    if (start > end){
		printf("Subset returning 0; start greater than end!");
		return 0;
	}

	// Initialize mask
	int mask = 1;

	// Make mask span from start to end, inclusive
	mask <<= ((end - start) + 1);
	mask -= 1;

	// Shift mask to start spot 
	mask <<= start;

	// printf("Original word: ");
	// print_binary(word, 32);

	// printf("Mask: ");
	// print_binary(mask, 32);

	// Apply mask to word
	word &= mask;

	// printf("Masked word: ");
	// print_binary(word, 32);

	// Shift to beginning
	word >>= start;
	
	// printf("Shifted word: ");
	// print_binary(word, 32);

	return word;
}