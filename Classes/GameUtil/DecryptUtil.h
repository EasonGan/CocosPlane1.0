//
// Created by 叶飞 on 15/10/16.
//

#ifndef PLANEGAME_DANJI_DECRYPTUTIL_H
#define PLANEGAME_DANJI_DECRYPTUTIL_H

#include "cocos2d.h"
#include "xxtea.h"

USING_NS_CC;

class DecryptUtil {
public:
    static bool isImgFile(std::string fileName);
    static bool needDecrypt();
    static Data decryptFile(Data fileData);
    static bool isLevelConfigFile(const std::string& filename);
};


#endif //PLANEGAME_DANJI_DECRYPTUTIL_H
