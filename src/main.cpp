// Kevin Lin COP3503 Project 2
#include <iostream>
#include <string>
#include <fstream>
#include <ostream>
#include <vector>
#include <algorithm>
using namespace std;

// the header storing information regarding the image
struct Header{
    char idLength, colorMapType, dataTypeCode;
    short colorMapOrigin, colorMapLength;
    char colorMapDepth, bitsPerPixel, imageDescriptor;
    short xOrigin, yOrigin, width, height;
};

// the individual pieces that make up the image
struct Pixel{
    // 0 - 255, byte size 1
    unsigned char red, green, blue;
};

// the big pieces that make up the image
struct Image{
    Header theHeader;
    vector<Pixel> Pixels;
};

// reads the data from a tga pile within a relative path
Image ReadData(const string& path){
    ifstream Data(path, ios_base::binary);
    Image theImage{};

    // information from the header regarding the image
    Data.read(&theImage.theHeader.idLength, sizeof(theImage.theHeader.idLength));
    Data.read(&theImage.theHeader.colorMapType, sizeof(theImage.theHeader.colorMapType));
    Data.read(&theImage.theHeader.dataTypeCode, sizeof(theImage.theHeader.dataTypeCode));
    Data.read((char*)&theImage.theHeader.colorMapOrigin, sizeof(theImage.theHeader.colorMapOrigin));
    Data.read((char*)&theImage.theHeader.colorMapLength, sizeof(theImage.theHeader.colorMapLength));
    Data.read(&theImage.theHeader.colorMapDepth, sizeof(theImage.theHeader.colorMapDepth));
    Data.read((char*)&theImage.theHeader.xOrigin, sizeof(theImage.theHeader.xOrigin));
    Data.read((char*)&theImage.theHeader.yOrigin, sizeof(theImage.theHeader.yOrigin));
    Data.read((char*)&theImage.theHeader.width, sizeof(theImage.theHeader.width));
    Data.read((char*)&theImage.theHeader.height, sizeof(theImage.theHeader.height));
    Data.read(&theImage.theHeader.bitsPerPixel, sizeof(theImage.theHeader.bitsPerPixel));
    Data.read(&theImage.theHeader.imageDescriptor, sizeof(theImage.theHeader.imageDescriptor));

    // deals with the pixels
    for(int i = 0; i < theImage.theHeader.width * theImage.theHeader.height; i++){
        Pixel newPixel{};
        Data.read((char*)&newPixel.blue, sizeof(newPixel.blue));
        Data.read((char*)&newPixel.green, sizeof(newPixel.green));
        Data.read((char*)&newPixel.red, sizeof(newPixel.red));
        theImage.Pixels.push_back(newPixel);
    }
    Data.close();
    return theImage;
}

// writes the data from the memory to a file within a relative path
void WriteData(const Image& headerObject, const string& testNumber){
    ofstream Data("output/part" + testNumber +".tga", ios_base::binary);

    // write out the header's data
    Data.write(&headerObject.theHeader.idLength, sizeof(headerObject.theHeader.idLength));
    Data.write(&headerObject.theHeader.colorMapType, sizeof(headerObject.theHeader.colorMapType));
    Data.write(&headerObject.theHeader.dataTypeCode, sizeof(headerObject.theHeader.dataTypeCode));
    Data.write((char*)&headerObject.theHeader.colorMapOrigin, sizeof(headerObject.theHeader.colorMapOrigin));
    Data.write((char*)&headerObject.theHeader.colorMapLength, sizeof(headerObject.theHeader.colorMapLength));
    Data.write(&headerObject.theHeader.colorMapDepth, sizeof(headerObject.theHeader.colorMapDepth));
    Data.write((char*)&headerObject.theHeader.xOrigin, sizeof(headerObject.theHeader.xOrigin));
    Data.write((char*)&headerObject.theHeader.yOrigin, sizeof(headerObject.theHeader.yOrigin));
    Data.write((char*)&headerObject.theHeader.width, sizeof(headerObject.theHeader.width));
    Data.write((char*)&headerObject.theHeader.height, sizeof(headerObject.theHeader.height));
    Data.write(&headerObject.theHeader.bitsPerPixel, sizeof(headerObject.theHeader.bitsPerPixel));
    Data.write(&headerObject.theHeader.imageDescriptor, sizeof(headerObject.theHeader.imageDescriptor));

    // write out Pixel's data (3 bytes), each color is a byte
    for(auto & Pixel : headerObject.Pixels){
        Data.write((char*)&Pixel.blue, sizeof(Pixel.blue));
        Data.write((char*)&Pixel.green, sizeof(Pixel.green));
        Data.write((char*)&Pixel.red, sizeof(Pixel.red));
    }
    Data.close();
}

