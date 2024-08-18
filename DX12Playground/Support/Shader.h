#pragma once
#include <Support/WinInclude.h>
#include <filesystem>
#include <string_view>
#include <fstream>
#include <iostream>

class Shader
{
public:
    Shader(std::string_view name);
    ~Shader();

    inline const void* GetBuffer() const { return m_data; }
    inline const size_t GetSize() const { return m_size;  }

private:
    void* m_data = nullptr;
    size_t m_size = 0;
};

