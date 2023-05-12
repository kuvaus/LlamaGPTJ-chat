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


// chatParams contains all the parameters you can import from json or with cli arguments
// it also contains the initial value for PromptContext
struct chatParams {
        //std::vector<float> logits,          // logits of current context 
        //std::vector<int32_t> tokens,        // current tokens in the context window 
        //These are in the prompt context, maybe add as parameters too.    
        float *logits = NULL;               // logits of current context
        size_t logits_size = 0;             // the size of the raw logits vector
        int32_t *tokens = NULL;             // current tokens in the context window
        size_t tokens_size = 0;             // the size of the raw tokens vector
        int32_t n_past = 0;                 // number of tokens in past conversation
        int32_t n_ctx = 0;                  // number of tokens possible in context window
        // below contains all the parameters you can import from json or with cli arguments
        int32_t n_predict = 50;
        int32_t top_k = 40;
        float top_p = 0.95;
        float temp = 0.28;
        int32_t n_batch = 9;     
        float repeat_penalty = 1.1;
        int32_t repeat_last_n = 64;          // last n tokens to penalize
        float context_erase = 0.75;          // percent of context to erase if we exceed the context window
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
void get_params_from_json(chatParams& params);
void print_usage(int argc, char** argv, const chatParams& params);
bool parse_params(int argc, char** argv, chatParams& params);

#endif