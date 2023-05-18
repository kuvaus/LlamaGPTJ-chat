#pragma once

#ifndef OPENAI_H
#define OPENAI_H

#include "header.h" 
#include "utils.h" 


//////////////////////////////////////////////////////////////////////////
////////////                 OPENAI CHATGPT                   ////////////
////////////////////////////////////////////////////////////////////////// 

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

void openai_chatgpt(chatParams params, ConsoleState con_st) {
	
    if (params.openai_api_key != "") {
        if (params.openai_api_key.find("sk-") != 0) {
            params.openai_api_key = readFile(params.openai_api_key);
        }
    }

	std::future<void> future;
    std::string input = "";
    std::string answer = "";
	
    openai::start(params.openai_api_key);
        nlohmann::json j = {
            {"model", params.model},
            {"messages", {{{"role", "user"}, {"content", params.prompt}}}}, 
            {"max_tokens", params.n_predict},
            {"temperature", params.temp},
            {"top_p", params.top_p},
            {"n", 1} //{"n", params.n_batch}
        };
        nlohmann::json completion;
        //openai chat loop.
        if (!params.no_interactive) {
            input = get_input(con_st, input);

            //Interactive mode. We have a prompt.
            if (params.prompt != "") {
                if (params.use_animation){ stop_display = false; future = std::async(std::launch::async, display_frames); }
                j = { {"model", params.model}, {"messages", {{{"role", "user"}, {"content",params.prompt + " " + input}}}},  {"max_tokens", params.n_predict},  {"temperature", params.temp},  {"top_p", params.top_p}, {"n", 1} };
                completion = openai::chat().create(j);
                if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
                answer = completion["choices"][0]["message"]["content"];
                std::cout << answer << std::flush;
                if (params.save_log != ""){ save_chat_log((params.prompt + " " + input).c_str(), params.save_log, answer.c_str()); }

            //Interactive mode. Else get prompt from input.
            } else {
                if (params.use_animation){ stop_display = false; future = std::async(std::launch::async, display_frames); }
                j = { {"model", params.model}, {"messages", {{{"role", "user"}, {"content",input}}}},  {"max_tokens", params.n_predict},  {"temperature", params.temp},  {"top_p", params.top_p}, {"n", 1} };
                completion = openai::chat().create(j);
                if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
                answer = completion["choices"][0]["message"]["content"];
                std::cout << answer << std::flush;
                if (params.save_log != ""){ save_chat_log(params.save_log, input.c_str(), answer.c_str()); }
            }
            //Interactive and continuous mode. Get prompt from input.

            while (!params.run_once) {
                answer = ""; //New prompt. We stored previous answer in memory so clear it.
                input = get_input(con_st, input);
                if (params.use_animation){ stop_display = false; future = std::async(std::launch::async, display_frames); }
                j = { {"model", params.model}, {"messages", {{{"role", "user"}, {"content",input}}}},  {"max_tokens", params.n_predict},  {"temperature", params.temp},  {"top_p", params.top_p}, {"n", 1} };
                completion = openai::chat().create(j);
                if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
                answer = completion["choices"][0]["message"]["content"];
                std::cout << answer << std::flush;
                if (params.save_log != ""){ save_chat_log(params.save_log, input.c_str(), answer.c_str()); }

            }

        //No-interactive mode. Get the answer once from prompt and print it.
        } else {
            if (params.use_animation){ stop_display = false; future = std::async(std::launch::async, display_frames); }
                j = { {"model", params.model}, {"messages", {{{"role", "user"}, {"content",params.prompt}}}},  {"max_tokens", params.n_predict},  {"temperature", params.temp},  {"top_p", params.top_p}, {"n", 1} };
                completion = openai::chat().create(j);
                if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
                answer = completion["choices"][0]["message"]["content"];
                std::cout << answer << std::flush;
                if (params.save_log != ""){ save_chat_log(params.save_log, params.prompt.c_str(), answer.c_str()); }
            std::cout << std::endl;
        }
    
    set_console_color(con_st, DEFAULT);
    return;

}
//////////////////////////////////////////////////////////////////////////
////////////                 /OPENAI CHATGPT                  ////////////
////////////////////////////////////////////////////////////////////////// 



#endif
