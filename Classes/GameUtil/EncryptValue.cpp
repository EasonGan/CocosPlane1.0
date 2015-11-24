//
// Created by 叶飞 on 15/10/24.
//

#include "EncryptValue.h"

EncryptValue* EncryptValue::create()
{
    EncryptValue* rec = new(std::nothrow) EncryptValue();
    if(rec) {
        return rec;
    } else {
        delete rec;
        rec = NULL;
        return NULL;
    }
}
