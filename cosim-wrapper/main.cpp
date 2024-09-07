#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <dlfcn.h>

std::map<std::string, void*> load_functions(const std::string& file_path, void* handle) {
    std::map<std::string, void*> function_map;
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return function_map;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t delimiter_pos = line.find(": ");
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 2);
            void* func_ptr = dlsym(handle, key.c_str());
            if (!func_ptr) {
                std::cerr << "Failed to load function: " << key << " - " << dlerror() << std::endl;
            } else {
                function_map[value] = func_ptr;
            }
        }
    }

    file.close();
    return function_map;
}

int main() {
    std::string file_path = "processed_log";
    const char * dl_file_path = "../build/emu";
    void* handle = dlopen(dl_file_path, RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to open handle: " << dlerror() << std::endl;
        return 1;
    }

    std::map<std::string, void*> function_map = load_functions(file_path, handle);

    // 打印加载的函数指针地址
    // for (const auto& pair : function_map) {
    //     std::cout << pair.first << ": " << pair.second << std::endl;
    // }

    auto difftest_step = (int (*)(void))function_map["difftest_step"];
    auto difftest_init = (void (*)(void))function_map["difftest_init"];
    auto difftest_raise_intr = (void (*)(int))function_map["difftest_raise_intr"];
    auto difftest_getregs = (void (*)(void))function_map["difftest_getregs"];
    auto difftest_setregs = (void (*)(void))function_map["difftest_setregs"];

    

    dlclose(handle);
    return 0;
}