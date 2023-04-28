#pragma once

#ifndef PARSE_JSON_H
#define PARSE_JSON_H

#include "header.h" 

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

void get_params_from_json(GPTJParams & params, std::string& filename) {
    std::map<std::string, std::string> parsed = parse_json_string(readFile(filename));

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
    if (parsed.find("seed") != parsed.end())
        params.seed = std::stoi(parsed["seed"]);
    if (parsed.find("n_threads") != parsed.end())
        params.n_threads = std::stoi(parsed["n_threads"]);
    if (parsed.find("model") != parsed.end())
        params.model = removeQuotes(parsed["model"]);
}


#endif