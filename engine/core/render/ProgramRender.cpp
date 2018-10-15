#include "ProgramRender.h"

#include "gles2/GLES2Program.h"
#include "Engine.h"
#include "Scene.h"

using namespace Supernova;

std::unordered_map< std::string, std::shared_ptr<ProgramRender> > ProgramRender::programsRender;


ProgramRender::ProgramRender(){
    this->loaded = false;
}

ProgramRender::~ProgramRender(){
    
}

std::shared_ptr<ProgramRender> ProgramRender::sharedInstance(std::string id){
    if (programsRender.count(id) > 0){
        return programsRender[id];
    }else{
        
        if (Engine::getRenderAPI() == S_GLES2){
            programsRender[id] = std::shared_ptr<ProgramRender>(new GLES2Program());
            return programsRender[id];
        }
    }
    
    return NULL;
}

bool ProgramRender::isLoaded(){
    return loaded;
}

void ProgramRender::deleteUnused(){
    
    ProgramRender::it_type remove = findToRemove();
    while (remove != programsRender.end()){
        programsRender.erase(remove);
        remove = findToRemove();
    }
    
}

ProgramRender::it_type ProgramRender::findToRemove(){
    
    for(ProgramRender::it_type iterator = programsRender.begin(); iterator != programsRender.end(); iterator++) {
        if (iterator->second.use_count() <= 1){
            if (iterator->second.get() != NULL)
                iterator->second.get()->deleteProgram();
            return iterator;
        }
    }
    
    return programsRender.end();
}

std::string ProgramRender::regexReplace(std::string_view haystack, const std::regex& rx, std::function<std::string(const std::cmatch&)> f) {
    std::string result;
    const char *begin = haystack.data();
    const char *end = begin + haystack.size();
    std::cmatch m, lastm;
    if (!std::regex_search(begin, end, m, rx)) {
        return std::string(haystack);
    }
    do {
        lastm = m;
        result.append(m.prefix());
        result.append(f(m));
        begin = m[0].second;
        begin += (begin != end && m[0].length() == 0);
        if (begin == end) break;
    } while (std::regex_search(begin, end, m, rx,
                               std::regex_constants::match_prev_avail));
    result.append(lastm.suffix());
    return result;
}

std::string ProgramRender::replaceAll(std::string source, const std::string from, const std::string to){
    std::string::size_type n = 0;
    while ( ( n = source.find( from, n ) ) != std::string::npos )
    {
        source.replace( n, from.size(), to );
        n += to.size();
    }
    return source;
}

std::string ProgramRender::unrollLoops(std::string source){
    // "[^\x00]+" is to get anything that is not the null character
    return regexReplace(source, std::regex{"[^ ]*#pragma unroll_loop[\\s]+?for\\s*\\(\\s*[a-z]*\\s*([a-z])\\s*\\=\\s*(\\d+)s*;\\s*([a-z])\\s*\\<\\s*(\\d+)s*;\\s*([a-z])\\s*\\+\\+\\s*\\)s*\\{([^\\x00]+?)(?=\\})\\}"},
                                   [](std::cmatch m)->std::string {

                                       int from = std::stoi(m.str(2));
                                       int to = std::stoi(m.str(4));

                                       std::regex reg1 ("\\[\\s*j\\s*\\]");
                                       std::regex reg2 ("\\(\\s*j\\s*\\)");
                                       std::string result;

                                       for (int i = from; i < to; i++){
                                           std::string body = m.str(6);
                                           body = std::regex_replace (body,reg1,"["+std::to_string(i)+"]");
                                           body = std::regex_replace (body,reg2,"("+std::to_string(i)+")");
                                           result = result + body;
                                       }

                                       return result;
                                   });
}

int ProgramRender::getMaxLights(){
    return maxLights;
}

int ProgramRender::getMaxShadows2D(){
    return maxShadows2D;
}

int ProgramRender::getMaxShadowsCube(){
    return maxShadowsCube;
}

void ProgramRender::createProgram(int shaderType, int numLights, int numShadows2D, int numShadowsCube, bool hasFog, bool hasTextureCoords, bool hasTextureRect, bool hasTextureCube, bool hasSkinning, bool isSky, bool isText){
    loaded = true;
}

void ProgramRender::deleteProgram(){
    loaded = false;
}
