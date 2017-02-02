#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include "state.hpp"
#include <string.h>
#include <ctime>

#define NMAX		10000
#define BSIZE		256
#define VERBOSE		1
#define INI_PS		7 //Fix 4
#define END_PS		7 //Fix 7
#define OUT_FILE	"out.csv"

// State class definition
struct Performance{
	int heuristic;
	int beamsize;
	int problem_size;
	int problem_number;
	int num_exp_nodes;
	int num_actions;
	double planning_time;
};

// Beam sizes to test
int beamsizes[] = {5, 10, 15, 20, 25, 50, 100, NMAX};

// Loading problems
int load_problems(list<State*> *start_list, int min_size, int max_size);

// Store performance results
void store_performance(char *filename,stack<Performance> perf);

// Clear list of states from memory
void clear_list(list<State*> *state_list,State* start, State* goal);

// Print plan
void print_plan(stack<State> plan);

// Print stack
void print_stack(stack<int> st);

// State display
void display(State *state);

// Compare two stacks
bool stack_equal(stack<int> sta, stack<int> stb);

// Compare two states
bool state_equal(State *sta, State *stb);

// Heuristic0
int heuristic0(State *node, State *goal);

// Heuristic1
int heuristic1(State *node, State *goal);

// Insert child to open list if correct conditions met
void new_child(State *child, list<State*> *open, list<State*> *closed, State *goal, int beamsize, bool heuristic);

// Search for a plan
int search(State *start, State *goal, stack<State> *plan, int beamsize, bool heuristic);

int main(int argc, char **argv){

	stack<Performance> performance_stack;

	// For each heuristic
	for(int h = 1; h >= 1; h--){//h >= 0 FIX

		// For each beam size to test
		for(int b = 7; b < 8; b++){ //b =0 FIX

			list<State*> start_list;

			// Loading problem list
			if(load_problems(&start_list,INI_PS,END_PS)){
				printf("Error loading file.\n");
				return 1;
			}
			printf("%d problems loaded.\n",start_list.size());

			// Solving each problem
			int problem_counter = -1;
			while(!start_list.empty()){
				problem_counter++;

				// Loading starting node
				State *start = start_list.front();
				start_list.pop_front();

				// Generating goal node
				char goalBuffer[BSIZE] = "";
				char auxBuffer[BSIZE] = "";
				for(int i = 0; i < start->A.size(); i++){
		  			sprintf(auxBuffer,"%d",start->A.size()-i-1);
		  			strcat(goalBuffer,auxBuffer);
				}
				strcat(goalBuffer,",_,_");
				State *goal = new State(goalBuffer);

				// Running and taking execution time
				stack<State> plan;
		  		clock_t t_start = clock();
				int num_exp_nodes = search(start, goal, &plan,beamsizes[b],h);
				double planning_time = (double)(clock() - t_start)/(double)CLOCKS_PER_SEC;

				// Preparing results to store to file
				Performance perf;
				perf.heuristic		= h;
				perf.beamsize		= beamsizes[b];
				perf.problem_size	= (problem_counter - problem_counter%20)/20 + INI_PS;
				perf.problem_number	= problem_counter%20;
				perf.num_exp_nodes	= num_exp_nodes;
				perf.num_actions	= plan.size();
				perf.planning_time	= planning_time;
				performance_stack.push(perf);

				// Presenting results on screen
				if(VERBOSE){
					if(num_exp_nodes > 0){
						printf("Problem %d.\n",problem_counter);
						printf("Start: ");
						display(start);
						printf("Goal: ");
						display(goal);
						printf("Plan found.\n");
						printf("%d expanded nodes.\n",num_exp_nodes);
						printf("%d actions.\n",plan.size());
						printf("%f seconds.\n",planning_time);
						printf("Plan: \n");
						print_plan(plan);
						printf("\n");
					}
					else
						printf("Plan failed.\n");
				}else
					printf("Heuristic %d, beam size %d, problem size %d, problem number %3d, planning time %.6f s.\n",perf.heuristic, perf.beamsize, perf.problem_size, perf.problem_number, perf.planning_time);

			}

		}
	}

	// Storing performance results to file
	store_performance(OUT_FILE,performance_stack);

	return 0;

}

