#pragma once

#ifndef HEADER_H
#define HEADER_H



#include <cstdio>

//Switch to MinGW compilation.
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

//For Windows paths
//Commented out to support really old xcode
#ifdef _WIN32
    #include <filesystem>
#endif


#include <typeinfo>
#include <future>
#include <chrono>
#include <atomic>
#include <fcntl.h>
#include "config.h"

// this contains all the parameters you can import from json or with cli arguments
struct chatParams {
        //.logits = NULL,
        //.logits_size = 0,
        //.tokens = NULL,
        //.tokens_size = 4096,
        //.n_past = 0,
        //.n_ctx = 1024, 
        int32_t n_predict = 50;
        int32_t top_k = 40;
        float top_p = 0.95;
        float temp = 0.28;
        int32_t n_batch = 9;     
        float repeat_penalty = 1.1;
        int32_t repeat_last_n = 64;
        float context_erase = 0.75;
	    int32_t seed = -1; 
    	int32_t n_threads = std::min(4, (int32_t)std::thread::hardware_concurrency()); 
    	std::string model = "./models/ggml-vicuna-13b-1.1-q4_2.bin";
		std::string prompt = "";
		bool no_interactive = false;
		bool use_animation = true;
		std::string load_template = ""; //template file location
		bool run_once = false;
        int32_t remember = 200;
		std::string load_json = ""; //json file location
	
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

std::string appname = "LlamaGPTJ-chat";

//utils.h functions
void set_console_color(ConsoleState &con_st, ConsoleColor color);
std::string random_prompt(int32_t seed);
std::string readFile(const std::string& filename);
std::map<std::string, std::string> parse_json_string(const std::string& jsonString);
std::string removeQuotes(const std::string& input);

//parse_json.h functions
//void get_params_from_json(LLMParams & params, std::string& prompt, bool& interactive, bool& continuous, int& memory, std::string& prompt_template, std::string& filename);
void get_params_from_json(chatParams& params);
//void print_usage(int argc, char** argv, const LLMParams& params, std::string& prompt, int& memory);
void print_usage(int argc, char** argv, const chatParams& params);
//bool parse_params(int argc, char** argv, LLMParams& params, std::string& prompt, bool& interactive, bool& continuous, int& memory);
bool parse_params(int argc, char** argv, chatParams& params);

#endif