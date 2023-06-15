#pragma once

#ifndef PARSE_JSON_H
#define PARSE_JSON_H

#include "header.h" 

//helper function to convert string to bool
bool stob(const std::string& str) {
    std::string lowerStr = str;
    std::transform(str.begin(), str.end(), lowerStr.begin(), ::tolower);
    if (lowerStr == "true") {
        return true;
    } else if (lowerStr == "false") {
        return false;
    } else {
        throw std::invalid_argument("Invalid boolean string");
    }
}

std::string readFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    inFile.close();
    return buffer.str();
}

std::map<std::string, std::string> parse_json_string(const std::string& jsonString) {
    std::map<std::string, std::string> resultMap;
    std::regex pattern("\"([^\"]+)\":\\s*([^\"]+|\"[^\"]+\")");
    std::smatch match;
    std::string::const_iterator searchStart(jsonString.cbegin());

    while (std::regex_search(searchStart, jsonString.cend(), match, pattern)) {
        resultMap[match[1]] = match[2];
        searchStart = match.suffix().first;
    }
    return resultMap;
}

std::string removeQuotes(const std::string& input) {
    std::string result = input;
    result.erase(std::remove(result.begin(), result.end(), '\"'), result.end());
    return result;
}

void get_params_from_json(chatParams& params) {
    std::map<std::string, std::string> parsed = parse_json_string(readFile(params.load_json));

    if (parsed.find("top_p") != parsed.end())
        params.top_p = std::stof(parsed["top_p"]);
    if (parsed.find("top_k") != parsed.end())
        params.top_k = std::stoi(parsed["top_k"]);
    if (parsed.find("temp") != parsed.end())
        params.temp = std::stof(parsed["temp"]);
    if (parsed.find("n_predict") != parsed.end())
        params.n_predict = std::stoi(parsed["n_predict"]);
    if (parsed.find("n_batch") != parsed.end())
        params.n_batch = std::stoi(parsed["n_batch"]);
    if (parsed.find("n_ctx") != parsed.end())
        params.n_ctx = std::stoi(parsed["n_ctx"]); 
    if (parsed.find("seed") != parsed.end())
        params.seed = std::stoi(parsed["seed"]);
    if (parsed.find("threads") != parsed.end())
        params.n_threads = std::stoi(parsed["threads"]);
    if (parsed.find("model") != parsed.end())
        params.model = removeQuotes(parsed["model"]);

    if (parsed.find("prompt") != parsed.end())
        params.prompt = removeQuotes(parsed["prompt"]);
    if (parsed.find("no-interactive") != parsed.end())
        params.no_interactive = stob(removeQuotes(parsed["no-interactive"]));    
    if (parsed.find("run-once") != parsed.end())
        params.run_once = stob(removeQuotes(parsed["run-once"]));        
    if (parsed.find("no-animation") != parsed.end())
        params.use_animation = !stob(removeQuotes(parsed["no-animation"]));
    if (parsed.find("no-saves") != parsed.end())
        params.no_saves = stob(removeQuotes(parsed["no-saves"]));

    if (parsed.find("repeat_penalty") != parsed.end())
        params.repeat_penalty = std::stof(parsed["repeat_penalty"]);
    if (parsed.find("repeat_last_n") != parsed.end())
        params.repeat_last_n = std::stoi(parsed["repeat_last_n"]);
    if (parsed.find("context_erase") != parsed.end())
        params.context_erase = std::stof(parsed["context_erase"]);
    if (parsed.find("b_token") != parsed.end())
        params.b_token = removeQuotes(parsed["b_token"]);
    if (parsed.find("e_token") != parsed.end())
        params.e_token = removeQuotes(parsed["e_token"]);              
    if (parsed.find("load_template") != parsed.end())
        params.load_template = removeQuotes(parsed["load_template"]);   
    if (parsed.find("save_log") != parsed.end())
        params.save_log = removeQuotes(parsed["save_log"]);
    if (parsed.find("load_log") != parsed.end())
        params.load_log = removeQuotes(parsed["load_log"]);
    if (parsed.find("save_dir") != parsed.end())
        params.save_dir = removeQuotes(parsed["save_dir"]);
    if (parsed.find("save_name") != parsed.end())
        params.save_name = removeQuotes(parsed["save_name"]);}


#endif