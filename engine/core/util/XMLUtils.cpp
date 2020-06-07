//
// (c) 2020 Eduardo Doria.
//

#include "XMLUtils.h"
#include "Log.h"
#include "io/File.h"
#include "io/FileData.h"

using namespace Supernova;

tinyxml2::XMLElement* XMLUtils::getNodeForKey(const char* XMLFilePath, const char* rootName, const char* key, tinyxml2::XMLElement** rootNode, tinyxml2::XMLDocument* doc){


    tinyxml2::XMLElement* curNode = NULL;

    *rootNode = NULL;

    File file;

    std::string xmlBuffer = "";
    if (file.open(XMLFilePath) == FileErrors::NO_ERROR)
        xmlBuffer = file.readString();

    if (!xmlBuffer.empty()) {
        doc->Parse(xmlBuffer.c_str(), xmlBuffer.size());
        *rootNode = doc->RootElement();
    }

    if (!(*rootNode)) {
        if (!doc->FirstChild()) {
            tinyxml2::XMLDeclaration *xmlDeclaration = doc->NewDeclaration(NULL);
            if (xmlDeclaration) {
                doc->LinkEndChild(xmlDeclaration);
            }
        }

        tinyxml2::XMLElement *rootElement = doc->NewElement(rootName);
        if (!rootElement)
            return curNode;

        doc->LinkEndChild(rootElement);
        *rootNode = rootElement;
    }

    curNode = (*rootNode)->FirstChildElement();
    while (curNode) {
        const char* nodeName = curNode->Value();
        if (!strcmp(nodeName, key)){
            return curNode;
        }

        curNode = curNode->NextSiblingElement();
    }

    return curNode;
}

const char* XMLUtils::getValueForKey(const char* XMLFilePath, const char* rootName, const char *key){
    if (!key) {
        Log::Error("Can't get value in XML (%s): Key is invalid", XMLFilePath);
        return NULL;
    }

    tinyxml2::XMLDocument doc;

    //They are deleted when doc is deleted
    tinyxml2::XMLElement* node;
    tinyxml2::XMLElement* rootNode;

    node = XMLUtils::getNodeForKey(XMLFilePath, rootName, key, &rootNode, &doc);
    if (node && node->FirstChild()){
        return (const char*)(node->FirstChild()->Value());
    }

    return NULL;
}

void XMLUtils::setValueForKey(const char* XMLFilePath, const char* rootName, const char* key, const char* value) {
    if (!key)
        Log::Error("Can't set value in XML (%s): Key is invalid", XMLFilePath);

    if (!value)
        Log::Error("Can't set value in XML (%s): Value is invalid", XMLFilePath);

    if (key && value) {
        tinyxml2::XMLDocument doc;

        //They are deleted when doc is deleted
        tinyxml2::XMLElement *rootNode;
        tinyxml2::XMLElement *node;

        node = getNodeForKey(XMLFilePath, rootName, key, &rootNode, &doc);

        if (node) {
            if (node->FirstChild()) {
                node->FirstChild()->SetValue(value);
            } else {
                tinyxml2::XMLText *content = doc.NewText(value);
                node->LinkEndChild(content);
            }
        } else {
            if (rootNode) {
                tinyxml2::XMLElement *tmpNode = doc.NewElement(key);
                rootNode->LinkEndChild(tmpNode);
                tinyxml2::XMLText *content = doc.NewText(value);
                tmpNode->LinkEndChild(content);
            }
        }

        File file;
        if (file.open(XMLFilePath, true) == FileErrors::NO_ERROR) {
            doc.SaveFile(file.getFilePtr());
        } else {
            Log::Error("Can't save XML file: %s", XMLFilePath);
        }
    }
}