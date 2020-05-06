#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image_write.h"
#include <string>
#include <sstream>

class Point{
    public:
    int x, y;
    Point(int x = 0, int y = 0):x(x),y(y){}
    Point operator + (const Point p){
        return Point(x + p.x, y + p.y);
    }
    std::string str(){
        std::stringstream ss;
        ss << "(" << x << ", " << y << ")";
        return ss.str();
    }
};

class int2d{
    public:
    int **data;
    int width;
    int height;
    int2d(int width, int height, int val = 0):width(width),height(height){
        data = new int*[height];
        for(int i = 0; i < height; i++){
            data[i] = new int[width];
        }
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                data[y][x] = val;
            }
        }
    }
    ~int2d(){
        for(int i = 0; i < height; i++){
            delete[] data[i];
        }
        delete data;
    }
    int get(int x, int y){
        return data[y][x];
    }
    int get(Point p){return get(p.x, p.y);}
    void set(int x, int y, int val){
        data[y][x] = val;
    }
    void set(Point p, int val){set(p.x, p.y, val);}
    bool ok(int x, int y){
        if(x < 0 || y < 0)return false;
        if(x >= width || y >= height)return false;
        return true;
    }
    bool ok(Point p){
        return ok(p.x, p.y);
    }
    bool next(int &x, int &y){
        x++;
        if(x >= width){
            x = 0;
            y++;
        }
        if(y >= height)return false;
        return true;
    }
    bool next(Point &p){
        return next(p.x, p.y);
    }
    void show(){
        printf("y  x :");
        for(int x = 0; x < width; x++)printf(" %8d", x);
        printf("\n");
        for(int y = 0; y < height; y++){
            printf("%4d :", y);
            for(int x = 0; x < width; x++){
                printf(" %8d", get(x, y));
            }
            printf("\n");
        }
    }
};

class Color{
    public:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    Color(unsigned char r = 255, unsigned char g = 255, unsigned char b = 255):r(r), g(g), b(b){}
    unsigned int sum(){
        return (unsigned int)r + (unsigned int)g + (unsigned int)b;
    }
    unsigned char ave(){
        return (unsigned char)(((unsigned int)r + (unsigned int)g + (unsigned int)b)/3);
    }
    unsigned char th(unsigned char t){
        return (ave() < t ? 0 : 255);
    }
    Color thColor(unsigned char t){
        unsigned c = th(t);
        return Color(c, c, c);
    }
    Color gray(){
        unsigned char c = ave();
        return Color(c, c, c);
    }
};

Color HLStoRGB(double h, double l, double s){
    Color rgb(0, 0, 0);
    while(h >= 360)h -= 360;
    double mx, mn;
    if(l <= 49){
        mx = 2.55 * (l + l * (s / 100));
        mn = 2.55 * (l - l * (s / 100));
    }else{
        mx = 2.55 * (l + (100 - l) * (s / 100));
        mn = 2.55 * (l - (100 - l) * (s / 100)); 
    }
    if (h < 60){
        rgb.r = mx;
        rgb.g = mn + (mx - mn) * (h / 60) ;
        rgb.b = mn;
    }else if (h >= 60 &&  h < 120){
        rgb.r = mn + (mx - mn) * ((120 - h) / 60);
        rgb.g = mx ;
        rgb.b = mn;    
    }else if (h >= 120 &&  h < 180){
        rgb.r = mn;
        rgb.g = mx ;
        rgb.b = mn + (mx - mn) * ((h - 120) / 60);        
    }else if (h >= 180 &&  h < 240){
        rgb.r = mn;
        rgb.g = mn + (mx - mn) * ((240 - h) / 60);
        rgb.b = mx;     
    }else if (h >= 240 &&  h < 300){
        rgb.r = mn + (mx - mn) * ((h - 240) / 60);
        rgb.g = mn;
        rgb.b = mx;     
    }else if (h >= 300 &&  h < 360){
        rgb.r = mx;
        rgb.g = mn;
        rgb.b = mn + (mx - mn) * ((360 - h) / 60); 
    }
    return rgb;
}

class Image{
    private:
    unsigned int width;
    unsigned int height;
    unsigned int bpp;
    unsigned char* pixels;
    unsigned int calculateIndex(unsigned int x, unsigned int y);

    public:
    Image(std::string filename);
    Image(unsigned int width = 640, unsigned height = 480);
    ~Image();
    