// combines two images by multiplying their colors at each pixel
Image MultiplyMode(const Image& imgOne, const Image& imgTwo){
    Image NewImage{};
    // headers are similar for the copying
    NewImage.theHeader = imgOne.theHeader;

    // calculate the new pixels
    for(int i = 0; i < imgOne.Pixels.size(); i++){
        // beware of small precision errors, work with floats and then cast to char
        NewImage.Pixels.at(i).blue = ((imgOne.Pixels.at(i).blue * imgTwo.Pixels.at(i).blue) / 255.0f) + 0.5f;
        NewImage.Pixels.at(i).green = ((imgOne.Pixels.at(i).green * imgTwo.Pixels.at(i).green) / 255.0f) + 0.5f;
        NewImage.Pixels.at(i).red = ((imgOne.Pixels.at(i).red * imgTwo.Pixels.at(i).red) / 255.0f) + 0.5;

        //cap the colors if it exceeds 255
        if(NewImage.Pixels.at(i).blue > 255)
            NewImage.Pixels.at(i).blue = 255;
        if(NewImage.Pixels.at(i).green > 255)
            NewImage.Pixels.at(i).green = 255;
        if(NewImage.Pixels.at(i).red > 255)
            NewImage.Pixels.at(i).red = 255;
    }
    return NewImage;
}

// combines two images by subtracting one image's pixel's color by
// the corresponding image's pixel's color for every pixel
Image SubtractMode(const Image& imgOne, const Image& imgTwo){
    Image NewImage{};
    // headers will be similar
    NewImage.theHeader = imgTwo.theHeader;

    // calculate new pixels and clamp the colors if it drops below 0
    for(int i = 0; i < imgOne.Pixels.size(); i++){
        // subtracts the top layer (imgOne) from bottom layer(imgTwo)
        int blue = (int)imgTwo.Pixels.at(i).blue - (int)imgOne.Pixels.at(i).blue;
        int green = (int)imgTwo.Pixels.at(i).green - (int)imgOne.Pixels.at(i).green;
        int red = (int)imgTwo.Pixels.at(i).red - (int)imgOne.Pixels.at(i).red;

        if(blue < 0)
            blue = 0;
        if(green < 0)
            green = 0;
        if(red < 0)
            red = 0;
        NewImage.Pixels.at(i).blue = (char)blue;
        NewImage.Pixels.at(i).green = (char)green;
        NewImage.Pixels.at(i).red = (char)red;
    }

    return NewImage;
}

// combines two images using formula C = 1 - (1 - A)*(1 - B)
Image ScreenMode(const Image& imgOne, const Image& imgTwo){
    Image newImage{};
    newImage.theHeader = imgOne.theHeader;

    for(int i = 0; i < imgOne.Pixels.size(); i++){
        newImage.Pixels.at(i).blue = ((1 - (1 - (imgOne.Pixels.at(i).blue / 255.0f) ) * (1 - (imgTwo.Pixels.at(i).blue / 255.0f) )) * 255) + 0.5f;
        newImage.Pixels.at(i).green = ((1 - (1 - (imgOne.Pixels.at(i).green / 255.0f) ) * (1 - (imgTwo.Pixels.at(i).green / 255.0f) )) * 255) + 0.5f;
        newImage.Pixels.at(i).red = ((1 - (1 - (imgOne.Pixels.at(i).red / 255.0f) ) * (1 - (imgTwo.Pixels.at(i).red / 255.0f) )) * 255) + 0.5f;

        if(newImage.Pixels.at(i).blue > 255)
            newImage.Pixels.at(i).blue = 255;
        if(newImage.Pixels.at(i).green > 255)
            newImage.Pixels.at(i).green = 255;
        if(newImage.Pixels.at(i).red > 255)
            newImage.Pixels.at(i).red = 255;
    }

    return newImage;
}

