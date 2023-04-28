#pragma once

#ifndef HEADER_H
#define HEADER_H




#include <cstdio>

//Switched to MinGW compilation.
//No need to include <windows.h>
#include <unistd.h>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <regex>
#include <cstring>
#include <functional>


#include <future>
#include <chrono>
#include <atomic>
#include <fcntl.h>
#include "config.h"


struct GPTJParams {
    int32_t seed = -1;
    int32_t n_threads = std::min(4, (int32_t)std::thread::hardware_concurrency());
    int32_t n_predict = 200;

    int32_t top_k = 50400;
    float top_p = 1.0f;
    float temp = 0.9f;

    int32_t n_batch = 9;
    std::string model = "models/ggml-vicuna-13b-1.1-q4_2.bin";
};

enum ConsoleColor {
    DEFAULT = 0,
    PROMPT,
    USER_INPUT,
    BOLD
};

struct ConsoleState {
    bool use_color = false;
    ConsoleColor color = DEFAULT;
};



//utils.h functions
void set_console_color(ConsoleState &con_st, ConsoleColor color);
std::string random_prompt(int32_t seed);
std::string readFile(const std::string& filename);
std::map<std::string, std::string> parse_json_string(const std::string& jsonString);
std::string removeQuotes(const std::string& input);

//parse_json.h functions
void get_params_from_json(GPTJParams & params, std::string& filename);
void print_usage(int argc, char** argv, const GPTJParams& params, std::string& prompt, int& memory);
bool parse_params(int argc, char** argv, GPTJParams& params, std::string& prompt, bool& interactive, bool& continuous, int& memory);

#endif