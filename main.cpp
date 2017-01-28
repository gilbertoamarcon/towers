#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <stack>
#include <list>
#include <string>
#include <iostream>

using namespace std;

#define NMAX	10000

// State class definition
class State{

	public:

		State *parent;

		float g;

		stack<int> A;
		stack<int> B;
		stack<int> C;

		// Constructor
		State(State *parent){
			*this = *parent;
			this->parent = parent;
			this->g++;
		}

		State(char *in_str, float g, State *parent){
			string str = in_str;
			this->parent = parent;
			this->g = g;
			char stack_counter = 'C';
			for(int i = str.size()-1; i >=0; i--){
				if(str[i] == ','){
					stack_counter--;
					continue;
				}
				if(str[i] != '_'){
					switch(stack_counter){
						case 'A':
							A.push(str[i]-48);
							break;
						case 'B':
							B.push(str[i]-48);
							break;
						case 'C':
							C.push(str[i]-48);
							break;
					}
				}
			}
		}

		// Return stack with children states
		void expand(stack<State*> *children){
			if(!A.empty()){
				State *ab = new State(this);
				State *ac = new State(this);
				children->push(ab);
				children->push(ac);
				ab->A.pop();
				ac->A.pop();
				ab->B.push(A.top());
				ac->C.push(A.top());
			}
			if(!B.empty()){
				State *ba = new State(this);
				State *bc = new State(this);
				children->push(ba);
				children->push(bc);
				ba->B.pop();
				bc->B.pop();
				ba->A.push(B.top());
				bc->C.push(B.top());
			}
			if(!C.empty()){
				State *ca = new State(this);
				State *cb = new State(this);
				children->push(ca);
				children->push(cb);
				ca->C.pop();
				cb->C.pop();
				ca->A.push(C.top());
				cb->B.push(C.top());
			}
		}

};

// Clear list
void clear_list(list<State*> *lis);

// Print plan
void print_plan(stack<State> plan);

// Print stack
void print_stack(stack<int> st);

// State display
void display(State *state);

// Compare two stacks
bool compare_stack(stack<int> sta, stack<int> stb);

// Compare two states
bool compare(State *sta, State *stb);

bool search(State *start, State *goal, stack<State> *plan);

int main(int argc, char **argv){

	State *start		= new State("301,_,42",0,NULL);
	State *goal			= new State("43210,_,_",0,NULL);
	stack<State> *plan	= new stack<State>;

	printf("Start: ");
	display(start);
	printf("\n");

	printf("Goal: ");
	display(goal);
	printf("\n");

	if(search(start, goal, plan)){
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
	printf("(");
	print_stack(state->A);
	printf(",");
	print_stack(state->B);
	printf(",");
	print_stack(state->C);
	printf(")\n");
}

// Compare two stacks
bool compare_stack(stack<int> sta, stack<int> stb){
	if(sta.size() != stb.size()) return false;
	while(!sta.empty()){
		if(sta.top() != stb.top()) return false;
		sta.pop();
		stb.pop();
	}
	return true;
}

// Compare two states
bool compare(State *sta, State *stb){
	if(!compare_stack(sta->A, stb->A)) return false;
	if(!compare_stack(sta->B, stb->B)) return false;
	if(!compare_stack(sta->C, stb->C)) return false;
	return true;
}

// Compare two states
void new_child(State *child, list<State*> *open, list<State*> *closed){
	for(State* node : *closed)
		if(compare(child,node)){
			delete child;
			return;
		}
	for(State* node : *open)
		if(compare(child,node)){
			if(child->g >= node->g){
				delete child;
				return;
			}else{
				open->remove(node);
				break;
			}
		}
	open->push_back(child);
}

bool search(State *start, State *goal, stack<State> *plan){

	stack<State*> children;
	list<State*> open;
	list<State*> closed;

	State *state = NULL;

	// Initializing open list
	open.push_back(start);
	
	// Search loop
	for(;;){

		// Checking if open empty (failure)
		if(open.empty()) return false;

		// Visiting current node (least cost)
		state = open.front();
		open.pop_front();

		// Adding current node to the closed list
		closed.push_back(state);

		// Checking if goal found
		if(compare(state,goal)) break;

		// Expanding current node
		state->expand(&children);

		while(!children.empty()){
			new_child(children.top(),&open,&closed);
			children.pop();
		}
	}

	// Final path position
	plan->push(*state);

	// Defining path as a sequence of positions
	for(;;){

		// Check if path completed
		if(compare(state,start)) break;

		// Moving to parent node
		state = state->parent;
		
		// Inserting position in the path vector
		plan->push(*state);
	}

	// Clearing memory
	clear_list(&open);
	clear_list(&closed);

	return true;

}
