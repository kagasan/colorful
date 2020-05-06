#include "image.hpp"

#include <random>
#include <vector>
#include <queue>
#include <iostream>
#include <stdio.h>
#include <map>
#include <algorithm>
#include <set>

std::random_device rd;
std::mt19937_64 mt;
Color randomColor(){
    return HLStoRGB(mt() % 360, 50 + mt() % 50, 50 + mt() % 50);
}
void negaposi(std::string input, std::string output);

class AreaInfo{
    public:
    std::string name;
    std::vector<std::string>parents;
    std::vector<int>parentsId;
    std::vector<std::string>neighbors;
    std::vector<int>neighborsId;
    std::vector<std::string>children;
    std::vector<int>childrenId;
    Color color;
    int min_x, min_y;
    int max_x, max_y;
    AreaInfo(std::vector<std::string> parents = {}, std::vector<std::string>neighbors = {}, std::vector<std::string>children = {}, Color color = Color()):
    parents(parents), neighbors(neighbors), children(children), color(color){
        name = "";
        min_x = 1145141919;
        min_y = 1145141919;
        max_x = -1;
        max_y = -1;        
    }
    void update(Point p){
        min_x = std::min(min_x, p.x);
        min_y = std::min(min_y, p.y);
        max_x = std::max(max_x, p.x);
        max_y = std::max(max_y, p.y);
    }
    bool contain(AreaInfo ac){
        if(min_x <= ac.min_x && min_y <= ac.min_y && max_x >= ac.max_x && max_y >= ac.max_y)return true;
        return false;
    }
    bool hasParent(std::string parent){
        if(std::count(parents.begin(), parents.end(), parent))return true;
        return false;
    }
    bool hasNeighbor(std::string neighbor){
        if(std::count(neighbors.begin(), neighbors.end(), neighbor))return true;
        return false;
    }
    bool hasChild(std::string child){
        if(std::count(children.begin(), children.end(), child))return true;
        return false;
    }
};

std::map<std::string, AreaInfo> makeAreaMap(){
    std::map<std::string, AreaInfo> areaMap;
    areaMap["background"] = AreaInfo(
        {}, {}, {"body", "foot"}, Color(200, 200, 200)
    );
    areaMap["body"] = AreaInfo(
        {"background"}, {"foot"}, {"white_eye", "mouth"}, Color(128, 128, 255)
    );
    areaMap["white_eye"] = AreaInfo(
        {"body"}, {}, {"black_eye"}, Color(255, 255, 255)
    );
    areaMap["black_eye"] = AreaInfo(
        {"white_eye"}, {}, {}, Color(25, 25, 25)
    );
    areaMap["mouth"] = AreaInfo(
        {"body"}, {}, {}, Color(255, 128, 128)
    );
    areaMap["foot"] = AreaInfo(
        {"background"}, {"body"}, {}, Color(255, 255, 128)
    );
    return areaMap;
}

std::vector<std::vector<std::string> > makeDepthNames(std::map<std::string, AreaInfo> areaMap){
    std::vector<int>parentsNum;
    std::vector<std::string>names;
    for(auto kv : areaMap){
        parentsNum.push_back(kv.second.parents.size());
        names.push_back(kv.first);
    }
    // std::cout << std::endl;
    std::vector<std::vector<std::string> >res;
    for(int dep = 0;; dep++){
        std::vector<std::string>tmpVec;
        for(int i = 0; i < parentsNum.size(); i++){
            if(parentsNum[i] == 0){
                tmpVec.push_back(names[i]);
                parentsNum[i] = -1;
            }
        }
        for(int i = 0; i < parentsNum.size(); i++){
            if(parentsNum[i] <= 0)continue;
            for(auto parent : areaMap[names[i]].parents){
                if(std::count(tmpVec.begin(), tmpVec.end(), parent)){
                    parentsNum[i]--;
                }
            }
        }
        if(tmpVec.size() == 0)break;
        res.push_back(tmpVec);
        // std::cout << dep << " : ";
        // for(auto s : tmpVec)std::cout << "[" << s << "] ";
        // std::cout << std::endl;
    }
    return res;
}

std::vector<int>makeAreaDepth(std::vector<AreaInfo> areaVec){
    std::vector<int>res(areaVec.size(), 0);
    std::vector<int>parentsNum;
    for(int i = 0; i < areaVec.size(); i++){
        parentsNum.push_back(areaVec[i].parentsId.size());
    }
    for(int i = 0;; i++){
        std::set<int>S;
        for(int j = 0; j < parentsNum.size(); j++){
            if(parentsNum[j] == 0){
                S.insert(j);
                parentsNum[j] = -1;
                res[j] = i;
            }
        }
        for(int j = 0; j < parentsNum.size(); j++){
            if(parentsNum[j] <= 0)continue;
            for(auto x : areaVec[j].parentsId){
                if(S.find(x) != S.end())parentsNum[j]--;
            }
        }
        if(S.empty())break;
    }
    return res;
}

