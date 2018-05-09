#include <deque>
#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <cmath>
#include "Entry.h"
#include "state.h"
#include "BinaryHeap.h"
#include <thrust/device_vector.h>
#include <thrust/device_ptr.h>

#define N 512
#define  RAW(x) thrust::raw_pointer_cast(x)

std::vector<bool> map;
std::vector<int> visited;
std::vector<xyLoc> succ;
int width, height;

const char *GetName()
{
    return "TestProgram";
}

void PreprocessMap(std::vector<bool> &bits, int width, int height, const char *filename)
{
    printf("Not writing to file '%s'\n", filename);
}

void *PrepareForSearch(std::vector<bool> &bits, int w, int h, const char *filename)
{
    printf("Not reading from file '%s'\n", filename);
    map = bits;
    width = w;
    height = h;
    return (void *)13182;
}

__device__ state goal;
__device__ state candidate;

__global__ void remove(state*** arrays, int* lengths, state** S, int* lengthForS) {
    int num = threadIdx.x;
    state* array = array[num];
    int len = lengths[num];

    BinaryHeap pq = BinaryHeap(array, len);
    if (pq.empty()) {
        return;
    }
    state* min = pq.remove();
    if (min->node.x == goal.node.x && min->node.y == goal.node.y) {
        if (candidate.isNil() || min.f < candidate.f) {
            candidate = min;
        }
    }

    lengthForS[num] = GetSuccessors_for_gastar(min, S[num]);
    return;
}

bool isAllQueueEmpty(int* lengths) {
    for(int i=0; i< N; i++){
        if(lengths[i] != 0)
            return false;
    }
    return true;
}

__global__ void duplicate_detection(state** table, state* s, state*** pqs, int* lengths, int rand) {
    state old = state[s->hash()];
    if (!old->isNil() && old->g_value < s->g_value) {
        return;
    } else {
        /* s->f_value = s->f_value */
        curandState_t state;
        curand_init(0,0,0, &state);
        int result = curand(&state) %N;
        BinaryHeap pq = BinaryHeap(pqs[result], lengths[result]);
        pq.add(s);
        lengths[num] += 1;
    }
}

bool GetPath_GASTAR(void *data, xyLoc s, xyLoc g, std::vector<xyLoc> &path) {
    assert((long)data == 13182);

    thrust::device_vector<thrust::device_vector<state> > pqs;
    for(int i=0;i<N;i++){
        thrust::device_vector<state> tmp;
        pqs.push_back(tmp);
    }
    thrust::device_vector<int> lengths(N);
    lengths[0] = 1;
    pqs[0].push_back(state);
    thrust::device_vector<&state> table;

    if(path.size() > 0) {
        path.push_back(g);
        return true;
    }

    while(!isAllQueueEmpty(lengths)) {
        thrust::device_vector<thrust::device_vector<state> > S;
        for(int i=0;i<N;i++){
            thrust::device_vector<state> succ(8);
            pqs.push_back(succ);
        }
        thrust::device_vector<state> lengthForS(N,0);
        // XXX:parallel
        remove<<<1, N>>>(RAW(pqs), RAW(lengths), RAW(S), RAW(lengthForS));
        bool flag = true;
        if (!candidate.isNil()) {
            for(int i=0; i< N; i++) {
                if (pqs[i].empty()){
                    continue;
                } else {
                    if(candidate.f_value > pqs[i].top()->f_value)
                        flag = false;
                    break;
                }
            }
        }
        if(flag){
            break;
        }
        // XXX:parallel
        duplicate_detection<<<1,N>>>(RAW(table), RAW(s), RAW(pqs));
    }

    // TODO: must change
    while(last.x != s.x || last.y != s.y) {
        xyLoc pos;
        pos.x = last.x;
        pos.y = last.y;
        path.push_back(pos);
        last = *last.parent;
    }
    path.push_back(s);

    if(path.size() > 0) {
        path.pop_back();
        return false;
    }
    return true;
}

int GetIndex(xyLoc s)
{
    return s.y*width+s.x;
}

int GetSuccessors_for_gastar(state &s, state* neighbots) {
    return 0;
}

/* void GetSuccessors_for_astar(node &s, std::vector<node> &neighbors, xyLoc g) {     */
/*     bool up = false, down = false, left = false, right = false;                    */
/*     neighbors.resize(0);                                                           */

/*     node next = create_next_node(s,g,RIGHT);                                       */
/*     if (next.x < width && map[GetIndex(next)]) {                                   */
/*         neighbors.push_back(next);                                                 */
/*         right = true;                                                              */
/*     }                                                                              */
/*     next = create_next_node(s,g,LEFT);                                             */
/*     if (next.x >= 0 && map[GetIndex(next)]) {                                      */
/*         neighbors.push_back(next);                                                 */
/*         left = true;                                                               */
/*     }                                                                              */
/*     next = create_next_node(s,g,UP);                                               */
/*     if (next.y >= 0 && map[GetIndex(next)]) {                                      */
/*         neighbors.push_back(next);                                                 */
/*         up = true;                                                                 */
/*     }                                                                              */
/*     next = create_next_node(s,g,DOWN);                                             */
/*     if (next.y < height && map[GetIndex(next)]) {                                  */
/*         neighbors.push_back(next);                                                 */
/*         down = true;                                                               */
/*     }                                                                              */
/*     next = create_next_node(s,g,LOWERRIGHT);                                       */
/*     if (next.y < height && next.x < width && map[GetIndex(next)] && right && down) */
/*         neighbors.push_back(next);                                                 */
/*     next = create_next_node(s,g,UPPERRIGHT);                                       */
/*     if (next.y >= 0 &&  next.x < width && map[GetIndex(next)] && right && up)      */
/*         neighbors.push_back(next);                                                 */
/*     next = create_next_node(s,g,UPPERLEFT);                                        */
/*     if (next.y >= 0 && next.x >= 0 && map[GetIndex(next)] && left && up)           */
/*         neighbors.push_back(next);                                                 */
/*     next = create_next_node(s,g,LOWERLEFT);                                        */
/*     if (next.y < height && next.x >= 0 && map[GetIndex(next)] && left && down)     */
/*         neighbors.push_back(next);                                                 */
/* }                                                                                  */

// make plan
// trace path by visited
void ExtractPath(xyLoc end, std::vector<xyLoc> &finalPath)
{
    int currCost = visited[GetIndex(end)];

    finalPath.resize(0);
    finalPath.push_back(end);

    while (currCost != 1)
    {
        GetSuccessors(finalPath.back(), succ);
        for (unsigned int x = 0; x < succ.size(); x++)
        {
            if (visited[GetIndex(succ[x])] == currCost-1)
            {
                finalPath.push_back(succ[x]);
                currCost--;
                break;
            }
        }
    }
    std::reverse(finalPath.begin(), finalPath.end());
}

