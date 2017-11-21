#include <deque>
#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <cmath>
#include "Entry.h"

std::vector<bool> map;
std::vector<int> visited;
std::vector<xyLoc> succ;
int width, height;
int expanded;

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

bool GetPath_ASTAR(void *data, xyLoc s, xyLoc g, std::vector<xyLoc> &path, std::vector<xyWithFGH> &expanded_nodes){
    expanded = 0;
    assert((long)data == 13182);
    std::priority_queue<state, std::vector<state>, std::greater<state> > q;
    std::unordered_map<int, node> table;
    std::vector<node> successors;
    if(path.size() > 0){
        path.push_back(g);
        return true;
    }
    node start_node(s);
    table[s.hash()] = start_node;
    state start_state(s.hash(), start_node.minimum, 0);
    q.push(start_state);

    int goal_id = g.hash();
    state next;

    while(q.size() > 0){
        next = q.top();
        if(goal_id == next.id){
            break;
        }
        q.pop();

        node next_node = table[next.id];
        if(!next_node.isOpen){
            continue;
        }
        table[next.id].isOpen = false;

        xyWithFGH n(next_node, g);
        expanded_nodes.push_back(n);
        expanded++;

        GetSuccessors_for_astar(table[next.id], successors, g);
        for(auto it = successors.begin(); it != successors.end();it++){
            if(table.find(it->hash()) != table.end()){
                if(table[it->hash()].minimum > it->minimum){
                    table[it->hash()] = *it;
                    next = state(it->hash(), it->g + it->octile(g),it->g);
                    q.push(next);
                }
            } else {
                table[it->hash()] = *it;
                next = state(it->hash(), it->g + it->octile(g), it->g);
                q.push(next);
            }
        }
    }
    node last = table[next.id];

    while(last.x != s.x || last.y != s.y){
        xyLoc pos;
        pos.x = last.x;
        pos.y = last.y;
        path.push_back(pos);
        last = *last.parent;
    }
    path.push_back(s);
    std::reverse(path.begin(), path.end());
    if(path.size() > 0){
        path.pop_back();
        return false;
    }
    return true;
}

bool GetPath(void *data, xyLoc s, xyLoc g, std::vector<xyLoc> &path, std::vector<xyWithFGH> &expanded_nodes)
{
    expanded = 0;
    assert((long)data == 13182);
    std::deque<xyLoc> q;

    if (path.size() > 0)
    {
        path.push_back(g);
        return true;
    }
    visited.assign(map.size(),0);
    visited[GetIndex(s)] = 1;
    q.push_back(s);

    //BFS
    while (q.size() > 0)
    {
        xyLoc next = q.front();
        xyWithFGH next_node = xyWithFGH();
        double dist = sqrt((next.x - g.x)*(next.x - g.x)+(next.y-g.y)*(next.y-g.y));
        next_node.x = next.x, next_node.y = next.y, next_node.f = 0, next_node.g = 0, next_node.h = dist;
        expanded_nodes.push_back(next_node);
        expanded++;

        q.pop_front();
        GetSuccessors(next, succ);
        for (unsigned int x = 0; x < succ.size(); x++)
        {
            if (visited[GetIndex(succ[x])])
                continue;
            visited[GetIndex(succ[x])] = visited[GetIndex(next)]+1;

            if (succ[x].x == g.x && succ[x].y == g.y) // goal found
            {
                ExtractPath(g, path);
                if (path.size() > 0)
                {
                    path.pop_back();
                    return false;
                }
                return true; // empty path
            }

            q.push_back(succ[x]);
        }
    }
    return true; // no path returned, but we're done
}

int GetIndex(xyLoc s)
{
    return s.y*width+s.x;
}

enum Direction {
    UP, UPPERLEFT, LEFT, LOWERLEFT, DOWN, LOWERRIGHT, RIGHT, UPPERRIGHT
};

node create_next_node(node &orig, xyLoc goal, Direction d){
    node next = orig;
    next.parent = &orig;
    if(d%2 == 0){
        next.g = orig.g + 1;
    } else {
        next.g = orig.g + 1.4142;
    }
    switch(d) {
        case UP:                   next.y--; break;
        case UPPERLEFT:  next.x--; next.y--; break;
        case LEFT:       next.x--;           break;
        case LOWERLEFT:  next.x--; next.y++; break;
        case DOWN:                 next.y++; break;
        case LOWERRIGHT: next.x++; next.y++; break;
        case RIGHT:      next.x++;           break;
        case UPPERRIGHT: next.x++; next.y--; break;
    }
    next.isOpen = true;
    next.minimum = next.g + next.octile(goal);
    return next;
}

void GetSuccessors_for_astar(node &s, std::vector<node> &neighbors, xyLoc g){
    bool up = false, down = false, left = false, right = false;
    neighbors.resize(0);

    node next = create_next_node(s,g,RIGHT);
    if (next.x < width && map[GetIndex(next)]){
        neighbors.push_back(next);
        right = true;
    }
    next = create_next_node(s,g,LEFT);
    if (next.x >= 0 && map[GetIndex(next)]){
        neighbors.push_back(next);
        left = true;
    }
    next = create_next_node(s,g,UP);
    if (next.y >= 0 && map[GetIndex(next)]){
        neighbors.push_back(next);
        up = true;
    }
    next = create_next_node(s,g,DOWN);
    if (next.y < height && map[GetIndex(next)]){
        neighbors.push_back(next);
        down = true;
    }
    next = create_next_node(s,g,LOWERRIGHT);
    if (next.y < height && next.x < width && map[GetIndex(next)] && right && down)
        neighbors.push_back(next);
    next = create_next_node(s,g,UPPERRIGHT);
    if (next.y >= 0 &&  next.x < width && map[GetIndex(next)] && right && up)
        neighbors.push_back(next);
    next = create_next_node(s,g,UPPERLEFT);
    if (next.y >= 0 && next.x >= 0 && map[GetIndex(next)] && left && up)
        neighbors.push_back(next);
    next = create_next_node(s,g,LOWERLEFT);
    if (next.y < height && next.x >= 0 && map[GetIndex(next)] && left && down)
        neighbors.push_back(next);
}

// generates 4-connected neighbors
// doesn't generate 8-connected neighbors (which ARE allowed)
// a diagonal move must have both cardinal neighbors free to be legal
void GetSuccessors(xyLoc s, std::vector<xyLoc> &neighbors)
{
    neighbors.resize(0);

    xyLoc next = s;
    next.x++;
    // map[index] is true only when index is "G"||"S"||"."
    if (next.x < width && map[GetIndex(next)])
        neighbors.push_back(next);

    next = s;
    next.x--;
    if (next.x >= 0 && map[GetIndex(next)])
        neighbors.push_back(next);

    next = s;
    next.y--;
    if (next.y >= 0 && map[GetIndex(next)])
        neighbors.push_back(next);

    next = s;
    next.y++;
    if (next.y < height && map[GetIndex(next)])
        neighbors.push_back(next);
}

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
