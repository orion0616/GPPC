#include <stdint.h>
#include <vector>
#include <cmath>

struct xyLoc {
    int16_t x;
    int16_t y;
    int hash() {
        return (x+1)*10000+y;
    };
};

enum Direction
{
    UP,
    UPPERLEFT,
    LEFT,
    LOWERLEFT,
    DOWN,
    LOWERRIGHT,
    RIGHT,
    UPPERRIGHT
};

struct state {
    int id;
    double f_value;
    double g_value;
    bool operator>(const state& rhs) const {
        if(f_value == rhs.f_value)
            return g_value < rhs.g_value;
        else
            return f_value > rhs.f_value;
    }
    state() {};
    state(int id, double f, double g) {
        this->id = id;
        this->f_value = f;
        this->g_value = g;
    }
};

struct node: xyLoc {
    double minimum;
    node* parent;
    double g;
    bool isOpen;
    node() {};
    node(xyLoc l) {
        this->x = l.x;
        this->y = l.y;
        minimum = 0;
        isOpen = true;
        parent = NULL;
    }
    node(int16_t x, int16_t y) {
        this->x = x;
        this->y = y;
    }
    double octile(xyLoc g) {
        double dx = abs(x-g.x);
        double dy = abs(y-g.y);
        return (dx+dy) + (1.4142-2)*fmin(dx,dy);
    }
    bool isNULL() {
        return this->x < 0 && this->y < 0;
    }
};

struct xyWithFGH: xyLoc {
    double f;
    double g;
    double h;
    xyWithFGH(node n, xyLoc g) {
        this->x = n.x;
        this->y = n.y;
        this->g = n.g;
        this->h = n.octile(g);
        this->f = this->g + this->f;
    }
    xyWithFGH() {}
};

extern int expanded;
void PreprocessMap(std::vector<bool> &bits, int width, int height, const char *filename);
void *PrepareForSearch(std::vector<bool> &bits, int width, int height, const char *filename);
bool GetPath_ASTAR(void *data, xyLoc s, xyLoc g, std::vector<xyLoc> &path,
                   std::vector<xyWithFGH> &expanded_nodes, std::function<void(node&,std::vector<node>&, xyLoc)> get_successors);
bool GetPath(void *data, xyLoc s, xyLoc g, std::vector<xyLoc> &path, std::vector<xyWithFGH> &expanded_nodes);
const char *GetName();
void printPath(std::vector<xyLoc> path);
bool isValidNode(node n);
bool are_successors_forced(node px, node x, Direction d, xyLoc g);
void GetSuccessors(xyLoc s, std::vector<xyLoc> & neighbors);
void GetSuccessors_for_astar(node & s, std::vector<node> & successors, xyLoc g);
void GetSuccessors_for_jps(node & s, std::vector<node> & neighbors, xyLoc g);
int GetIndex(xyLoc s);
void ExtractPath(xyLoc end, std::vector<xyLoc> & finalPath);
