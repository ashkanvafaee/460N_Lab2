/*
REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

Name 1: Ashkan Vafaee
Name 2: Genki Oji
UTEID 1: av28837
UTEID 2: gto99
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;        /* run bit */


typedef struct System_Latches_Struct {

	int PC,                /* program counter */
		N,                /* n condition bit */
		Z,                /* z condition bit */
		P;                /* p condition bit */
	int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
	printf("----------------LC-3b ISIM Help-----------------------\n");
	printf("go               -  run program to completion         \n");
	printf("run n            -  execute program for n instructions\n");
	printf("mdump low high   -  dump memory from low to high      \n");
	printf("rdump            -  dump the register & bus values    \n");
	printf("?                -  display this help menu            \n");
	printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

	process_instruction();
	CURRENT_LATCHES = NEXT_LATCHES;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
	int i;

	if (RUN_BIT == FALSE) {
		printf("Can't simulate, Simulator is halted\n\n");
		return;
	}

	printf("Simulating for %d cycles...\n\n", num_cycles);
	for (i = 0; i < num_cycles; i++) {
		if (CURRENT_LATCHES.PC == 0x0000) {
			RUN_BIT = FALSE;
			printf("Simulator halted\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
	if (RUN_BIT == FALSE) {
		printf("Can't simulate, Simulator is halted\n\n");
		return;
	}

	printf("Simulating...\n\n");
	while (CURRENT_LATCHES.PC != 0x0000)
		cycle();
	RUN_BIT = FALSE;
	printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
	int address; /* this is a byte address */

	printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
	printf("-------------------------------------\n");
	for (address = (start >> 1); address <= (stop >> 1); address++)
		printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
	printf("\n");

	/* dump the memory contents into the dumpsim file */
	fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
	fprintf(dumpsim_file, "-------------------------------------\n");
	for (address = (start >> 1); address <= (stop >> 1); address++)
		fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
	fprintf(dumpsim_file, "\n");
	fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
	int k;

	printf("\nCurrent register/bus values :\n");
	printf("-------------------------------------\n");
	printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
	printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
	printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	printf("Registers:\n");
	for (k = 0; k < LC_3b_REGS; k++)
		printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
	printf("\n");

	/* dump the state information into the dumpsim file */
	fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
	fprintf(dumpsim_file, "-------------------------------------\n");
	fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
	fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
	fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	fprintf(dumpsim_file, "Registers:\n");
	for (k = 0; k < LC_3b_REGS; k++)
		fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
	fprintf(dumpsim_file, "\n");
	fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
	char buffer[20];
	int start, stop, cycles;

	printf("LC-3b-SIM> ");

	scanf("%s", buffer);
	printf("\n");

	switch (buffer[0]) {
	case 'G':
	case 'g':
		go();
		break;

	case 'M':
	case 'm':
		scanf("%i %i", &start, &stop);
		mdump(dumpsim_file, start, stop);
		break;

	case '?':
		help();
		break;
	case 'Q':
	case 'q':
		printf("Bye.\n");
		exit(0);

	case 'R':
	case 'r':
		if (buffer[1] == 'd' || buffer[1] == 'D')
			rdump(dumpsim_file);
		else {
			scanf("%d", &cycles);
			run(cycles);
		}
		break;

	default:
		printf("Invalid Command\n");
		break;
	}
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
	int i;

	for (i = 0; i < WORDS_IN_MEM; i++) {
		MEMORY[i][0] = 0;
		MEMORY[i][1] = 0;
	}
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
	FILE * prog;
	int ii, word, program_base;

	/* Open program file. */
	prog = fopen(program_filename, "r");
	if (prog == NULL) {
		printf("Error: Can't open program file %s\n", program_filename);
		exit(-1);
	}

	/* Read in the program. */
	if (fscanf(prog, "%x\n", &word) != EOF)
		program_base = word >> 1;
	else {
		printf("Error: Program file is empty\n");
		exit(-1);
	}

	ii = 0;
	while (fscanf(prog, "%x\n", &word) != EOF) {
		/* Make sure it fits. */
		if (program_base + ii >= WORDS_IN_MEM) {
			printf("Error: Program file %s is too long to fit in memory. %x\n",
				program_filename, ii);
			exit(-1);
		}

		/* Write the word to memory array. */
		MEMORY[program_base + ii][0] = word & 0x00FF;
		MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
		ii++;
	}

	if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

	printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
	int i;

	init_memory();
	for (i = 0; i < num_prog_files; i++) {
		load_program(program_filename);
		while (*program_filename++ != '\0');
	}
	CURRENT_LATCHES.Z = 1;
	NEXT_LATCHES = CURRENT_LATCHES;

	RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
	FILE * dumpsim_file;

	/* Error Checking */
	if (argc < 2) {
		printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
			argv[0]);
		exit(1);
	}

	printf("LC-3b Simulator\n\n");

	initialize(argv[1], argc - 1);

	if ((dumpsim_file = fopen("dumpsim", "w")) == NULL) {
		printf("Error: Can't open dumpsim file\n");
		exit(-1);
	}

	while (1)
		get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
You are allowed to use the following global variables in your
code. These are defined above.

MEMORY

CURRENT_LATCHES
NEXT_LATCHES

You may define your own local/global variables and functions.
You may use the functions to get at the control bits defined
above.

Begin your code here                                          */

/***************************************************************/



void process_instruction() {
	/*  function: process_instruction
	*
	*    Process one instruction at a time
	*       -Fetch one instruction
	*       -Decode
	*       -Execute
	*       -Update NEXT_LATCHES
	*/

	/* Instructions */
	enum {
		ADD = 1, AND = 5, BR = 0, XOR = 9, NOT = 9, JMP = 12, RET = 12, JSR = 4, JSRR = 4, LDB = 2, LDW = 6, LEA = 14, LSHF = 13, RSHFL = 13, RSHFA = 13,
		STB = 3, STW = 7, TRAP = 15
	};

	
	/* Bit Masks */
	enum {
		reg = 0b111, DR_shift = 9, SR1_shift = 6, SR2_shift = 0, BaseR_shift = 6, bit4 = 0b10000, bit5 = 0b100000, imm5 = 0b11111, sign_extend16 = 0xFFFF0000, sign_bit16 = 0b1000000000000000,
		sign_bit5 = 0b10000, sign_extend5 = 0xFFFFFFE0, nzpbits_shift = 9, PCoffset9 = 0b111111111, sign_bit9 = 0x100, sign_extend9 = 0xFFFFFE00, amount4 = 0b1111, trapvect8 = 0xFF,
		PCoffset11 = 0b11111111111, sign_extend11 = 0xFFFFF800, nzp_shift = 9, nzp_values = 0b111, bit11 = 0x800, offset6 = 0b111111, sign_bit6 = 0b100000, sign_extend6 = 0xFFFFFFC0, 

	};


	int instruction = (MEMORY[CURRENT_LATCHES.PC >> 1][1] << 8) | (MEMORY[CURRENT_LATCHES.PC >> 1][0]);
	int opcode = MEMORY[CURRENT_LATCHES.PC >> 1 ][1] >> 4;




	if (opcode == ADD || opcode == AND || opcode == XOR || opcode == NOT) {
		int result, op1, op2;


		/* imm5 */
		if (instruction & bit5) {
			op2 = instruction & imm5;
			op2 = op2 & sign_bit5 ? op2 | sign_extend5 : op2;
		}
		/* SR2 */
		else {
			op2 = CURRENT_LATCHES.REGS[(instruction >> SR2_shift) & reg];
			op2 = op2 & sign_bit16 ? op2 | sign_extend16 : op2;
		}

		op1 = CURRENT_LATCHES.REGS[(instruction >> SR1_shift) & reg];
		op1 = op1 & sign_bit16 ? op1 | sign_extend16 : op1;

		if (opcode == ADD) {
			result = op1 + op2;
		}
		else if (opcode == AND) {
			result = op1 & op2;
		}
		else if (opcode == XOR) {
			result = op1 ^ op2;			/* double check this later */
		}
		


		/* Correcting Overflow */
		result = result & sign_bit16 ? result | sign_extend16 : Low16bits(result);

		if (result < 0) {
			NEXT_LATCHES.N = 1;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 0;
		}
		else if (result == 0) {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 1;
			NEXT_LATCHES.P = 0;
		}
		else if (result > 0) {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 1;
		}

		result = Low16bits(result);
		NEXT_LATCHES.REGS[(instruction >> DR_shift) & reg] = result;
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}
	else if (opcode == BR) {
		int required_nzpBits, current_nzpBits, PC_Offset;


		required_nzpBits = (instruction >> nzpbits_shift) & nzp_values;
		current_nzpBits = (CURRENT_LATCHES.N << 2) | (CURRENT_LATCHES.Z << 1) | CURRENT_LATCHES.P;
		PC_Offset = instruction & PCoffset9;
		PC_Offset = PC_Offset & sign_bit9 ? PC_Offset | sign_extend9 : PC_Offset;
		PC_Offset = PC_Offset << 1;

		/* Branch Taken */
		if (current_nzpBits & required_nzpBits) {
			NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + PC_Offset;
		}
		/* Branch Not Taken */
		else {
			NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
		}
	}

	else if (opcode == JMP || opcode == RET) {
		NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[(instruction >> BaseR_shift) & reg];
	}

	else if (opcode == JSR || opcode == JSRR) {
		int TEMP = CURRENT_LATCHES.PC + 2;

		/* JSR */
		if (instruction & bit11) {
			int PC_Offset = instruction & PCoffset11;
			PC_Offset = PC_Offset & sign_bit9 ? PC_Offset | sign_extend9 : PC_Offset;
			PC_Offset = PC_Offset << 1;
			NEXT_LATCHES.PC = TEMP + PC_Offset;
		}

		/* JSRR */
		else {
			NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[(instruction >> BaseR_shift) & reg];
		}
		NEXT_LATCHES.REGS[7] = TEMP;
	}

	else if (opcode == LDB || opcode == LDW) {
		int BaseR, offset, result;

		BaseR = CURRENT_LATCHES.REGS[(instruction >> BaseR_shift) & reg];
		BaseR = (BaseR & sign_bit16) ? BaseR | sign_extend16 : BaseR;

		offset = instruction & offset6;
		offset = offset & sign_bit6 ? offset | sign_extend6 : offset;

		
		if (opcode == LDB) {

			/* Even address -> accessing Memory[#][0] */
			if ((BaseR + offset) % 2 == 0) {
				result = MEMORY[(Low16bits(BaseR + offset)) >> 1][0];
			}

			/* Odd address -> accessing Memory[#][1] */
			else {
				result = MEMORY[(Low16bits(BaseR + offset)) >> 1][1];
			}
		}
		/* LDW */
		else {
			offset = offset << 1;			/* Left Shifting offset by 1 for LDW */
			result = (MEMORY[(BaseR + offset) >> 1][1] << 8) | MEMORY[(BaseR + offset) >> 1][0];
		}

		result = (result & sign_bit16) ? result | sign_extend16 : result;

		if (result < 0) {
			NEXT_LATCHES.N = 1;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 0;
		}
		else if (result == 0) {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 1;
			NEXT_LATCHES.P = 0;
		}
		else if (result > 0) {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 1;
		}

		result = Low16bits(result);
		NEXT_LATCHES.REGS[(instruction >> DR_shift) & reg] = result;
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}

	else if (opcode == LEA) {
		int PC_Offset, result;

		PC_Offset = instruction & PCoffset9;
		PC_Offset = PC_Offset & sign_bit9 ? PC_Offset | sign_extend9 : PC_Offset;

		result = CURRENT_LATCHES.PC + 2 + (PC_Offset << 1);
		result = Low16bits(result);

		NEXT_LATCHES.REGS[(instruction >> DR_shift) & reg] = result;
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}

	else if (opcode == LSHF || opcode == RSHFL || opcode == RSHFA) {
		int result, SR, mask;

		SR = CURRENT_LATCHES.REGS[((instruction >> SR1_shift) & reg)];
		SR = SR & sign_bit16 ? SR | sign_extend16 : SR;
		

		if ((instruction & bit4) == 0) {
			result = SR << (instruction & amount4);
		}
		else {
			/* Logical shift right */
			if ((instruction & bit5) == 0) {
				mask = 0xFFFF;

				/* Shifting  mask bits to the right by the amount of 0s that should be in front of result */
				for (int i = 0; i < (instruction & amount4); i++) {
					mask = mask >> 1;
				}

				result = SR >> (instruction & amount4);		
				result = result & mask;
			}

			/* Arithmetic shift right */
			else {

				/* Preserving sign bit if it's a 1 */
				if (SR & sign_bit16) {
					mask = 0x8000;

					for (int i = 0; i < (instruction & amount4); i++) {
						mask = mask >> 1;
						mask = mask | 0x8000;
					}

					result = SR >> (instruction & amount4);
					result = result | mask;
				}

				/* Preserving sign bit if it's a 0 */
				else {
					mask = 0xFFFF;

					for (int i = 0; i < (instruction & amount4); i++) {
						mask = mask >> 1;
					}

					result = SR >> (instruction & amount4);
					result = result & mask;
				}
			}
		}

		result = (result & sign_bit16) ? result | sign_extend16 : result;

		if (result < 0) {
			NEXT_LATCHES.N = 1;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 0;
		}
		else if (result == 0) {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 1;
			NEXT_LATCHES.P = 0;
		}
		else if (result > 0) {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 1;
		}

		result = Low16bits(result);
		NEXT_LATCHES.REGS[(instruction >> DR_shift) & reg] = result;
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}


	else if (opcode == STB || opcode == STW) {
		int BaseR, offset, result_address;

		BaseR = CURRENT_LATCHES.REGS[(instruction >> BaseR_shift) & reg];
		BaseR = BaseR & sign_bit16 ? BaseR | sign_extend16 : BaseR;

		offset = instruction & offset6;
		offset = offset & sign_bit6 ? offset | sign_extend6 : offset;

		if (opcode == STB) {
			result_address = BaseR + offset;
			result_address = Low16bits(result_address);

			if ((result_address % 2) == 0) {
				MEMORY[result_address >> 1][0] = CURRENT_LATCHES.REGS[(instruction >> DR_shift) & reg] & 0xFF;
			}
			else {
				MEMORY[result_address >> 1][1] = CURRENT_LATCHES.REGS[(instruction >> DR_shift) & reg] & 0xFF;
			}
		}
		else {
			result_address = BaseR + (offset << 1);
			result_address = Low16bits(result_address);

			MEMORY[result_address >> 1][0] = CURRENT_LATCHES.REGS[(instruction >> DR_shift) & reg];
		}


		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}

	else if (opcode == TRAP) {
		int address;

		NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC + 2;

		address = (instruction & trapvect8) << 1;
		NEXT_LATCHES.PC = (MEMORY[address >> 1][1] << 8) | MEMORY[address >> 1][0];
	}


}