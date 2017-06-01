#include "File.h"

File::~File(){

}

unsigned int File::read8(){
    
    unsigned char d = 0;
    read((unsigned char*)&d, 1);
    return d;
    
}

unsigned int File::read16(){
    
    unsigned short d = 0;
    read((unsigned char*)&d, 2);
    return d;
    
}

unsigned int File::read32(){
    
    unsigned int d = 0;
    read((unsigned char*)&d, 4);
    return d;
    
}

std::string File::readString(){
    
    unsigned int stringlen = length();
    std::string s( stringlen, '\0' );
    
    seek(0);
    read((unsigned char*)&s[0], stringlen);

    return s;
    
}