// Overlay Blending:
Image OverlayMode(const Image& imgOne, const Image& imgTwo){
                //      top layer    , bottom layer
    Image newImage{};
    newImage.theHeader = imgOne.theHeader;

    vector<Pixel> newPixels;
    for(int i = 0; i < imgOne.Pixels.size(); i++){
        Pixel pix{};

        // check if the pixel is grey, with <= 0.5 being gray, > 0.5 being closer to white
        // C = (2*A*B) if B <= 0.5, otherwise do this screen, C = 1-2(1-A)(1-B) if B > 0.5
        if( (imgTwo.Pixels.at(i).blue / 255.0f) <= 0.5)
            pix.blue = (2 * imgOne.Pixels.at(i).blue * imgTwo.Pixels.at(i).blue / 255.0f) + 0.5f;
        else
            pix.blue = (1 - 2*(1-(imgOne.Pixels.at(i).blue/255.0f))*(1-(imgTwo.Pixels.at(i).blue/255.0f))) * 255 + 0.5f;
        if( (imgTwo.Pixels.at(i).green / 255.0f) <= 0.5)
            pix.green = (2 * imgOne.Pixels.at(i).green * imgTwo.Pixels.at(i).green / 255.0f) + 0.5f;
        else
            pix.green = (1 - 2*(1-(imgOne.Pixels.at(i).green/255.0f))*(1-(imgTwo.Pixels.at(i).green/255.0f))) * 255 + 0.5f;
        if( (imgTwo.Pixels.at(i).red / 255.0f) <= 0.5)
            pix.red = (2 * imgOne.Pixels.at(i).red * imgTwo.Pixels.at(i).red / 255.0f) + 0.5f;
        else
            pix.red = (1 - 2*(1-(imgOne.Pixels.at(i).red/255.0f))*(1-(imgTwo.Pixels.at(i).red/255.0f))) * 255 + 0.5f;

        // prevent overflow
        if(pix.blue > 255)
            pix.blue = 255;
        if(pix.green > 255)
            pix.green = 255;
        if(pix.red > 255)
            pix.red = 255;

        newPixels.push_back(pix);
    }

    newImage.Pixels = newPixels;
    return newImage;
}

// checks if image is equal to the example output
void PrintIfEqual(const Image& output, const string& example, int testNumber){
    Image theExample = ReadData(example);
    string condition = "Passed!";

    // check if their header is equal, every byte
    if((output.theHeader.height != theExample.theHeader.height) ||
        output.theHeader.width  != theExample.theHeader.width)
        condition = "Failed!";
    if((output.theHeader.idLength != theExample.theHeader.idLength) ||
       output.theHeader.colorMapType  != theExample.theHeader.colorMapType)
        condition = "Failed!";
    if((output.theHeader.dataTypeCode != theExample.theHeader.dataTypeCode) ||
       output.theHeader.colorMapOrigin  != theExample.theHeader.colorMapType)
        condition = "Failed!";
    if((output.theHeader.colorMapLength != theExample.theHeader.colorMapLength) ||
       output.theHeader.colorMapDepth  != theExample.theHeader.colorMapDepth)
        condition = "Failed!";
    if((output.theHeader.xOrigin != theExample.theHeader.xOrigin) ||
       output.theHeader.yOrigin  != theExample.theHeader.yOrigin)
        condition = "Failed!";
    if((output.theHeader.bitsPerPixel != theExample.theHeader.bitsPerPixel) ||
       output.theHeader.imageDescriptor  != theExample.theHeader.imageDescriptor)
        condition = "Failed!";

    for(int i = 0; i < output.Pixels.size(); i++){
        if(output.Pixels.at(i).blue != theExample.Pixels.at(i).blue)
            condition = "Failed!";
        if(output.Pixels.at(i).green != theExample.Pixels.at(i).green)
            condition = "Failed!";
        if(output.Pixels.at(i).red != theExample.Pixels.at(i).red)
            condition = "Failed!";
    }
    cout << "Test #" << testNumber << "......" << condition << endl;
}

