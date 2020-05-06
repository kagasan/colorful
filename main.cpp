#include "image.hpp"

#include <random>
#include <vector>
#include <queue>
#include <iostream>

std::random_device rd;
std::mt19937_64 mt;
Color randomColor(){
    return HLStoRGB(mt() % 360, 50 + mt() % 50, 50 + mt() % 50);
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
    for(Point p; mp.ok(p); mp.next(p)){
        if(mp.get(p) != -1)continue;
        int id = colorVec.size();
        colorVec.push_back(randomColor());
        std::queue<Point>Q;
        Q.push(p);
        mp.set(p, id);
        while(!Q.empty()){
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
    for(Point p; mp.ok(p); mp.next(p)){
        if(mp.get(p) == -2)dst.setColor(p, Color(0, 0, 0));
        dst.setColor(p, colorVec[mp.get(p)]);
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