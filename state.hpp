#ifndef STATE_H
#define STATE_H

#include <stack>
#include <string>

using namespace std;

// State class definition
class State{

	public:

		// Pointer to parent
		State *parent;

		// Cost to reach from start
		int g;

		// Estimate path cost
		int f;

		// Action taken from parent
		char *action;

		// Stacks
		stack<int> A;
		stack<int> B;
		stack<int> C;

		// Constructor from parent
		State(State *parent,char *action);

		// Constructor from start/goal
		State(char *in_str);

		// Return stack with children states
		void expand(stack<State*> *children);

};

#endif