int evalArea(std::vector<AreaInfo> areaVec, std::map<std::string, AreaInfo> areaMap){
    int sum = 0;
    for(auto area : areaVec){
        for(auto id : area.parentsId)area.parents.push_back(areaVec[id].name);
        for(auto id : area.neighborsId)area.neighbors.push_back(areaVec[id].name);
        for(auto id : area.childrenId)area.children.push_back(areaVec[id].name);
        AreaInfo ai = areaMap[area.name];
        for(auto parent : area.parents){
            if(ai.hasParent(parent))sum++;
            else sum--;
        }
        for(auto neighbor : area.neighbors){
            if(ai.hasNeighbor(neighbor))sum++;
            else sum--;
        }
        for(auto child : area.children){
            if(ai.hasChild(child))sum++;
            else sum--;
        }
        for(auto parent : ai.parents){
            if(area.hasParent(parent))sum++;
            else sum--;
        }
        for(auto neighbor : ai.neighbors){
            if(area.hasNeighbor(neighbor))sum++;
            else sum--;
        }
        for(auto child : ai.children){
            if(area.hasChild(child))sum++;
            else sum--;
        }
    }

    return sum;
}

void colorful(std::string input, std::string output){
    Image src(input);
    Image dst(src.getWidth(), src.getHeight());
    int2d mp(src.getWidth(), src.getHeight());
    for(Point p; src.ok(p); src.next(p)){
        if(src.getColor(p).ave() < 20)mp.set(p, -2);
        else mp.set(p, -1);
    }
    Point dxy[4] = {Point(1, 0), Point(0, 1), Point(-1, 0), Point(0, -1)};
    std::vector<Color>colorVec;
    std::vector<AreaInfo>areaVec;
    for(Point p; mp.ok(p); mp.next(p)){
        if(mp.get(p) != -1)continue;
        int id = areaVec.size();
        colorVec.push_back(randomColor());
        areaVec.push_back(AreaInfo());
        std::queue<Point>Q;
        Q.push(p);
        mp.set(p, id);
        while(!Q.empty()){
            areaVec[id].update(Q.front());
            for(auto d : dxy){
                Point nxt = Q.front() + d;
                if(!mp.ok(nxt))continue;
                if(mp.get(nxt) != -1)continue;
                Q.push(nxt);
                mp.set(nxt, id);
            }
            Q.pop();
        }
    }

    int2d mat(areaVec.size(), areaVec.size(), 0);
    std::vector<Point>dxy7;
    for(int y = -3; y <= 3; y++){
        for(int x = -3; x <= 3; x++){
            dxy7.push_back(Point(x, y));
        }
    }
    
    for(Point p; mp.ok(p); mp.next(p)){
        int id = mp.get(p);
        if(id < 0)continue;
        for(auto d : dxy7){
            if(!mp.ok(p + d))continue;
            int to = mp.get(p + d);
            if(to < 0)continue;
            if(id == to)continue;
            mat.set(id, to, mat.get(id, to) + 1);
        }
    }
    // mat.show();

    for(int i = 0; i < areaVec.size(); i++){
        for(int j = 0; j < areaVec.size(); j++){
            if(mat.get(i, j) < 10)continue;
            if(areaVec[i].contain(areaVec[j]))areaVec[i].childrenId.push_back(j);
            else if(areaVec[j].contain(areaVec[i]))areaVec[i].parentsId.push_back(j);
            else areaVec[i].neighborsId.push_back(j);
        }
    }

    std::map<std::string, AreaInfo>areaMap = makeAreaMap();
    std::vector<std::string>names;
    for(auto kv : areaMap)names.push_back(kv.first);

    std::vector<std::vector<std::string> > depthNames = makeDepthNames(areaMap);
    std::vector<int> areaDepth = makeAreaDepth(areaVec);

    for(int i = 0; i < areaVec.size(); i++){
        int idx = i;
        int depth = std::min(int(depthNames.size() - 1), areaDepth[idx]);
        areaVec[idx].name = depthNames[depth][mt() % depthNames[depth].size()];
    }

    int score = evalArea(areaVec, areaMap);
    for(int i = 0; i < 1000; i++){
        std::vector<AreaInfo>tmpVec = areaVec;
        for(int j = 0; j < 2; j++){
            int idx = 0;
            int depth = 0;
            for(int k = 0; k < 100; k++){
                idx = mt() % areaVec.size();
                depth = std::min(int(depthNames.size() - 1), areaDepth[idx]);
                if(depthNames[depth].size() > 1)break;
            }
            tmpVec[idx].name = depthNames[depth][mt() % depthNames[depth].size()];
        }
        int tmpScore = evalArea(tmpVec, areaMap);
        if(tmpScore > score){
            score = tmpScore;
            areaVec = tmpVec;
        }
    }
    for(int i = 0; i < areaVec.size(); i++){
        std::cout << i << " : " << areaVec[i].name << std::endl;
    }
    
    for(Point p; mp.ok(p); mp.next(p)){
        if(mp.get(p) < 0)dst.setRGB(p, 0, 0, 0);
        else dst.setColor(p, areaMap[areaVec[mp.get(p)].name].color);
    }
    dst.write(output);
}

void controller(std::string input, std::string output){
    // negaposi(input, output);
    colorful(input, output);
}

int main(int argc, char* argv[]){
    mt = std::mt19937_64(rd());
    if(argc == 3){
        controller(std::string(argv[1]), std::string(argv[2]));
    }
    else{
        controller("./test_input.png", "./test_output.png");
    }

    return 0;
}

void negaposi(std::string input, std::string output){
    Image src(input);
    int width = src.getWidth();
    int height = src.getHeight();
    Image dst(width, height);
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            Color color = src.getColor(x, y);
            if(y < height / 2){
                dst.setColor(x, y, color);
                dst.setR(x, y, x % 256);
            }
            else{
                dst.setRGB(x, y, 255 - color.r, 255 - color.g, 255 - color.b);
            }
        }
    }
    dst.write(output);
}