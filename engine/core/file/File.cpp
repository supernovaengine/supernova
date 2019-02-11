#include "File.h"
#include "FileData.h"
#include <stack>

using namespace Supernova;

File::~File(){

}

File* File::newFile(bool useHandle){
    if (!useHandle)
        return new FileData();
    else
        return new FileHandle();
}

File* File::newFile(const char *aFilename, bool useHandle){
    if (!useHandle)
        return new FileData(aFilename);
    else
        return new FileHandle(aFilename);
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

char File::getDirSeparator(){
#if defined(_WIN32)
    return '\\';
#else
    return '/';
#endif
}

std::string File::getBaseDir(std::string filepath){
    size_t found;

    found=filepath.find_last_of(getDirSeparator());

    std::string result = filepath.substr(0,found);

    if (filepath == result)
        result= "";

    return result + getDirSeparator();
}

std::string File::simplifyPath(std::string path) {

    std::stack<std::string> st;

    std::string dir;

    std::string res;

    int len_path = path.length();

    for (int i = 0; i < len_path; i++) {

        dir.clear();

        while (path[i] == getDirSeparator())
            i++;

        while (i < len_path && path[i] != getDirSeparator()) {
            dir.push_back(path[i]);
            i++;
        }

        if (dir.compare("..") == 0) {
            if (!st.empty())
                st.pop();
        }
        else if (dir.compare(".") == 0)
            continue;
        else if (dir.length() != 0)
            st.push(dir);
    }

    std::stack<std::string> st1;
    while (!st.empty()) {
        st1.push(st.top());
        st.pop();
    }

    while (!st1.empty()) {
        std::string temp = st1.top();
        if (st1.size() != 1)
            res.append(temp + getDirSeparator());
        else
            res.append(temp);

        st1.pop();
    }

    return res;
}

std::string File::readString(){
    unsigned int stringlen = length();
    std::string s( stringlen, '\0' );
    
    seek(0);
    read((unsigned char*)&s[0], stringlen);

    return s;
}

std::string File::getFilePathExtension(const std::string &filepath) {
    if (filepath.find_last_of(".") != std::string::npos)
        return filepath.substr(filepath.find_last_of(".") + 1);
    return "";
}
