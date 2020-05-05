#include "image.hpp"

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
                dst.setG(x, y, x % 256);
            }
            else{
                dst.setRGB(x, y, 255 - color.r, 255 - color.g, 255 - color.b);
            }
        }
    }
    dst.write(output);
}

void controller(std::string input, std::string output){
    negaposi(input, output);
}

int main(int argc, char* argv[]){

    if(argc == 3){
        controller(std::string(argv[1]), std::string(argv[2]));
    }
    else{
        controller("./test_input.png", "./test_output.png");
    }

    return 0;
}