// modifies the passed in parameter Image by changing it to a green tint
void SetGreen(Image& theImage, const int amount){
    for(int i = 0; i < theImage.Pixels.size(); i++) {
        int green = (int)theImage.Pixels.at(i).green + amount;

        if(green > 255)
            green = 255;
        theImage.Pixels.at(i).green = (char)green;
    }
}

// modifies the parameter Image by changing the intensity
void ModifyIntensity(Image& theImage, const string& color, int factor){
    // all other channel's colors will become color of the passed in parameter
    for(auto & Pixel : theImage.Pixels){
        if(color == "blue") {
            int blue = Pixel.blue * factor + 0.5f;
            if(blue > 255)
                blue = 255;
            Pixel.blue = (char)blue;
        }
        else if(color == "red") {
            int red = Pixel.red * factor + 0.5f;
            if(red > 255)
                red = 255;
            Pixel.red = (char)red;
        }
        else if(color == "green") {
            int green = Pixel.green * factor + 0.5f;
            if(green > 255)
                green = 255;
            Pixel.green = (char)green;
        }
    }
}

// write file based on channel color
void LeaveOnlyChannel(Image& headerObject, const string& color){
    // write out Pixel's data (3 bytes), each color is a byte
    for(auto & Pixel : headerObject.Pixels){
        if(color == "r") {
            Pixel.blue = Pixel.red;
            Pixel.green = Pixel.red;
        }
        if(color == "g") {
            Pixel.blue = Pixel.green;
            Pixel.red = Pixel.green;
        }
        if(color == "b"){
            Pixel.red = Pixel.blue;
            Pixel.green = Pixel.blue;
        }
    }
}

// combines different channels of the same image
Image CombineChannels(const Image& B, const Image& G, const Image& R){
    Image newImage{};
    newImage.theHeader = R.theHeader;

    for(int i = 0; i < R.Pixels.size(); i++){
        Pixel newPix{};
        newPix.blue = B.Pixels.at(i).blue;
        newPix.green = G.Pixels.at(i).green;
        newPix.red = R.Pixels.at(i).red;
        newImage.Pixels.push_back(newPix);
    }
    return newImage;
}

// rotates 180 degrees
void FlipImage(Image& theImage){
    reverse(theImage.Pixels.begin(), theImage.Pixels.end());
}