    unsigned int getWidth();
    unsigned int getHeight();
    unsigned char getR(unsigned int x, unsigned int y);
    unsigned char getG(unsigned int x, unsigned int y);
    unsigned char getB(unsigned int x, unsigned int y);
    unsigned char getR(Point p);
    unsigned char getG(Point p);
    unsigned char getB(Point p);
    Color getColor(unsigned int x, unsigned int y);
    Color getColor(Point p);

    void setR(unsigned int x, unsigned int y, unsigned char r);
    void setG(unsigned int x, unsigned int y, unsigned char g);
    void setB(unsigned int x, unsigned int y, unsigned char b);
    void setRGB(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b);
    void setColor(unsigned int x, unsigned int y, Color color);
    void setR(Point p, unsigned char r);
    void setG(Point p, unsigned char g);
    void setB(Point p, unsigned char b);
    void setRGB(Point p, unsigned char r, unsigned char g, unsigned char b);
    void setColor(Point p, Color color);
    
    void read(std::string filename);
    void write(std::string filename);
    bool next(int &x, int &y);
    bool ok(int x, int y);
    bool next(Point &p);
    bool ok(Point p);
};

unsigned int Image::calculateIndex(unsigned int x, unsigned int y){
    if(x >= width || y >= height)return 0;
    return y * width * bpp + x * bpp;
}

Image::Image(std::string filename){
    read(filename);
}

Image::Image(unsigned int width, unsigned int height):width(width),height(height){
    bpp = 4;
    pixels = new unsigned char[width * height * bpp];
    for(unsigned int y = 0, index = 0; y < height; y++){
        for(unsigned int x = 0; x < width; x++, index += 4){
            pixels[index] = 255;
            pixels[index + 1] = 255;
            pixels[index + 2] = 255;
            pixels[index + 3] = 255;
        }
    }
}

Image::~Image(){
    stbi_image_free(pixels);
}

unsigned int Image::getWidth(){
    return width;
}

unsigned int Image::getHeight(){
    return height;
}

unsigned char Image::getR(unsigned int x, unsigned int y){
    return pixels[calculateIndex(x, y)];
}
unsigned char Image::getG(unsigned int x, unsigned int y){
    return pixels[calculateIndex(x, y) + 1];
}
unsigned char Image::getB(unsigned int x, unsigned int y){
    return pixels[calculateIndex(x, y) + 2];
}

Color Image::getColor(unsigned int x, unsigned int y){
    return Color(getR(x, y), getG(x, y), getB(x, y));
}
unsigned char Image::getR(Point p){return getR(p.x, p.y);}
unsigned char Image::getG(Point p){return getG(p.x, p.y);}
unsigned char Image::getB(Point p){return getB(p.x, p.y);}
Color Image::getColor(Point p){return getColor(p.x, p.y);}

void Image::setR(unsigned int x, unsigned int y, unsigned char r){
    pixels[calculateIndex(x, y)] = r;
}
void Image::setG(unsigned int x, unsigned int y, unsigned char g){
    pixels[calculateIndex(x, y) + 1] = g;
}
void Image::setB(unsigned int x, unsigned int y, unsigned char b){
    pixels[calculateIndex(x, y) + 2] = b;
}
void Image::setRGB(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b){
    unsigned int index = calculateIndex(x, y);
    pixels[index] = r;
    pixels[index + 1] = g;
    pixels[index + 2] = b;
}

void Image::setColor(unsigned int x, unsigned int y, Color color){
    setRGB(x, y, color.r, color.g, color.b);
}

void Image::setR(Point p, unsigned char r){setR(p.x, p.y, r);};
void Image::setG(Point p, unsigned char g){setG(p.x, p.y, g);};
void Image::setB(Point p, unsigned char b){setB(p.x, p.y, b);};
void Image::setRGB(Point p, unsigned char r, unsigned char g, unsigned char b){
    setRGB(p.x, p.y, r, g, b);
}
void Image::setColor(Point p, Color color){
    setColor(p.x, p.y, color);
}


void Image::read(std::string filename){
    int w, h, b;
    pixels = stbi_load(filename.c_str(), &w, &h, &b, 0);
    width = w;
    height = h;
    bpp = b;
}
void Image::write(std::string filename){
    stbi_write_png(filename.c_str(), width, height, bpp, pixels, 0);
}

bool Image::next(int &x, int &y){
    x++;
    if(x >= width){
        x = 0;
        y++;
    }
    if(y >= height)return false;
    return true;
}

bool Image::ok(int x, int y){
    if(x < 0 || y < 0)return false;
    if(x >= width || y >= height)return false;
    return true;
}

bool Image::next(Point &p){
    return next(p.x, p.y);
}

bool Image::ok(Point p){
    return ok(p.x, p.y);
}