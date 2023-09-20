#include <math.h>
#include <limits.h>
int * djikstra(struct room *r, int pc_x, int pc_y) {
    int dist[21][80];
    int i, j;
    for(j = 0; j < 21; j++) {
        for(i = 0; i < 80; i++) {
            dist[j][i] = INT_MAX;
        }
    }
    dist[pc_y][pc_x] = 0;

    return dist;
}


/*Starting node has cost of zero. all else infinity.
Queue has @ with cost 0
@ inf
A inf
B inf
C inf
D inf
...

Pull out @ (lowest cost) and assign its neighbors their cost, mark start node (@) done. All checked nodes get a prev of @
Check next lowest (D) and set cost of its neighbors + cost D. All nodes get  prev of D. mark D done
Pick next lowest cost again (say, A) and find neighbors' cost and assign cost + A to them. mark previous as A and done
If a new cost is ever cheaper than an old cost, update the cost and the prev
If all neighbors are done, the node is done

remove edge_penalty
Remove 