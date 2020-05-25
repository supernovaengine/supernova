//
// Inspired by work of Jari Komppa in SoLoud audio engine
// https://sol.gfxile.net/soloud/file.html
// Modified by (c) 2020 Eduardo Doria.
//

#include "FileData.h"
#include "Data.h"
#include "system/System.h"
#include <stack>

using namespace Supernova;

FileData::~FileData(){

}

FileData* FileData::newFile(bool useHandle){
    if (!useHandle)
        return new Data();
    else
        return new File();
}

FileData* FileData::newFile(const char *aFilename, bool useHandle){
    if (!useHandle)
        return new Data(aFilename);
    else
        return new File(aFilename);
}

unsigned int FileData::read8(){
    unsigned char d = 0;
    read((unsigned char*)&d, 1);
    return d;
}

unsigned int FileData::read16(){
    unsigned short d = 0;
    read((unsigned char*)&d, 2);
    return d;
}

unsigned int FileData::read32(){
    unsigned int d = 0;
    read((unsigned char*)&d, 4);
    return d;
}

char FileData::getDirSeparator(){
#if defined(_WIN32)
    return '\\';
#else
    return '/';
#endif
}

bool FileData::beginWith(std::string path, std::string prefix){
    if (prefix.length() > path.length()) {
        return false;
    }

    if (prefix.length() == 0) {
        return true;
    }

    int i;
    for (i = 0; i < prefix.length(); i++) {
        if (path[i] != prefix[i]) {
            return false;
        }
    }

    return i == prefix.length();
}

std::string FileData::getBaseDir(std::string filepath){
    size_t found;

    found=filepath.find_last_of(getDirSeparator());

    std::string result = filepath.substr(0,found);

    if (filepath == result)
        result= "";

    return result + getDirSeparator();
}

std::string FileData::simplifyPath(std::string path) {

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

std::string FileData::getFilePathExtension(const std::string &filepath) {
    if (filepath.find_last_of(".") != std::string::npos)
        return filepath.substr(filepath.find_last_of(".") + 1);
    return "";
}

std::string FileData::getSystemPath(std::string path){
    if (beginWith(path, "data://")){
        path = path.substr(7, path.length());
        return System::instance()->getUserDataPath() + "/" + FileData::simplifyPath(path);
    }
    if (beginWith(path, "asset://")){
        path = path.substr(8, path.length());
        return System::instance()->getAssetPath() + "/" + FileData::simplifyPath(path);
    }
    if (beginWith(path, "/")){
        path = path.substr(1, path.length());
        return System::instance()->getAssetPath() + "/" + FileData::simplifyPath(path);
    }
    return System::instance()->getAssetPath() + "/" + FileData::simplifyPath(path);
}

std::string FileData::readString(int aOffset){
    unsigned int stringlen = length();
    std::string s( stringlen, '\0' );
    
    seek(aOffset);
    read((unsigned char*)&s[0], stringlen);

    return s;
}

unsigned int FileData::writeString(std::string s){
    return write((unsigned char*)&s[0], s.length());
}
