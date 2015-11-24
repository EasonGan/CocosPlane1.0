//
// Created by 叶飞 on 15/10/24.
//

#ifndef PLANEGAME_DANJI_ENCRYPTVALUE_H
#define PLANEGAME_DANJI_ENCRYPTVALUE_H

#include <stdlib.h>
#include "cocos2d.h"

class EncryptValue : public cocos2d::Ref{
public:
    static EncryptValue* create();

    EncryptValue()
            : m_Value(0)
    {
    }
    ~EncryptValue() {}

    int getValue()
    {
        int v = m_Value ^ m_EncryptKey;
        return v;
    }

    void setValue(int value)
    {
        m_Value = value;
        m_EncryptKey = rand();
        m_Value ^= m_EncryptKey;
    }

    void offset(int value)
    {
        setValue(getValue() + value);
    }

    int m_Value;
    int m_EncryptKey;
};

#endif //PLANEGAME_DANJI_ENCRYPTVALUE_H
