#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image_write.h"
#include <string>

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
    Color getColor(unsigned int x, unsigned int y);
    void setR(unsigned int x, unsigned int y, unsigned char r);
    void setG(unsigned int x, unsigned int y, unsigned char g);
    void setB(unsigned int x, unsigned int y, unsigned char b);
    void setRGB(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b);
    void setColor(unsigned int x, unsigned int y, Color color);
    void read(std::string filename);
    void write(std::string filename);
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