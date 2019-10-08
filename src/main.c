#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "graph.h"
#include "dfs_stack.h"

int num_blocks;
target *blocks[MAX_TARGETS];

int num_recipies;
char * recipies[MAX_RECIPES_PT*MAX_TARGETS];


int free_block(target * block){
	if(block == NULL){
		return 0;
	}
	for (int i = 0; i < block->dep_count;i++){
		free(block->depend[i]);
	}for (int i = 0; i < block->recipe_count;i++){
		free(block->recipe[i]);
	}
	free(block->name);
	free(block);
	return 0;
}


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
	*/

		//free str
		free(str);

	}
	return 0;
}

int parse_lines(){
	//points to tokens in lines
	char * ptr;
	//counts the number of dependencies for a target
	int c = 0;
	//counts the number of recipies for a target
	int r = 0;
	//counts the current block number
	int e = 0;

	//tracks whether we've found a target yet
	char found = 0;

	target * cur_target;
	for (int i = 0; i < MAX_LINES; i++) {
		
		char line[LINE_SIZE];
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
				//add dependency
				cur_target->depend[c] = malloc(sizeof(char) * (strlen(ptr)));
				strcpy(cur_target->depend[c],ptr);
				//printf("depend[%d]: %s\n",c, ptr);
				ptr = strtok(NULL, " \n");
				c++;
			}
			//reset recipe counter to 0
			r=0;
		}else if(strchr(lines[i],'\t')!=NULL){
			ptr = strtok(line, "\t\n");
			//add recipe
			cur_target->recipe[r] = malloc(sizeof(char) * (strlen(ptr)));
			strcpy(cur_target->recipe[r],ptr);
			//printf("recipe[%d]: %s\n",r, ptr);
			r++;
		}
    	}

	//gone through all lines, add curent block to blocks
	cur_target->dep_count = c;
	cur_target->recipe_count = r;
	cur_target->visited = 0;
	blocks[e] = cur_target;
	num_blocks = e+1;

	//return success
	return 0;
}
int print_blocks(){
	
	for (int i = 0; i<num_blocks;i++){
		//printf("Block %d:\n",i);
		printf("target '%s' has %d dependencies and %d recipes\n",blocks[i]->name,blocks[i]->dep_count,blocks[i]->recipe_count);
		for (int j = 0; j<blocks[i]->dep_count;j++){	
			printf("Dependency %d is %s\n",j,blocks[i]->depend[j]);
		}
		for (int j = 0; j<blocks[i]->recipe_count;j++){	
			printf("Recipe %d is %s\n",j,blocks[i]->recipe[j]);
		}
	}
	return 0;
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

	for (int i = 0; i< num_blocks;i++){
		free_block(blocks[i]);
	}
	
	exit(EXIT_SUCCESS);
}
