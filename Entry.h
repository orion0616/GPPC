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
    }
    double octile(xyLoc g) {
        double dx = abs(x-g.x);
        double dy = abs(y-g.y);
        return (dx+dy) + (1.4142-2)*fmin(dx,dy);
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
bool GetPath_ASTAR(void *data, xyLoc s, xyLoc g, std::vector<xyLoc> &path, std::vector<xyWithFGH> &expanded_nodes);
bool GetPath(void *data, xyLoc s, xyLoc g, std::vector<xyLoc> &path, std::vector<xyWithFGH> &expanded_nodes);
const char *GetName();
void printPath(std::vector<xyLoc> path);
void GetSuccessors(xyLoc s, std::vector<xyLoc> &neighbors);
void GetSuccessors_for_astar(node &s, std::vector<node> &neighbors, xyLoc g);
int GetIndex(xyLoc s);
void ExtractPath(xyLoc end, std::vector<xyLoc> &finalPath);
