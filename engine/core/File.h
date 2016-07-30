#ifndef file_h
#define file_h

class File {

private:

    long dataLength;
    void* data;
    void* fileHandle;

public:

    File();
    File(const char* path);
    virtual ~File();

    void loadFile(const char* path);

    long getDataLength();
    void* getData();
    void* getFileHandle();
};

#endif /* file_h */
