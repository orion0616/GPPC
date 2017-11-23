#include <stdio.h>
#include <stdint.h>
#include <numeric>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "ScenarioLoader.h"
#include "Timer.h"
#include "Entry.h"

void LoadMap(const char *fname, std::vector<bool> &map, int &w, int &h);

struct stats {
    std::vector<double> times;
    std::vector<xyLoc> path;
    std::vector<int> lengths;
    int total_expanded_nodes;

    double GetTotalTime()
    {
        return std::accumulate(times.begin(), times.end(), 0.0);
    }
    double GetMaxTimestep()
    {
        return *std::max_element(times.begin(), times.end());
    }
    double Get20MoveTime()
    {
        for (unsigned int x = 0; x < lengths.size(); x++)
            if (lengths[x] >= 20)
                return std::accumulate(times.begin(), times.begin()+1+x, 0.0);
        return GetTotalTime();
    }
    double GetPathLength()
    {
        double len = 0;
        for (int x = 0; x < (int)path.size()-1; x++)
        {
            if (path[x].x == path[x+1].x || path[x].y == path[x+1].y)
            {
                len++;
            }
            else {
                len += 1.4142;
            }
        }
        return len;
    }
    bool ValidatePath(int width, int height, const std::vector<bool> &mapData)
    {
        for (int x = 0; x < (int)path.size()-1; x++)
        {
            if (abs(path[x].x - path[x+1].x) > 1)
                return false;
            if (abs(path[x].y - path[x+1].y) > 1)
                return false;
            if (!mapData[path[x].y*width+path[x].x])
                return false;
            if (!mapData[path[x+1].y*width+path[x+1].x])
                return false;
            if (path[x].x != path[x+1].x && path[x].y != path[x+1].y)
            {
                if (!mapData[path[x+1].y*width+path[x].x])
                    return false;
                if (!mapData[path[x].y*width+path[x+1].x])
                    return false;
            }
        }
        return true;
    }
};

string getFileName(char* path) {
    string s(path);
    while(s.find("/") != std::string::npos) {
        s = s.substr(s.find("/") + 1);
    }
    return s;
}