int load_problems(list<State*> *start_list, int min_size, int max_size){

	char nameBuffer[BSIZE] = "";
	char fileBuffer[BSIZE] = "";

	for(int i = min_size; i <= max_size; i++){
		sprintf(nameBuffer,"problems/perms-%d.txt",i);
		FILE *file = NULL;
		file = fopen(nameBuffer,"r");
		if(file == NULL || fgets(fileBuffer,BSIZE,file) == NULL){
			printf("Error: Origin file '%s' not found.",nameBuffer);
			return 1;
		}
		while(fgets(fileBuffer,BSIZE,file) != NULL && fileBuffer[0] != '\n'){
			int j = -1;
			while(fileBuffer[++j] != '\n');
			fileBuffer[j] = '\0';
			sprintf(fileBuffer,"%s,_,_",fileBuffer);
			State *start = new State(fileBuffer);
			start_list->push_back(start);
		}
		fclose(file);
	}
	return 0;
}

// Store performance results
void store_performance(char *filename,stack<Performance> perf){
	FILE *file = NULL;
	file = fopen(filename,"w");
	while(!perf.empty()){
		fprintf(file,"%d,",perf.top().heuristic);
		fprintf(file,"%d,",perf.top().beamsize);
		fprintf(file,"%d,",perf.top().problem_size);
		fprintf(file,"%d,",perf.top().problem_number);
		fprintf(file,"%d,",perf.top().num_exp_nodes);
		fprintf(file,"%d,",perf.top().num_actions);
		fprintf(file,"%.6f\n",perf.top().planning_time);
		perf.pop();
	}
	fclose(file);
}

// Clear list
void clear_list(list<State*> *state_list,State* start, State* goal){
	while(!state_list->empty()){
		if(state_list->front() != start && state_list->front() != goal)
			delete state_list->front();
		state_list->pop_front();
	}
}

// Print plan
void print_plan(stack<State> plan){
	int i = 0;
	while(!plan.empty()){
		printf("%3d: ",i++);
		display(&plan.top());
		plan.pop();
	}
}

// Print stack
void print_stack(stack<int> st){
	if(st.empty()) printf("_");
	while(!st.empty()){
		printf("%d",st.top());
		st.pop();
	}
}

// State display
void display(State *state){
	if(state->action[0] != '_')
		printf(" %s ",state->action);
	printf("(");
	print_stack(state->A);
	printf(",");
	print_stack(state->B);
	printf(",");
	print_stack(state->C);
	printf(")\n");
}

// Check if two stacks are equal
bool stack_equal(stack<int> sta, stack<int> stb){
	if(sta.size() != stb.size()) return false;
	while(!sta.empty()){
		if(sta.top() != stb.top()) return false;
		sta.pop();
		stb.pop();
	}
	return true;
}

// Compare two states
bool state_equal(State *sta, State *stb){
	if(!stack_equal(sta->A, stb->A)) return false;
	if(!stack_equal(sta->B, stb->B)) return false;
	if(!stack_equal(sta->C, stb->C)) return false;
	return true;
}

// Heuristic 0
int heuristic0(State *node, State *goal){
	stack<int> node_stack	= node->A;
	stack<int> goal_stack	= goal->A;
	int counter = goal_stack.size() - node_stack.size();
	return counter;
}

// Heuristic 1
int heuristic1(State *node, State *goal){
	stack<int> node_stack	= node->A;
	stack<int> goal_stack	= goal->A;
	int counter = goal_stack.size() - node_stack.size();
	while(goal_stack.size() != node_stack.size())
		goal_stack.pop();
	int aux = 0;
	while(!goal_stack.empty()){
		counter += 2*abs(node_stack.top() - goal_stack.top());
		node_stack.pop();
		goal_stack.pop();
	}
	return counter;
}


