#include "Texture.h"
#include "Engine.h"


using namespace Supernova;

Texture::Texture(){
    this->textureRender = NULL;

    this->textureFrameWidth = 0;
    this->textureFrameHeight = 0;

    this->paths.clear();
    this->texturesData.clear();
    this->type = S_TEXTURE_2D;
    
    this->id = "";
    
    this->dataOwned = false;
    this->preserveData = false;

    this->resampleToPowerOfTwo = false;
    this->nearestScale = false;

    this->userResampleToPowerOfTwo = false;
    this->userNearestScale = false;
}

Texture::Texture(std::string filename): Texture(){
    this->paths.push_back(filename);
    this->id = filename;
}

Texture::Texture(std::vector<std::string> paths, std::string id): Texture(){
    this->paths = paths;
    this->id = id;
}

Texture::Texture(TextureData* textureData, std::string id): Texture(){
    this->texturesData.push_back(textureData);
    this->id = id;
}

Texture::Texture(std::vector<TextureData*> texturesData, std::string id): Texture(){
    this->texturesData = texturesData;
    this->id = id;
}

Texture::Texture(int textureFrameWidth, int textureFrameHeight, std::string id): Texture(){
    this->type = S_TEXTURE_FRAME;
    this->textureFrameWidth = textureFrameWidth;
    this->textureFrameHeight = textureFrameHeight;
    this->id = id;
}

Texture::Texture(const Texture& t){
    this->textureRender = t.textureRender;
    this->paths = t.paths;
    this->texturesData = t.texturesData;
    this->type = t.type;
    this->id = t.id;
    this->dataOwned = t.dataOwned;
}

Texture& Texture::operator = (const Texture& t){
    this->textureRender = t.textureRender;
    this->paths = t.paths;
    this->texturesData = t.texturesData;
    this->type = t.type;
    this->id = t.id;
    this->dataOwned = t.dataOwned;

    return *this;
}

Texture::~Texture(){
    textureRender.reset();
    if (dataOwned){
        releaseData();
    }
}

bool Texture::isIdLoaded(std::string id){
    return TextureRender::sharedInstance(id).get()->isLoaded();
}

void Texture::setId(std::string id){
    this->id = id;
}

void Texture::setTextureData(TextureData* textureData){
    texturesData[0] = textureData;
}

void Texture::setType(int type){
    this->type = type;
}

void Texture::setDataOwned(bool dataOwned){
    this->dataOwned = dataOwned;
}

void Texture::setTextureFrameSize(int textureFrameWidth, int textureFrameHeight){
    this->textureFrameWidth = textureFrameWidth;
    this->textureFrameHeight = textureFrameHeight;
}

void Texture::setResampleToPowerOfTwo(bool resampleToPowerOfTwo){
    this->resampleToPowerOfTwo = resampleToPowerOfTwo;
    this->userResampleToPowerOfTwo = true;
}

void Texture::setNearestScale(bool nearestScale){
    this->nearestScale = nearestScale;
    this->userNearestScale = true;
}

void Texture::setDefaults(){
    if (!userResampleToPowerOfTwo)
        resampleToPowerOfTwo = Engine::isDefaultResampleToPOTTexture();

    if (!userNearestScale)
        nearestScale = Engine::isDefaultNearestScaleTexture();
}

bool Texture::load(){

    textureRender = TextureRender::sharedInstance(id);

    setDefaults();
    textureRender.get()->setNearestScale(nearestScale);
    
    bool renderNotPrepared = false;

    if (!textureRender.get()->isLoaded()){

        if (type == S_TEXTURE_2D){

            if (paths.size() > 0){
                texturesData.push_back(new TextureData());
                texturesData.back()->loadTextureFromFile(paths[0].c_str());
                if (!texturesData.back()->getData()) {
                    releaseData();
                    this->textureRender.reset();
                    TextureRender::deleteUnused();
                    return false;
                }
                dataOwned = true;
            }

            if (resampleToPowerOfTwo){
                texturesData[0]->resamplePowerOfTwo();
            }else{
                texturesData[0]->fitPowerOfTwo();
            }

            if (!textureRender.get()->loadTexture(texturesData[0])){
                renderNotPrepared = true;
            }
            
        }else if (type == S_TEXTURE_CUBE){

            if (paths.size() > 0) {
                for (int i = 0; i < paths.size(); i++) {
                    texturesData.push_back(new TextureData());
                    texturesData.back()->loadTextureFromFile(paths[i].c_str());
                    if (!texturesData.back()->getData()) {
                        releaseData();
                        this->textureRender.reset();
                        TextureRender::deleteUnused();
                        return false;
                    }
                    texturesData.back()->resamplePowerOfTwo();
                }
                dataOwned = true;
            }

            if (!textureRender.get()->loadTextureCube(texturesData)){
                renderNotPrepared = true;
            }

        }else if (type == S_TEXTURE_FRAME){
            
            if (!textureRender.get()->loadTextureFrame(textureFrameWidth, textureFrameHeight, false)){
                renderNotPrepared = true;
            }
            
        }else if (type == S_TEXTURE_DEPTH_FRAME){
            
            if (!textureRender.get()->loadTextureFrame(textureFrameWidth, textureFrameHeight, true)){
                renderNotPrepared = true;
            }
            
        }else if (type == S_TEXTURE_FRAME_CUBE){
            
            if (!textureRender.get()->loadTextureFrameCube(textureFrameWidth, textureFrameHeight)){
                renderNotPrepared = true;
            }
            
        }
        
        if (renderNotPrepared){
            this->textureRender.reset();
            TextureRender::deleteUnused();
            return false;
        }
        
        if (dataOwned && !preserveData){
            releaseData();
        }
        
    }
    
    return true;

}

void Texture::releaseData(){
    for (int i = 0; i < texturesData.size(); i++){
        delete texturesData[i];
    }
    this->texturesData.clear();
    this->texturesData.push_back(NULL);
}

std::string Texture::getId(){
    return id;
}

int Texture::getType(){
    return type;
}

bool Texture::getDataOwned(){
    return dataOwned;
}

int Texture::getTextureFrameWidth(){
    return textureFrameWidth;
}

int Texture::getTextureFrameHeight(){
    return textureFrameHeight;
}

bool Texture::getResampleToPowerOfTwo(){
    return this->resampleToPowerOfTwo;
}

bool Texture::getNearestScale(){
    return this->nearestScale;
}

TextureData* Texture::getTextureData(int index){
    return texturesData[index];
}

bool Texture::isPreserveData() const {
    return preserveData;
}

void Texture::setPreserveData(bool preserveData) {
    Texture::preserveData = preserveData;
}

std::shared_ptr<TextureRender> Texture::getTextureRender(){
    return textureRender;
}

int Texture::getColorFormat(){
    if (textureRender)
        return textureRender.get()->getColorFormat();
    
    return -1;
}

bool Texture::hasAlphaChannel(){
    
    int colorFormat = getColorFormat();

    if (colorFormat == S_COLOR_GRAY_ALPHA ||
        colorFormat == S_COLOR_RGB_ALPHA ||
        colorFormat == S_COLOR_ALPHA)
        return true;

    return false;
}

int Texture::getWidth(){
    if (textureRender)
        return textureRender.get()->getWidth();
    
    return -1;
}

int Texture::getHeight(){
    if (textureRender)
        return textureRender.get()->getHeight();
    
    return -1;
}

void Texture::destroy(){
    textureRender.reset();
    this->textureRender = NULL;
    TextureRender::deleteUnused();
}
