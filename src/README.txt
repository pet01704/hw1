• Team names and x500
	Tseki Lhamo, lhamo012
	Daniel Peterson, pet01704

• The purpose of your program
Depending on the flags provided, the program will do one of the following things:
- Print the number of dependencies and recipes a target has (follows the Makefile’s order) 
- If a target name is provided, print only the number of dependencies and recipes beginning from that target
- Print only the recipes as a list (topologically sorted order) 

• How to compile the program
gcc -o mymake main.cc
	./mymake Makefile
	./mymake Makefile target
	./mymake -p Makefile
	./mymake -r Makefile

• What exactly your program does
The program prints the recipes, forks and executes in a topologically sorted order. 
The program uses a topological sort algorithm which adds a target_block to a array once all of its dependencies have been explored. The algorithm maintains a visited array that accounts for all the targets that have already been visited so that they are not explored more than once. 

Simultaneously, a recipe array is populated with recipes that will be printed with -r flag. 

The sorting provides the order of fork() and exec() so that every time a new recipe is encountered, the program forks a new process. 

• Any assumptions outside this document



• Your and your partners individual contributions
Tseki: 
    1. reading command line arguments for correct functionality (flags, targets etc)
    2. topological_sort(), top_sort(), find_dep_block() , contains() methods for sorting
    3. sort_recipes() and print_recipe_array()  method for -r flag 
Daniel:
    1. parse_lines() and print_blocks() method for printing dependencies and recipes
    2. execute_recipes() method for forking and executing recipes
    3. free_blocks() method for freeing blocks array
       
• If you have attempted extra credit