int heuristic2(State *node, State *goal) {
  stack<int> node_stack	= node->A;
	stack<int> goal_stack	= goal->A;
	int goal_size = goal_stack.size();
	int node_size = node_stack.size();

	//count number of rings in place
	while(goal_stack.size() != node_stack.size()){
    goal_stack.pop();
  }
  int out_of_place = 0;
  int pos = 0;
  while (!goal_stack.empty())  {
    pos ++;
    if (node_stack.top() != goal_stack.top()) {
      out_of_place = pos;
    }
    node_stack.pop();
    goal_stack.pop();
  }
  int in_place = node_size - out_of_place;

  //count number of rings in ideal location
  stack<int> node_stack_B	= node->B;
  stack<int> node_stack_C	= node->C;
  stack<int> node_stack_B_copy;
  stack<int> node_stack_C_copy;
  int ideal = 0;
  int ring;

  while (!node_stack_B.empty()) {
    ring = node_stack_B.top();
    node_stack_B.pop();
    node_stack_B_copy	= node_stack_B;

    ideal ++;
    while (!node_stack_B_copy.empty()) {
      if (node_stack_B_copy.top() < ring) {
        ideal --;
        break;
      }
      node_stack_B_copy.pop();
    }
  }

  while (!node_stack_C.empty()) {
    ring = node_stack_C.top();
    node_stack_C.pop();
    node_stack_C_copy	= node_stack_C;

    ideal ++;
    while (!node_stack_C_copy.empty()) {
      if (node_stack_C_copy.top() < ring) {
        ideal --;
        break;
      }
      node_stack_C_copy.pop();
    }
  }

  int not_ideal = goal_size - ideal - in_place;
  return ideal + 2 * not_ideal;

}




// Insert child to open list if correct conditions met
void new_child(State *child, list<State*> *open, list<State*> *closed, State *goal, int beamsize, bool heuristic){

	// Check if in the closed list
	for(State* node : *closed)
		if(state_equal(child,node)){
			delete child;
			return;
		}

	// Check if in the open list
	for(State* node : *open)
		if(state_equal(child,node)){
			if(child->g >= node->g){
				delete child;
				return;
			}else{
				open->remove(node);
				break;
			}
		}

	// Computing the heuristic
	int h = (heuristic)?heuristic2(child,goal):heuristic0(child,goal); //FIX

	// Computing the estimated path cost
	child->f = child->g + h;

	// Inserting child into the sorted open list
	list<State*>::iterator it = open->begin();
	while(it != open->end())
		if((*(it++))->f > child->f) break;
	open->insert(it,child);

	// Beam search size limit to open list
	if(open->size() > beamsize)
		open->pop_back();

}

int search(State *start, State *goal, stack<State> *plan, int beamsize, bool heuristic){

	stack<State*> children;
	list<State*> open;
	list<State*> closed;

	State *state = NULL;

	// Initializing open list
	open.push_back(start);

	// Search loop
	int num_exp_nodes = 0;
	for(;;){

		// Checking if failure
		if(num_exp_nodes++ == NMAX || open.empty()) return -1;

		// Visiting current node (least cost)
		state = open.front();
		open.pop_front();

		// Adding current node to the closed list
		closed.push_back(state);

		// Checking if goal found
		if(state_equal(state,goal)) break;

		// Expanding current node
		state->expand(&children);

		while(!children.empty()){
			new_child(children.top(),&open,&closed,goal,beamsize,heuristic);
			children.pop();
		}
	}

	// Final path position
	plan->push(*state);

	// Defining path as a sequence of positions
	for(;;){

		// Check if path completed
		if(state_equal(state,start)) break;

		// Moving to parent node
		state = state->parent;

		// Inserting position in the path vector
		plan->push(*state);
	}

	// Clearing memory
	clear_list(&open,start,goal);
	clear_list(&closed,start,goal);

	return num_exp_nodes;

}
