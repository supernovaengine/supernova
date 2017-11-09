#ifndef ProgramRender_h
#define ProgramRender_h

#include <string>
#include <unordered_map>

namespace Supernova {

    class ProgramRender {
        
    private:
        
        typedef std::unordered_map< std::string, std::shared_ptr<ProgramRender> >::iterator it_type;
        static std::unordered_map< std::string, std::shared_ptr<ProgramRender> > programsRender;
        
        bool loaded;
        
        static ProgramRender* getProgramRender();
        static ProgramRender::it_type findToRemove();
        
    protected:
        
        ProgramRender();
        
    public:
        
        virtual ~ProgramRender();
        
        static std::shared_ptr<ProgramRender> sharedInstance(std::string id);
        static void deleteUnused();
        
        bool isLoaded();

        virtual void createProgram(int shaderType, bool hasLight, bool hasFog, bool hasTextureCoords, bool hasTextureRect, bool hasTextureCube, bool isSky, bool isText, bool hasShadows);
        virtual void deleteProgram();
        
    };
    
}

#endif /* ProgramRender_h */
