
/* 
The rule base is defined here....This example is for a system with 4 I/Os in total and 6 rules 
*/
int rule_dim_1 = 9;
int rule_dim_2 = 4;

 // rules[no. of rules][no. of I/Os]={ {ant_no, ant_no, cons_no, cons_no}, {,,,}, ........,{,,,}}

int rules[9][4] = {
    {4, 4, 5, 5},   //high err high d_err : high power
    {4, 2, 5, 5},   //high err no   d_err : high power
    {4, 1, 4, 4},   //high err no   d_err : high power
    {3, 1, 3, 3},   //low  err neg  d_err : low  power
    {3, 2, 4, 4},   //low  err no   d_err : low  power
    {2, 3, 4, 4},   //no   err low  d_err : low  power
    {2, 2, 3, 3},   //no   err no   d_err : no   power
    {2, 1, 2, 2},   //no   err neg  d_err : neg  power
    {1, 2, 2, 2},   //neg  err no   d_err : neg  power
    };
