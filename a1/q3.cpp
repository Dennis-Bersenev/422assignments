#include <stdio.h>
#include <math.h>
#include <iostream>
#include <io.h>
#include <vector>

//Holds beliefs of being in each state; ignores entry 5 FOR PILLAR
float belief[12];

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

//Helper to get the state corresponding to an index from the belief array
state_t get_state(int index)
{
    int row = floor(index/4) + 1;
    int col = index - ((row - 1) * 4) + 1;
    return state_t{row, col};
}


//Check equality between two states
bool check_states(const state_t& s1, const state_t& s2) 
{
    return ((s1.row == s2.row) && (s1.col == s2.col));
}

//Initialise a belief state, given a uniform assumption about initial state, i.e. agent could be anywhere non-terminal
void init_belief_uniform()
{
    int i = 11;
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
    int i = 11;
    while (i >= 0) {
        belief[i] = 0;
        i--;
    }
    belief[determined] = 1.f;
}

//Ensures all (row, col) tuples are within bounds
void check_boundries(state_t * states, const state_t& s0)
{
    //Check if outside the grid or inside the pillar
    for (int i = 0; i < 3; i++) {
        if ((check_states(states[i], pillar)) || (states[i].row < 1) || (states[i].row > 3) || (states[i].col < 1) || (states[i].col > 4)) {
            states[i].row = s0.row;
            states[i].col = s0.col;
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
            break;

        //DOWN
        case 1:
            reachable[0] = state_t{s0.row - 1, s0.col + 0};
            reachable[1] = state_t{s0.row + 0, s0.col - 1};
            reachable[2] = state_t{s0.row + 0, s0.col + 1};
            break;

        //LEFT
        case 2:
            reachable[0] = state_t{s0.row + 0, s0.col - 1};
            reachable[1] = state_t{s0.row + 1, s0.col + 0};
            reachable[2] = state_t{s0.row - 1, s0.col - 0};
            break;

        //RIGHT
        case 3:
            reachable[0] = state_t{s0.row + 0, s0.col + 1};
            reachable[1] = state_t{s0.row - 1, s0.col + 0};
            reachable[2] = state_t{s0.row + 1, s0.col - 0};
            break;

        default:
            break;
    }

    check_boundries(&reachable[0], s0);
            
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

void output_beliefs()
{
    FILE* fd;
    if (fopen_s(&fd, "out/test_4", "w") != 0 || fd == NULL)
        exit(errno);

    if (_dup2(_fileno(fd), _fileno(stdout)) != 0)
        exit(errno);

    printf("----------------------------\n");
    printf("|%.4f|%.4f|%.4f|%.4f|\n", belief[8], belief[9], belief[10], belief[11]);
    printf("|%.4f|xxxxxx|%.4f|%.4f|\n", belief[4], belief[6], belief[7]);
    printf("|%.4f|%.4f|%.4f|%.4f|\n", belief[0], belief[1], belief[2], belief[3]);
    printf("----------------------------\n");
    fclose(fd);
    return;
}

int main()
{
    
    int seq_length = 4;

    //1. Specify strat
    //init_belief_uniform();
    //state_t s_initial{3, 2};
    //init_belief(s_initial)
    state_t s_initial{1, 1};
    init_belief(s_initial)
    
    //2. Specify action seq
    //int actions[] = {0, 0, 0};
    //int actions[] = {0, 0, 0};
    //int actions[] = {3, 3, 0};
    int actions[] = {0, 3, 3, 3};

    //3. Specify observation seq
    //int obs[] = {2, 2, 2};
    //int obs[] = {1, 1, 1};
    //int obs[] = {1, 1, 0};
    int obs[] = {2, 2, 1, 1};

    //eg. test transition
    for (int i = 0; i < seq_length; i++) {
        float b_new[12];
        float sum = 0.f;
        for (int sp =0; sp < 12; sp++) {
            if (sp==5) 
                continue;
            else {
                float tot = 0.f;
                for (int s = 0; s < 12; s++) {
                    if (s==5)
                        continue;
                    else {
                        tot += (transition(actions[i], get_state(s), get_state(sp)) * belief[s]);
                    }
                }
                tot *= sensor(obs[1], get_state(sp));
                //((sp+1)%4 ==0)? printf("transition res: %f\n\n", tot):printf("transition res: %f\n", tot);
                b_new[sp] = tot;
                sum += tot;
            }
        }
        //printf("alpha denom = %f\n", sum);
        float alpha = 1.f/sum;
        for (int sp = 0; sp < 12; sp++) {
            if (sp==5)
                continue;
            else {
                b_new[sp] *= alpha;
                belief[sp] = b_new[sp];
            }
        }
    }
    output_beliefs();
    return 0;
}