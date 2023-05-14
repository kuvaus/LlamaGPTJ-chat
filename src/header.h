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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


// chatParams contains all the parameters you can import from json or with cli arguments
// it also contains the initial value for PromptContext
struct chatParams {
        //std::vector<float> logits,          // logits of current context 
        //std::vector<int32_t> tokens,        // current tokens in the context window 

        //These are in the prompt context, maybe add as parameters too.    
        float *logits = nullptr;            // logits of current context
        size_t logits_size = 0;             // the size of the raw logits vector
        int32_t *tokens = nullptr;          // current tokens in the context window
        size_t tokens_size = 0;             // the size of the raw tokens vector
        int32_t n_past = 0;                 // number of tokens in past conversation
        //Parameters below you can import from json or with cli arguments
        int32_t n_ctx = 0;                  // number of tokens possible in context window
        int32_t n_predict = 200;            // number of tokens to predict
        int32_t top_k = 40;                 // top k logits to sample from
        float top_p = 0.95;                 // nucleus sampling probability threshold
        float temp = 0.28;                  // temperature to adjust model's output distribution
        int32_t n_batch = 9;                // number of predictions to generate in parallel
        float repeat_penalty = 1.1;         // penalty factor for repeated tokens
        int32_t repeat_last_n = 64;         // last n tokens to penalize
        float context_erase = 0.75;         // percent of context to erase if we exceed the context window
        //Parameters below are not inside prompt_context, but handled separately
        int32_t seed = -1; 
        int32_t n_threads = std::min(4, (int32_t)std::thread::hardware_concurrency()); 
        std::string model = "./models/ggml-vicuna-13b-1.1-q4_2.bin";
        std::string prompt = "";
        //You can toggle chat interactivity with these parameters
        bool no_interactive = false;
        bool use_animation = true;
        bool run_once = false;
        std::string load_template = "";     //template file location
        std::string load_json = "";         //json file location

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

std::string APPNAME = "GPT4All-cli";

//utils.h functions
void set_console_color(ConsoleState &con_st, ConsoleColor color);
std::string random_prompt(int32_t seed);
void print_usage(int argc, char** argv, const chatParams& params);
bool parse_params(int argc, char** argv, chatParams& params);

//parse_json.h functions
void get_params_from_json(chatParams& params);

#endif