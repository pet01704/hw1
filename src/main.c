#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "dfs_stack.h"
int num_blocks;
target *blocks[MAX_TARGETS];

int parse_lines(){
	char * ptr;
	

	//int n_array = (sizeof (lines) / sizeof (const char *));
	int c = 0;
	int r = 0;
	int e = -1;
	target * cur_target = malloc(sizeof(target));
	target * cur_depend = malloc(sizeof(char *)*MAX_DEP);
	target * cur_recipe = malloc(sizeof(char *)*MAX_RECIPES_PT);
	for (int i = 0; i < MAX_LINES; i++) {
		char line[200];
		strcpy(line,lines[i]);
		if(strchr(lines[i],':')!=NULL){
			//this line introduces a new target
			ptr = strtok(line, ":");
			if(ptr != NULL){
				
				if(e>=0){
					//target * cpy = cur_target;
					cur_target->dep_count = c;
					cur_target->recipe_count = r;
					cur_target->visited = 0;
					blocks[e] = cur_target;
					cur_target = malloc(sizeof(target));
					cur_depend = malloc(sizeof(char *)*MAX_DEP);
					cur_recipe = malloc(sizeof(char *)*MAX_RECIPES_PT);
				}
				//pointer is the target
				//char name[100];
				cur_target->name = malloc(sizeof(char) * 100);
				strcpy(cur_target->name,ptr);
				e++;
				//printf("name: %s\n",ptr);		
				ptr = strtok(NULL, " \n");
			}
			c = 0;
			while(ptr!=NULL){
				//add dependency
				cur_target->depend[c] = malloc(sizeof(char) * 100);
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
			cur_target->recipe[r] = malloc(sizeof(char) * 100);
			strcpy(cur_target->recipe[r],ptr);
			//printf("recipe[%d]: %s\n",r, ptr);
			r++;
		}
    	}
	cur_target->dep_count = c;
	cur_target->recipe_count = r;
	cur_target->visited = 0;
	blocks[e] = cur_target;
	num_blocks = e+1;
	return 0;
}
int print_blocks(){
	
	for (int i = 0; i<num_blocks;i++){
		printf("Block %d:\n",i);
		printf("name: %s\n",blocks[i]->name);
		printf("dep_count: %d\n",blocks[i]->dep_count);
		for (int j = 0; j<blocks[i]->dep_count;j++){	
			printf("depend[%d]%s\n",j,blocks[i]->depend[j]);
		}
		printf("recipe_count: %d\n",blocks[i]->recipe_count);
		for (int j = 0; j<blocks[i]->recipe_count;j++){	
			printf("recipe[%d]%s\n",j,blocks[i]->recipe[j]);
		}
	}
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

	//$./mymake Makefile
	//Similarly account for -r flag
	
	/*
	if (argc == 2 && strncmp(argv[1], "-p", 2)) {
		process_file(argv[1]); 
		//print the target dependencies and recipies in the makefile

	}else if (argc == 2 && strncmp(argv[1], "-r", 2)) {
		process_file(argv[1]); 
		//print the recipe order
	}

	if (argc == 3) {
		//$./mymake Makefile target
		if (strncmp(argv[1], "-p", 2) && !process_file(argv[1])) {
			//TODO
			
		} else if (!strncmp(argv[1], "-p", 2)) {
			//$./mymake -p Makefile 
			if (!process_file(argv[2])) {
				//TODO
			}
		}
	}
	*/
	exit(EXIT_SUCCESS);
}
