//
// Created by 叶飞 on 15/10/16.
//

#include "DecryptUtil.h"

using namespace std;

Data DecryptUtil::decryptFile(Data fileData)
{
    Data ret;
    xxtea_long ret_len;
    std::string keyStr = "weme_game";
    unsigned char* key = (unsigned char* )keyStr.c_str();
    size_t length = strlen((char*)(key));

    unsigned char*ret_data = xxtea_decrypt(fileData.getBytes(),(xxtea_long)fileData.getSize(), key, (xxtea_long)length, &ret_len);

    if (ret_data==NULL) {
        return Data::Null;
    } else {
        ret.fastSet(ret_data,ret_len);
    }
    return ret;
    CC_SAFE_DELETE(ret_data);
}

bool DecryptUtil::isLevelConfigFile(const std::string& strName)
{
    if(strName.find(".enemy") != -1 || strName.find(".track") != -1 || strName.find("barrage.json") != -1 || strName.find("item.json") != -1)
    {
        return true;
    } else {
        return false;
    }
}

bool DecryptUtil::isImgFile(string strName)
{
    if(strName.find(".png") != -1 || strName.find(".jpg") != -1)
    {
        return true;
    } else {
        return false;
    }
}

bool DecryptUtil::needDecrypt()
{
    return false;
}