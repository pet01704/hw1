#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "graph.h"
// this comment should be removed
int num_blocks;
target *blocks[MAX_TARGETS];

int num_recipies;
char * recipies[MAX_RECIPES_PT*MAX_TARGETS];

//frees the memory alocated for everything in the target pointer array, blocks.
//no arguments, returns 0 on success, 1 if found a NULL pointer in blocks
int free_blocks(){
	for(int i = 0; i<num_blocks;i++){
		target * block = blocks[i];
		if(block == NULL){
			return 1;
		}
		for (int i = 0; i < block->dep_count;i++){
			free(block->depend[i]);
		}for (int i = 0; i < block->recipe_count;i++){
			free(block->recipe[i]);
		}
		free(block->name);
		free(block);
	}
	return 0;
}

//executes the recipes in the char pointer array, recipies
//no arguments, returns 0 on success, 1 if fork error
int execute_recipes(){
	for (int i = 0; i < num_recipies; i++){
		//make a copy of the recipe to tokenize
		char * str =  malloc(sizeof(char) * (strlen(recipies[i])));
		strcpy(str,recipies[i]);
		char * token  = strtok(str, " ");
		//make an array to hold args
		char * args[MAX_PARM] = {};
		int j = 0;
		while(token != NULL){
			args[j] = token;
			//get the next token (argument)
			token = strtok(NULL, " ");
			j++;
		}
		//create a child process
		pid_t pid = fork();
		if(pid == -1){
			//error
			return 1;
		}else if (pid == 0){
			//child, execute the arguments in args
			execvp(args[0],args);
		}else{
			//parent, wait for child to finish
			wait(NULL);
		}
		free(str);
	}
	return 0;
}


/*
int execute_recipes(){
	for (int i = 0; i < num_recipies; i++){
		//make a copy of the line
		char * str =  malloc(sizeof(char) * (strlen(recipies[i])));
		strcpy(str,recipies[i]);

		//split on comas
		//then tokenize each command

			//point to the first token
			char * token  = strtok(str, " ");
			//make an array to hold tokens
			char * tokens[MAX_PARM] = {};
			int j = 0;
			while(token != NULL){
				tokens[j] = token;
				//get the next token
				token = strtok(NULL, " ");
				j++;
			}
		
		num_parallel = 0;
		
		pid_t pid = fork();			
		if(pid == -1){
			//error
		}else if (pid == 0){
			
			//child, execute the arguments in tokens
			for(int j = 0; j<num_parallel; j++){
				pid_t pid2 = fork();
				if(pid == -1){
					//error
				}else if (pid == 0){
					//child
				}else{
					//parent
					
				}
			}
			execvp(tokens[0],tokens);
			
		}else{
			//parent, wait for child to finish
			//wait for all children to finish
			wait(NULL);
		}


	/*
		//create a child process
		//for each parallel command
		pid_t pid = fork();
		if(pid == -1){
			//error
		}else if (pid == 0){
			//child, execute the arguments in tokens
			execvp(tokens[0],tokens);
		}else{
			//parent, wait for child to finish
			//wait for all children to finish
			wait(NULL);
		}
	

		//free str
		free(str);

	}
	return 0;
}
*/


//populates the target pointer array, blocks, from the char pointer array, lines
//no arguments, no return value
void parse_lines(){
	//counts the number of dependencies for a target
	int c = 0;
	//counts the number of recipies for a target
	int r = 0;
	//counts the current block number
	int e = 0;
	//tracks whether we've found a target yet
	char found = 0;
	//current target to add recipes, dependencies, etc. to
	target * cur_target;

	char * ptr;
	for (int i = 0; i < MAX_LINES; i++) {
		char line[LINE_SIZE];
		//copy current line to tokenize
		strcpy(line,lines[i]);
		if(strchr(lines[i],':')!=NULL){
			//we've hit a new block, get the target
			ptr = strtok(line, ":");
			if(ptr != NULL){
				if(found){
					//save the last block to blocks
					cur_target->dep_count = c;
					cur_target->recipe_count = r;
					cur_target->visited = 0;
					blocks[e] = cur_target;
					e++;
				}else{found=1;}
				//starting a new block
				cur_target = malloc(sizeof(target));
				cur_target->name = malloc(sizeof(char) * (strlen(ptr)));
				strcpy(cur_target->name,ptr);		
				ptr = strtok(NULL, " \n");
			}
			c = 0;
			while(ptr!=NULL){
				//we've found a new dependency for the current block
				cur_target->depend[c] = malloc(sizeof(char) * (strlen(ptr)));
				strcpy(cur_target->depend[c],ptr);
				ptr = strtok(NULL, " \n");
				c++;
			}
			r=0;
		}else if(strchr(lines[i],'\t')!=NULL){
			//we've found a new recipe for the current block
			ptr = strtok(line, "\t\n");
			cur_target->recipe[r] = malloc(sizeof(char) * (strlen(ptr)));
			strcpy(cur_target->recipe[r],ptr);
			r++;
		}
    	}
	//gone through all lines, add curent block to blocks
	cur_target->dep_count = c;
	cur_target->recipe_count = r;
	cur_target->visited = 0;
	blocks[e] = cur_target;
	num_blocks = e+1;
	return;
}

//prints the current blocks to standard output
//no arguments, no return value
void print_blocks(){
	for (int i = 0; i<num_blocks;i++){
		printf("target '%s' has %d dependencies and %d recipes\n",blocks[i]->name,blocks[i]->dep_count,blocks[i]->recipe_count);
		for (int j = 0; j<blocks[i]->dep_count;j++){	
			printf("Dependency %d is %s\n",j,blocks[i]->depend[j]);
		}
		for (int j = 0; j<blocks[i]->recipe_count;j++){	
			printf("Recipe %d is %s\n",j,blocks[i]->recipe[j]);
		}
	}
	return;
}


//Parse the input makefile to determine targets, dependencies, and recipes
int process_file(char *fname)
{
	FILE* fp = fopen(fname, "r");
	char line[LINE_SIZE];
	int i = 0;

	if (!fp) {
		printf("Failed to open the file: %s \n", fname);
		return -1;
	}

	//Read the contents and store in lines
	while (fgets(line, LINE_SIZE, fp)) 
		strncpy(lines[i++], line, strlen(line));
		
	fclose(fp);

	return 0;
}

//Validate the input arguments, bullet proof the program
int main(int argc, char *argv[]) 
{
	process_file(argv[1]);
	parse_lines();
	print_blocks();
	
	//recipies = top_sort();
	//
	//execute_recipes();

	free_blocks();
	
	exit(EXIT_SUCCESS);
}
