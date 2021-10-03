#include <stdio.h>


float belief[11];

struct state{
    int row;
    int col;
}; 
typedef struct state state_t;

//Setting up the terminal states
const state_t term1{3, 4};
const state_t term2{2, 4};
const state_t pillar{2, 2};

//Helper to access belief state array
int get_index(state_t s)
{
    return (4*(s.row-1)+(s.col-1));
}


//Check equality between two states
bool check_states(const state_t& s1, const state_t& s2) 
{
    return ((s1.row == s2.row) && (s1.col == s2.col));
}

//Initialise a belief state, given a uniform assumption about initial state, i.e. agent could be anywhere non-terminal
void init_belief_uniform()
{
    int i = 10;
    while (i >= 0) {
        belief[i] = 1.f/9;
        i--;
    }
    belief[get_index(term1)] = 0;
    belief[get_index(term2)] = 0;   
}

//Initialise the belief state, given a certain initial state
void init_belief(state_t s)
{
    int determined = get_index(s);
    int i = 10;
    while (i >= 0) {
        belief[i] = 0;
        i--;
    }
    belief[determined] = 1.f;
}

//Ensures all (row, col) tuples are within bounds
void check_boundries(state_t * states, int action)
{
    //Check if inside grid
    for (int i = 0; i++; i < 3) {
        if (states[i].row < 1)
            states[i].row = 1;
    
        if (states[i].row > 3)
            states[i].row = 3;
        
        if (states[i].col < 1)
            states[i].col = 1;
        
        if (states[i].col > 4)
            states[i].row = 4; 
    }

    //Check if inside pillar
    if (check_states(states[i], pillar)) {
        switch (action) {
            //UP
            case 0:
                states[i].row = 1;
                states[i].col = 2;
                break;
            //DOWN
            case 1:
                states[i].row = 3;
                states[i].col = 2;
                break;
            //LEFT
            case 2:
                states[i].row = 2;
                states[i].col = 3;
                break;
            //RIGHT
            case 3:
                states[i].row = 2;
                states[i].col = 1;
                break;
        }
    }
}

//Returns the probability of getting to s1 from s0, given the intended action a
float transition(int action, state_t s0, state_t s1)
{
    float res = 0.f;

    if (check_states(s0, term1) || check_states(s0, term2))
        return res;
    
    state_t reachable[3]; //First index is intended direction
    
    switch(action) {
        //UP
        case 0:
            reachable[0] = state_t{s0.row + 1, s0.col + 0};
            reachable[1] = state_t{s0.row + 0, s0.col + 1};
            reachable[2] = state_t{s0.row + 0, s0.col - 1};
            check_boundries(&reachable[0], 0);
            break;

        //DOWN
        case 1:
            reachable[0] = state_t{s0.row - 1, s0.col + 0};
            reachable[1] = state_t{s0.row + 0, s0.col + 1};
            reachable[2] = state_t{s0.row + 0, s0.col - 1};
            check_boundries(&reachable[0], 1);
            break;

        //LEFT
        case 2:
            reachable[0] = state_t{s0.row + 0, s0.col - 1};
            reachable[1] = state_t{s0.row + 1, s0.col + 0};
            reachable[2] = state_t{s0.row - 1, s0.col - 0};
            check_boundries(&reachable[0], 2);
            break;

        //RIGHT
        case 3:
            reachable[0] = state_t{s0.row + 0, s0.col + 1};
            reachable[1] = state_t{s0.row + 1, s0.col + 0};
            reachable[2] = state_t{s0.row - 1, s0.col - 0};
            check_boundries(&reachable[0], 3);
            break;

        default:
            break;
    }

    if (check_states(reachable[0], s1))
        res+=0.8;
    
    if (check_states(reachable[1], s1))
        res+=0.1;
    
    if (check_states(reachable[2], s1))
        res+=0.1;
    
    return res;
}

//Computes the probability of an observation, given the state
float sensor(int obs, state_t s)
{
    float res;
    //obs is 1wall, 2wall, or end (0)
    if ((obs == 1) && !(check_states(s, term1) || check_states(s, term2))) {
        res = (s.col == 3)? 0.9 : 0.1;
    } else if ((obs == 2) && !(check_states(s, term1) || check_states(s, term2))) {
        res = (s.col == 3)? 0.1 : 0.9;
    } else if ((obs == 0) && (check_states(s, term1) || check_states(s, term2))) {
        res = 1.f;
    } else {
        res = 0.f;
    }
    return res;
}

int main()
{
    init_belief_uniform();
    printf("just a review\n");
    return 0;
}