// for the extra credit, combines four images into a collage, their dimensions remain the same
void WritePhotoCollage(Image& topOne,Image& NewImage, const Image& botOne, const Image& botTwo) {
  NewImage.theHeader.height *= 2, NewImage.theHeader.width *= 2;

  ofstream Collage("output/extracredit.tga", ios_base::binary);
  // write the header
    Collage.write(&NewImage.theHeader.idLength, sizeof(NewImage.theHeader.idLength));
    Collage.write(&NewImage.theHeader.colorMapType, sizeof(NewImage.theHeader.colorMapType));
    Collage.write(&NewImage.theHeader.dataTypeCode, sizeof(NewImage.theHeader.dataTypeCode));
    Collage.write((char*)&NewImage.theHeader.colorMapOrigin, sizeof(NewImage.theHeader.colorMapOrigin));
    Collage.write((char*)&NewImage.theHeader.colorMapLength, sizeof(NewImage.theHeader.colorMapLength));
    Collage.write(&NewImage.theHeader.colorMapDepth, sizeof(NewImage.theHeader.colorMapDepth));
    Collage.write((char*)&NewImage.theHeader.xOrigin, sizeof(NewImage.theHeader.xOrigin));
    Collage.write((char*)&NewImage.theHeader.yOrigin, sizeof(NewImage.theHeader.yOrigin));
    Collage.write((char*)&NewImage.theHeader.width, sizeof(NewImage.theHeader.width));
    Collage.write((char*)&NewImage.theHeader.height, sizeof(NewImage.theHeader.height));
    Collage.write(&NewImage.theHeader.bitsPerPixel, sizeof(NewImage.theHeader.bitsPerPixel));
    Collage.write(&NewImage.theHeader.imageDescriptor, sizeof(NewImage.theHeader.imageDescriptor));

  // write the pixels row by column, combined image is 1024 x 1024
  // this is the bottom half
    for(int i = 0; i < 512; i++){
        // each image is 512 x 512
        // 512 pixels per image row, each pixel is 3 bytes, so 1536 bytes per image row
        // each image row has 512 columns
        for(int j = 0 ; j < 512; j++) {
            Collage.write((char *) &topOne.Pixels.at(j + (512 * i)).blue, sizeof(topOne.Pixels.at(j+ (512 * i)).blue));
            Collage.write((char *) &topOne.Pixels.at(j+ (512 * i)).green, sizeof(topOne.Pixels.at(j+ (512 * i)).green));
            Collage.write((char *) &topOne.Pixels.at(j+ (512 * i)).red, sizeof(topOne.Pixels.at(j+ (512 * i)).red));
        }
        // image NewImage aka topTwo because lazyness
        for(int z = 0; z < 512; z++){
            Collage.write((char *) &NewImage.Pixels.at(z+ (512 * i)).blue, sizeof(NewImage.Pixels.at(z+ (512 * i)).blue));
            Collage.write((char *) &NewImage.Pixels.at(z+ (512 * i)).green, sizeof(NewImage.Pixels.at(z+ (512 * i)).green));
            Collage.write((char *) &NewImage.Pixels.at(z+ (512 * i)).red, sizeof(NewImage.Pixels.at(z+ (512 * i)).red));
        }
    }

    // this is the top half
    for(int i = 0; i < 512; i++){
        for(int j = 0 ; j < 512; j++) {
            Collage.write((char *) &botOne.Pixels.at(j+ (512 * i)).blue, sizeof(botOne.Pixels.at(j+ (512 * i)).blue));
            Collage.write((char *) &botOne.Pixels.at(j+ (512 * i)).green, sizeof(botOne.Pixels.at(j+ (512 * i)).green));
            Collage.write((char *) &botOne.Pixels.at(j+ (512 * i)).red, sizeof(botOne.Pixels.at(j+ (512 * i)).red));
        }
        for(int z = 0; z < 512; z++){
            Collage.write((char *) &botTwo.Pixels.at(z+ (512 * i)).blue, sizeof(botTwo.Pixels.at(z+ (512 * i)).blue));
            Collage.write((char *) &botTwo.Pixels.at(z+ (512 * i)).green, sizeof(botTwo.Pixels.at(z+ (512 * i)).green));
            Collage.write((char *) &botTwo.Pixels.at(z+ (512 * i)).red, sizeof(botTwo.Pixels.at(z+ (512 * i)).red));
        }
    }

    Collage.close();
}

