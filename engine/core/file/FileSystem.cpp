#include "FileSystem.h"

FileSystem::~FileSystem(){

}

unsigned int FileSystem::read8()
{
    unsigned char d = 0;
    read((unsigned char*)&d, 1);
    return d;
}

unsigned int FileSystem::read16()
{
    unsigned short d = 0;
    read((unsigned char*)&d, 2);
    return d;
}

unsigned int FileSystem::read32()
{
    unsigned int d = 0;
    read((unsigned char*)&d, 4);
    return d;
}