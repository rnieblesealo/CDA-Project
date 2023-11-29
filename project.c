#include "spimcore.h"

void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero) 
{
    unsigned Z;

    //ADD
    if (ALUControl == 0) {
       Z=A+B;
       //printf("add");
    }
    
    //SUB
    else if (ALUControl == 1) {
       Z=A-B;
       //printf("sub");
    }
     
    //SLT *set A and B to int to account of signs*
    else if (ALUControl == 2) {
        if ((int)A<(int)B)
          Z = 1;
        else
          Z = 0;
    }
     
    //SLTU 
    else if (ALUControl == 3) {
        if (A<B)
          Z = 1;
        else
          Z = 0;
    }
    
    //AND
    else if (ALUControl == 4)
        Z = A&B;

    //OR
    else if (ALUControl == 5)
        Z = A|B;

    //SLL
    else if (ALUControl == 6)
        Z = B << 16;
    
    //NOT
    else if (ALUControl == 7)
        Z = ~A;
    
    // set ALUresult to the output of operations (Z)
    *ALUresult = Z;
    
    // set Zero to 0 or 1 depending on value of ALUresult
    if (*ALUresult == 0)
        *Zero = 1;

    else
        *Zero = 0;
}


int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	// case to check if word aligned (mult of 4)
	if (PC % 4 != 0) {
		// flag for halt
		return 1;
	}

	// fetch instruction
	*instruction = Mem[PC >> 2];
	return 0;

}

int subset(int word, int start, int end);

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
    // initialize all control signals to 0
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
          
          // for all r-types
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

        // 0000 0100 beq
        case 4:
          controls->RegDst = 2;
          controls->MemtoReg = 2;
          controls->Branch = 1;
          
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

        default:
          // case to assert halt
          return 1;
    }

    // else return 0 and deassert Halt
    return 0;

}

void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
  *data1 = Reg[r1];
  *data2 = Reg[r2];
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