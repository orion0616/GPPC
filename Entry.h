#include <stdint.h>

struct xyLoc {
  int16_t x;
  int16_t y;
};

extern int expanded;
void PreprocessMap(std::vector<bool> &bits, int width, int height, const char *filename);
void *PrepareForSearch(std::vector<bool> &bits, int width, int height, const char *filename);
bool GetPath(void *data, xyLoc s, xyLoc g, std::vector<xyLoc> &path);
const char *GetName();
void printPath(std::vector<xyLoc> path);
