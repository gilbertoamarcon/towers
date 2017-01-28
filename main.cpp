#include <stdlib.h>
#include <list>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "state.hpp"

#define NMAX	10000

// Clear list of states from memory
void clear_list(list<State*> *lis);

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

// Heuristic distance from node to goal
int heuristic(State *node, State *goal);

// Insert child to open list if correct conditions met
void new_child(State *child, list<State*> *open, list<State*> *closed, State *goal);

// Search for a plan
int search(State *start, State *goal, stack<State> *plan);

int main(int argc, char **argv){

	State *start		= new State("01234,_,_");
	State *goal			= new State("43210,_,_");
	stack<State> *plan	= new stack<State>;

	printf("Start: ");
	display(start);
	printf("\n");

	printf("Goal: ");
	display(goal);
	printf("\n");

	int num_exp_nodes = search(start, goal, plan);
	if(num_exp_nodes > 0){
		printf("Plan found.\n");
		printf("%d expanded nodes.\n",num_exp_nodes);
		printf("%d actions.\n",plan->size());
		printf("Plan: \n");
		print_plan(*plan);
		printf("\n");
	}
	else
		printf("Plan failed.\n");

	return 0;

}

// Clear list
void clear_list(list<State*> *lis){
	while(!lis->empty()){
		delete lis->front();
		lis->pop_front();
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

// Heuristic distance from node to goal
int heuristic(State *node, State *goal){
	stack<int> node_stack	= node->A;
	stack<int> goal_stack	= goal->A;
	int counter = goal_stack.size() - node_stack.size();
	while(goal_stack.size() != node_stack.size())
		goal_stack.pop();
	int aux = 0;
	while(!goal_stack.empty()){
		if(node_stack.top() != goal_stack.top())
			counter+=2;
		node_stack.pop();
		goal_stack.pop();
	}
	return counter;
}

// Insert child to open list if correct conditions met
void new_child(State *child, list<State*> *open, list<State*> *closed, State *goal){

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
	int h = heuristic(child,goal);

	// Computing the estimated path cost
	child->f = child->g + h;

	// Inserting child into the sorted open list
	list<State*>::iterator it = open->begin();
	while(it != open->end())
		if((*(it++))->f > child->f) break;
	open->insert(it,child);

}

int search(State *start, State *goal, stack<State> *plan){

	stack<State*> children;
	list<State*> open;
	list<State*> closed;

	State *state = NULL;

	// Initializing open list
	open.push_back(start);
	
	// Search loop
	int num_exp_nodes = 0;
	for(;;){

		// Checking if open empty (failure)
		if(open.empty()) return -1;

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
			new_child(children.top(),&open,&closed,goal);
			children.pop();
		}
		num_exp_nodes++;
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
	clear_list(&open);
	clear_list(&closed);

	return num_exp_nodes;

}