void PartOne(){
    Image layer1 = ReadData("input/layer1.tga");
    Image pattern1 = ReadData("input/pattern1.tga");
    Image part1 = MultiplyMode(layer1, pattern1);
    WriteData(part1, "1");
    PrintIfEqual(part1, "examples/EXAMPLE_part1.tga", 1);
};
void PartTwo(){
    Image layer2 = ReadData("input/layer2.tga");
    Image car = ReadData("input/car.tga");
    Image part2 = SubtractMode(layer2, car);
    WriteData(part2, "2");
    PrintIfEqual(part2, "examples/EXAMPLE_part2.tga", 2);
}
void PartThree(){
    Image layer1 = ReadData("input/layer1.tga");
    Image pattern2 = ReadData("input/pattern2.tga");
    Image TempPart3 = MultiplyMode(pattern2, layer1);
    Image text = ReadData("input/text.tga");
    Image part3 = ScreenMode(TempPart3, text);
    WriteData(part3, "3");
    PrintIfEqual(part3, "examples/EXAMPLE_part3.tga", 3);
}
void PartFour(){
    Image layer2 = ReadData("input/layer2.tga");
    Image pattern2 = ReadData("input/pattern2.tga");
    Image circles = ReadData("input/circles.tga");
    Image tempPart4 = MultiplyMode(layer2, circles);
    Image part4 = SubtractMode(pattern2, tempPart4);
    WriteData(part4, "4");
    PrintIfEqual(part4, "examples/EXAMPLE_part4.tga", 4);
}
void PartFive(){
    Image layer1 = ReadData("input/layer1.tga");
    Image pattern1 = ReadData("input/pattern1.tga");
    Image part5 = OverlayMode(layer1, pattern1);
    WriteData(part5,"5");
    PrintIfEqual(part5, "examples/EXAMPLE_part5.tga",5);
}
void PartSix(){
    Image car_part6 = ReadData("input/car.tga");
    SetGreen(car_part6, 200);
    WriteData(car_part6, "6");
    PrintIfEqual(car_part6, "examples/EXAMPLE_part6.tga", 6);
}
void PartSeven(){
    Image car_part7 = ReadData("input/car.tga");
    ModifyIntensity(car_part7, "red", 4);
    ModifyIntensity(car_part7, "blue", 0);
    WriteData(car_part7,"7");
    PrintIfEqual(car_part7, "examples/EXAMPLE_part7.tga",7);
}
void PartEight(){
    // blue - 8, green - 9, red - 10
    // blue channel
    Image car_part8_b = ReadData("input/car.tga");
    LeaveOnlyChannel(car_part8_b, "b");
    WriteData(car_part8_b, "8_b");
    PrintIfEqual(car_part8_b, "examples/EXAMPLE_part8_b.tga", 9);
    // green channel
    Image car_part8_g = ReadData("input/car.tga");
    LeaveOnlyChannel(car_part8_g, "g");
    WriteData(car_part8_g, "8_g");
    PrintIfEqual(car_part8_g, "examples/EXAMPLE_part8_g.tga", 10);
    // red channel
    Image car_part8_r = ReadData("input/car.tga");
    LeaveOnlyChannel(car_part8_r, "r");
    WriteData(car_part8_r, "8_r");
    PrintIfEqual(car_part8_r, "examples/EXAMPLE_part8_r.tga", 11);
}
void PartNine(){
    Image layerR = ReadData("input/layer_red.tga");
    Image layerG = ReadData("input/layer_green.tga");
    Image layerB = ReadData("input/layer_blue.tga");
    Image part9 = CombineChannels(layerB, layerG, layerR);
    WriteData(part9, "9");
    PrintIfEqual(part9, "examples/EXAMPLE_part9.tga", 12);
}
void PartTen(){
    Image text2 = ReadData("input/text2.tga");
    FlipImage(text2);
    WriteData(text2, "10");
    PrintIfEqual(text2, "examples/EXAMPLE_part10.tga", 13);
}
void ExtraCredit(){
    Image ec_car = ReadData("input/car.tga");
    Image ec_text = ReadData("input/text.tga");
    Image ec_pattern = ReadData("input/pattern1.tga");
    Image ec_circle = ReadData("input/circles.tga");
    try{
        WritePhotoCollage(ec_text,ec_pattern,ec_car,ec_circle);
    }
    catch(out_of_range){}
    Image example_ec = ReadData("examples/EXAMPLE_extracredit.tga");
    cout << "EC: ";
    PrintIfEqual(example_ec, "output/extracredit.tga", 14);
}

int main() {

    // PART 1 - MULTIPLICATION
    PartOne();
    // PART 2 - SUBTRACTION
    PartTwo();
    // PART 3 - SCREEN BLENDING
    PartThree();
    // PART 4 - SUBTRACT BLENDING
    PartFour();
    // PART 5 - OVERLAY BLENDING
    PartFive();
    // PART 6 - Add 200 to green channel of car.tga
    PartSix();
    // PART 7 - Multiply channel by color to modify color intensity
    PartSeven();
    // PART 8 - Load and separate each channel into a separate file
    PartEight();
    // PART 9 - combine the three files into one file
    PartNine();
    // PART 10 - Rotate 180 degrees
    PartTen();
    // EXTRA CREDIT
    ExtraCredit();

    return 0;
}
