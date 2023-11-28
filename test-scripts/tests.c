#include <stdio.h>
#include <stdlib.h>

void print_binary(int a, int trim){
	// If amount of bits to print exceeds size of an int, cap it at that size
	if (trim > 31)
		trim = 31;
	
	// Reduce trim by 1 to make bits printed exclusive
	trim -= 1;

	// Make mask such that we print bits left to right
	unsigned int mask = 1 << trim;

	// Print each bit using the mask; space every 8 bits
	int c = 0;
	while (mask){
		printf("%d", a & mask ? 1 : 0);
		mask >>= 1;

		if (c == 7){
			printf(" ");
			c = 0;
		}

		else{
			c++;
		}
	}

	// Newline at end
	printf("\n");
}

int mask(int word, int start, int end){
	if (start > end)
		return 0;

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

void instruction_partition(unsigned instruction){
	printf("Instruction:\n"); print_binary(instruction, 32);
	
	printf("opcode:\n"); print_binary(mask(instruction, 26, 31), 6);
	printf("rs:\n"); print_binary(mask(instruction, 21, 25), 5);
	printf("rt:\n"); print_binary(mask(instruction, 16, 20), 5);
	printf("rd:\n");print_binary(mask(instruction, 11, 15), 5);
}

int main(){
	instruction_partition(36847648);

	return 0;
}
