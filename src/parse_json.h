#pragma once

#ifndef PARSE_JSON_H
#define PARSE_JSON_H

#include "header.h" 

void from_json(const nlohmann::json& j, chatParams& params) {
    auto it = j.find("n_ctx"); if (it != j.end()) { it->get_to(params.n_ctx); }
    it = j.find("n_predict"); if (it != j.end()) { it->get_to(params.n_predict); }
    it = j.find("top_k"); if (it != j.end()) { it->get_to(params.top_k); }
    it = j.find("top_p"); if (it != j.end()) { it->get_to(params.top_p); }
    it = j.find("temp"); if (it != j.end()) { it->get_to(params.temp); }
    it = j.find("n_batch"); if (it != j.end()) { it->get_to(params.n_batch); }
    it = j.find("repeat_penalty"); if (it != j.end()) { it->get_to(params.repeat_penalty); }
    it = j.find("repeat_last_n"); if (it != j.end()) { it->get_to(params.repeat_last_n); }
    it = j.find("context_erase"); if (it != j.end()) { it->get_to(params.context_erase); }
    it = j.find("seed"); if (it != j.end()) { it->get_to(params.seed); }
    it = j.find("n_threads"); if (it != j.end()) { it->get_to(params.n_threads); }
    it = j.find("model"); if (it != j.end()) { it->get_to(params.model); }
    it = j.find("prompt"); if (it != j.end()) { it->get_to(params.prompt); }
    it = j.find("no_interactive"); if (it != j.end()) { it->get_to(params.no_interactive); }
    it = j.find("no-animation"); if (it != j.end())
         { it->get_to(params.use_animation); params.use_animation = !params.use_animation; }
    it = j.find("run_once"); if (it != j.end()) { it->get_to(params.run_once); }
    it = j.find("load_template"); if (it != j.end()) { it->get_to(params.load_template); }
    it = j.find("load_json"); if (it != j.end()) { it->get_to(params.load_json); }
    it = j.find("save_log"); if (it != j.end()) { it->get_to(params.save_log); }
    it = j.find("httpserver"); if (it != j.end()) { it->get_to(params.httpserver); }
    it = j.find("openai_api_key"); if (it != j.end()) { it->get_to(params.openai_api_key); }
}

void get_params_from_json(chatParams& params) {
    nlohmann::json j;
    
    std::ifstream inFile(params.load_json);
    if (!inFile) {
        std::cerr << "Unable to open file: " << params.load_json << std::endl;
        return;
    }
    inFile >> j;
    from_json(j, params);
};


#endif