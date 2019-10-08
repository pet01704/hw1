#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "graph.h"
/**
 *test machine: CSELAB_machine_name
 * date: 10/09/19
 * name: Daniel Peterson, Tseki Lhamo
 * x500: pet01704, lhamo012
 */

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

/**
 * Checks if a given target array contains a given target block
 *
 * @param array target array that is searched
 * @param block target that is being searched for
 * @return i index of block or -1 if block is not contained in array
 */
int contains(target *array[], target *block) {
	for(int i=0; i<num_blocks; i++) {
		if(array[i] == block) {
			return i;
		}
	}
	return -1;
}

/**
 * Searches for dependency in blocks array (array of targets)
 *
 * @param dep dependency that is being searched for
 * @return result target that was either found in blocks array or was not a listed target
 */
target * find_dep_block(char *dep) {
	target * result = malloc(sizeof(target));
	for (int i=0;i<num_blocks;i++) {
		if(strncmp(blocks[i]->name, dep, strlen(dep)) == 0 ) {
			result = blocks[i];
			return result;
		}
	}
	result->name = dep;
	result->dep_count = 0;
	result->recipe_count = 0;
	return result;
}

/**
 * Prints the sorted recipe array
 */
void print_recipe_array() {
	// adding recipe to  array
	int j = 0;
	while(sorted_recipies[j]!=NULL) {
		printf("recipe: %s\n", sorted_recipies[j]);
		j++;
	}
}

/**
 * Traverses sorted array and populates recipes into soreted_recipies array
 *
 * @param sorted arrat that is used to get ordered recieps
 */
void sort_recipes(target *sorted[]) {
		int k = 0;
		while(k<num_blocks*2) {
			if(sorted[k]!=NULL) {
				for(int i=0; i<sorted[k]->recipe_count;i++) {
					// only print the recipes of targets with recipes
					if(sorted[k]->recipe[i] == NULL) {
						break;
					} else {
						total_num_recipies++;
						// adding recipe to sorted_recipe array
						int j = 0;
						while(j < sizeof(sorted_recipies)) {
							if(sorted_recipies[j]==NULL){
								sorted_recipies[j]=sorted[k]->recipe[i];
								break;
							}
							j++;
						}
					}
				}
			}
			k++;
		}
}

/**
 * Recursive top_sort function that sorts blocks into a sorted array
 *
 * @param block target that is being explored
 * @param sorted array that contains sorted targets
 * @param visited array that maintains visted targets
 */
void top_sort(target *block, target *sorted[], target *visited[]) {
	int i = 0;
	// add block to visited
	while(i<num_blocks*2) {
		if(visited[i]==NULL){
			visited[i]=block;
			break;
		}
		i++;
	}
	// explore the dependencies
	for(int i=0; i<block->dep_count; i++) {
		target * child_block = malloc(sizeof(target));
		child_block = find_dep_block(block->depend[i]);

		if(contains(visited, child_block)!=-1) {
			continue;
		}

		top_sort(child_block, sorted, visited);
		// adding block to sorted array
		int j = 0;
		while(j<num_blocks*2) {
			if(sorted[j]==NULL){
				sorted[j]=child_block;
				break;
			}
			j++;
		}
	}
}

/**
 * Calls top_sort on very block
 *
 * @param start the starting index for top sort to begin at
 */
void topological_sort(int start) {
	// array for visited nodes
	target *visited[sizeof(blocks)*num_blocks*2];
	// array for sorted nodes or fully explored nodes
	target *sorted[sizeof(blocks)*num_blocks*2];

	for(int i=start; i<num_blocks; i++) {
		if(contains(visited, blocks[i])!=-1) {
			continue;
		}
		top_sort(blocks[start], sorted, visited);
	}
	// add the first target to sorted
	sorted[num_blocks*2-1] = blocks[start];
	sort_recipes(sorted);
}

//executes the recipes in the char pointer array, recipies
//no arguments, returns 0 on success, 1 if fork error
int execute_recipes(){
	for (int i = 0; i < total_num_recipies; i++){
		//make a copy of the recipe to tokenize
		char * str =  malloc(sizeof(char) * (strlen(sorted_recipies[i])));
		strcpy(str,sorted_recipies[i]);
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

//Validate the input arguments, bullet proof the program
int main(int argc, char *argv[]) {
	if (argc>=2 && argc<=3 && strncmp(argv[1], "-p", 2) && strncmp(argv[1], "-r", 2) && !process_file(argv[1])) {
		parse_lines();
		if (argc == 2) { //$./mymake Makefile
			// run topological sort starting at the first target
			topological_sort(0);
		} else if (argc == 3) {   // $./mymake Makefile target
			// $./mymake Makefile -p/-r
			if(!strncmp(argv[2], "-p", 2) || !strncmp(argv[2], "-r", 2)) {
				printf("Incorrect arg order");
				exit(EXIT_SUCCESS);
			}
			target *target_block = find_dep_block(argv[2]);
			// check if target has dependencies or recipes
			if (target_block->dep_count == 0 && target_block->recipe_count == 0) {
				printf("Target not found");
				exit(EXIT_SUCCESS);
			} else {
				int index = contains(blocks, target_block);
				topological_sort(index);
			}
		}
		execute_recipes();
	}
	else if (argc == 3 && !process_file(argv[2])) { 	//$./mymake -p Makefile
		parse_lines();
		if(!strncmp(argv[1], "-p", 2)){
			// print the target, dependencies and recipes in the makefile
			print_blocks();
		} else if (!strncmp(argv[1], "-r", 2)) {  	//$./mymake -r Makefile
			// print recipe order of target
			topological_sort(0);
			print_recipe_array();
		}
	}else if (argc>3){
		printf("Too many arguments");
	}else if (argc<2){
		printf("Not enough arguments");
	}else{
		//file not found etc.
	}
	// Freeing blocks array
	free_blocks();

	exit(EXIT_SUCCESS);
}

