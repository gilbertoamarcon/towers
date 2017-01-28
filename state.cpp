#include "state.hpp"

// Constructor from parent
State::State(State *parent,char *action){

	// Copy from parent
	*this = *parent;

	// Pointer to parent
	this->parent = parent;

	// Action from parent
	this->action = action;

	// Cost to reach from goal
	this->g++;
}

// Constructor from start/goal
State::State(char *in_str){
	string str = in_str;
	this->parent = NULL;
	this->action = "_->_";
	this->g = 0;
	this->f = 0;
	char stack_counter = 'C';

	// Parsing state descriptor string
	for(int i = str.size()-1; i >=0; i--){
		if(str[i] == ','){
			stack_counter--;
			continue;
		}
		if(str[i] != '_' && str[i] != '-'){
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
void State::expand(stack<State*> *children){

	// Popping from A
	if(!A.empty()){
		State *ab = new State(this,"A->B");
		State *ac = new State(this,"A->C");
		children->push(ab);
		children->push(ac);
		ab->A.pop();
		ac->A.pop();
		ab->B.push(A.top());
		ac->C.push(A.top());
	}

	// Popping from B
	if(!B.empty()){
		State *ba = new State(this,"B->A");
		State *bc = new State(this,"B->C");
		children->push(ba);
		children->push(bc);
		ba->B.pop();
		bc->B.pop();
		ba->A.push(B.top());
		bc->C.push(B.top());
	}

	// Popping from C
	if(!C.empty()){
		State *ca = new State(this,"C->A");
		State *cb = new State(this,"C->B");
		children->push(ca);
		children->push(cb);
		ca->C.pop();
		cb->C.pop();
		ca->A.push(C.top());
		cb->B.push(C.top());
	}
}