int main(int argc, char **argv)
{
    char filename[255];
    std::vector<xyLoc> thePath;
    std::vector<xyWithFGH> theExpandeds;
    std::vector<bool> mapData;
    int width, height;
    bool pre = false;
    bool run = false;

    if (argc != 4)
    {
        printf("Invalid Arguments\nUsage %s <flag> <map> <scenario>\n", argv[0]);
        printf("Flags:\n");
        printf("\t-full : Preprocess map and run scenario\n");
        printf("\t-pre : Preprocess map\n");
        printf("\t-run : Run scenario without preprocessing\n");
        exit(0);
    }
    if (strcmp(argv[1], "-full") == 0)
    {
        pre = run = true;
    }
    else if (strcmp(argv[1], "-pre") == 0)
    {
        pre = true;
    }
    else if (strcmp(argv[1], "-run") == 0)
    {
        run = true;
    }
    else {
        printf("Invalid Arguments\nUsage %s <flag> <map> <scenario>\n", argv[0]);
        printf("Flags:\n");
        printf("\t-full : Preprocess map and run scenario\n");
        printf("\t-pre : Preprocess map\n");
        printf("\t-run : Run scenario without preprocessing\n");
        exit(0);
    }

    LoadMap(argv[2], mapData, width, height);
    sprintf(filename, "%s-%s", GetName(), argv[2]);

    if (pre)
    {
        PreprocessMap(mapData, width, height, filename);
    }

    if (!run)
    {
        return 0;
    }

    void *reference = PrepareForSearch(mapData, width, height, filename);

    ScenarioLoader scen(argv[3]);

    Timer t;
    std::vector<stats> experimentStats;
    for (int x = 0; x < scen.GetNumExperiments(); x++)
    {
        //printf("%d of %d\n", x+1, scen.GetNumExperiments());
        thePath.resize(0);
        theExpandeds.resize(0);
        experimentStats.resize(x+1);
        bool done;
        do {
            xyLoc s, g;
            s.x = scen.GetNthExperiment(x).GetStartX();
            s.y = scen.GetNthExperiment(x).GetStartY();
            g.x = scen.GetNthExperiment(x).GetGoalX();
            g.y = scen.GetNthExperiment(x).GetGoalY();

            t.StartTimer();
            // done = GetPath(reference, s, g, thePath, theExpandeds);
            done = GetPath_ASTAR(reference, s, g, thePath, theExpandeds);
            t.EndTimer();

            experimentStats[x].times.push_back(t.GetElapsedTime());
            experimentStats[x].lengths.push_back(thePath.size());
            if(!done){
                experimentStats[x].total_expanded_nodes = expanded;

                std::ofstream fout;
                char pathfile[255];
                string mapfile = getFileName(argv[2]);
                string scefile = getFileName(argv[3]);
                sprintf(pathfile, "expanded/%s-%s%s-%d-expanded.txt",mapfile.c_str(), scefile.c_str(), argv[1], x);
                fout.open(pathfile);
                if (fout.fail()){
                    std::cout << "Opening the input file failed." << std::endl;
                    exit(1);
                }
                for (auto it = theExpandeds.begin(); it != theExpandeds.end(); it++){
                    fout << "(" << it->x << "," << it->y << "," << it->f << "," << it->g << "," << it->h << ")";
                }
                fout << std::endl;
                fout.close();
            }
            for (unsigned int t = experimentStats[x].path.size(); t < thePath.size(); t++)
                experimentStats[x].path.push_back(thePath[t]);
        } while (done == false);

    }

    for (unsigned int x = 0; x < experimentStats.size(); x++)
    {
        printf("%s\ttotal-time\t%f\tmax-time-step\t%f\ttime-20-moves\t%f\ttotal-len\t%f\tsubopt\t%f\texpanded_nodes\t%d\t", argv[3],
               experimentStats[x].GetTotalTime(), experimentStats[x].GetMaxTimestep(), experimentStats[x].Get20MoveTime(),
               experimentStats[x].GetPathLength(), experimentStats[x].GetPathLength()/scen.GetNthExperiment(x).GetDistance(), experimentStats[x].total_expanded_nodes);
        if (experimentStats[x].path.size() == 0 ||
                (experimentStats[x].ValidatePath(width, height, mapData) &&
                 scen.GetNthExperiment(x).GetStartX() == experimentStats[x].path[0].x &&
                 scen.GetNthExperiment(x).GetStartY() == experimentStats[x].path[0].y &&
                 scen.GetNthExperiment(x).GetGoalX() == experimentStats[x].path.back().x &&
                 scen.GetNthExperiment(x).GetGoalY() == experimentStats[x].path.back().y))
        {
            printf("valid\n");
        }
        else {
            printf("invalid\n");
        }
    }

    // logging paths
    std::ofstream fout;
    char pathfile[255];
    string mapfile = getFileName(argv[2]);
    string scefile = getFileName(argv[3]);
    sprintf(pathfile, "%s-%s%s-paths.txt", mapfile.c_str(), scefile.c_str(), argv[1]);
    fout.open(pathfile);
    if (fout.fail()){
        std::cout << "Opening the input file failed." << std::endl;
        exit(1);
    }
    fout << argv[2] << std::endl;
    for (unsigned int x = 0; x < experimentStats.size(); x++) {
        std::vector<xyLoc> path = experimentStats[x].path;
        for (auto it = path.begin(); it != path.end(); it++){
            fout << "(" << it->x << "," << it->y << ")";
        }
        fout << std::endl;
    }
    fout.close();
    return 0;
}

void LoadMap(const char *fname, std::vector<bool> &map, int &width, int &height)
{
    FILE *f;
    f = fopen(fname, "r");
    if (f)
    {
        fscanf(f, "type octile\nheight %d\nwidth %d\nmap\n", &height, &width);
        map.resize(height*width);
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                char c;
                do {
                    fscanf(f, "%c", &c);
                } while (isspace(c));
                map[y*width+x] = (c == '.' || c == 'G' || c == 'S');
                //printf("%c", c);
            }
            //printf("\n");
        }
        fclose(f);
    }
}
