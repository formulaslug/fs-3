//
// Created by wangd on 10/8/2024.
//

#ifndef MODULE_H
#define MODULE_H

#include <cstdint>

/**
* Base class for modules containing error codes.
*/
class Module {
public:
    static constexpr int32_t MOD_ERR_ARG = -1;
    static constexpr int32_t MOD_ERR_RESOURCE = -2;
    static constexpr int32_t MOD_ERR_STATE = -3;
    static constexpr int32_t MOD_ERR_BAD_CMD = -4;
    static constexpr int32_t MOD_ERR_BUF_OVERRUN = -5;
    static constexpr int32_t MOD_ERR_BAD_INSTANCE = -6;

};
#endif //MODULE_H
