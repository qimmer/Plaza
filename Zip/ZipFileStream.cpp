
#include "ZipFileStream.h"
#include <zip.h>


    struct ZipFileStream {
    };

    DefineComponent(ZipFileStream)

    EndComponent()

    DefineService(ZipFileStream)

    EndService()

    static bool ServiceStart() {
        return true;
    }

    static bool ServiceStop() {
        return true;
    }
}