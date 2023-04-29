#include "./header.h"

#include "../llmodel/llmodel.h"
#include "../llmodel/llmodel_c.h"
#include "../llmodel/llmodel_c.cpp"
#include "../llmodel/llamamodel.h" 
#include "../llmodel/gptj.h" 
#include "./utils.h"
#include "./parse_json.h"

//////////////////////////////////////////////////////////////////////////
////////////                    ANIMATION                     ////////////
//////////////////////////////////////////////////////////////////////////

std::atomic<bool> stop_display{false}; 

void display_frames() {
    const char* frames[] = {".", ":", "'", ":"};
    int frame_index = 0;
    ConsoleState con_st;
    con_st.use_color = true;
    while (!stop_display) {
        set_console_color(con_st, PROMPT);
        std::cout << "\r" << frames[frame_index % 4] << std::flush;
        frame_index++;
        set_console_color(con_st, DEFAULT);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::cout << "\r" << " " << std::flush;
    std::cout << "\r" << std::flush;
}

void display_loading() {

    while (!stop_display) {


        for (int i=0; i < 14; i++){
                fprintf(stdout, ".");
                fflush(stdout);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                if (stop_display){ break; }
        }
        
            std::cout << "\r" << "               " << "\r" << std::flush;
    }
    std::cout << "\r" << " " << std::flush;

}


//////////////////////////////////////////////////////////////////////////
////////////                    ANIMATION                     ////////////
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
////////////                 LLAMA FUNCTIONS                  ////////////
//////////////////////////////////////////////////////////////////////////

std::string llmodel_getModelType(llmodel_model model)
{
    LLModelWrapper *wrapper = reinterpret_cast<LLModelWrapper*>(model);
    const std::type_info &modelTypeInfo = typeid(*wrapper->llModel);

    if (modelTypeInfo == typeid(GPTJ))
    {
        return "GPTJ";
    }
    else if (modelTypeInfo == typeid(LLamaModel))
    {
        return "LLamaModel";
    }
    else
    {
        return "Unknown";
    }
}

bool containsSubstring(const std::string &str, const std::string &substr) {
    return str.find(substr) != std::string::npos;
}

void update_struct(llmodel_prompt_context  &prompt_context, LLMParams &params){
    // TODO: handle this better
    prompt_context.n_predict = params.n_predict;
    prompt_context.top_k = params.top_k;
    prompt_context.top_p = params.top_p;
    prompt_context.temp = params.temp;
    prompt_context.n_batch = params.n_batch;

    prompt_context.repeat_penalty = 1.10f;  
    prompt_context.repeat_last_n = 64;  
    prompt_context.context_erase = 0.75f; 
    }

        // Set up the prompt context
llmodel_prompt_context prompt_context = {
        .logits = NULL,
        .logits_size = 0,
        .tokens = NULL,
        .tokens_size = 0,
        .n_past = 0,
        .n_ctx = 1024,
        .n_predict = 50,
        .top_k = 10,
        .top_p = 0.9,
        .temp = 1.0,
        .n_batch = 1,
        .repeat_penalty = 1.2,
        .repeat_last_n = 10,
        .context_erase = 0.5
    };

std::string hashstring ="";
std::string answer ="";

//////////////////////////////////////////////////////////////////////////
////////////                 LLAMA FUNCTIONS                  ////////////
//////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////
////////////                 CHAT FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////



std::string get_input(ConsoleState& con_st, llmodel_model model, std::string& input) {
    set_console_color(con_st, USER_INPUT);

    std::cout << "\n> ";
    std::getline(std::cin, input);
    set_console_color(con_st, DEFAULT);

    if (input == "exit" || input == "quit") {
        
            
        std::string check_model_type = llmodel_getModelType(model);
        if (check_model_type == "LLamaModel") {
            llmodel_llama_destroy(model);
        }
        if (check_model_type == "GPTJ") {
            llmodel_gptj_destroy(model);
        }
        
        exit(0);
    }

    return input;
}


//////////////////////////////////////////////////////////////////////////
////////////                 CHAT FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
////////////                  MAIN PROGRAM                    ////////////
//////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[]) {


    ConsoleState con_st;
    con_st.use_color = true;
    set_console_color(con_st, DEFAULT);

    bool interactive = true;
    bool continuous = true;

    std::string response;
    response.reserve(10000);
    answer.reserve(10000);
    int memory = 200;
    LLMParams params;
    std::string prompt = "";
    std::string input = "";
    //std::string answer = "";
   

    set_console_color(con_st, PROMPT);
    set_console_color(con_st, BOLD);
    std::cout << "LlamaGPTJ-chat";
    set_console_color(con_st, DEFAULT);
    set_console_color(con_st, PROMPT);
    std::cout << " (v. " << VERSION << ")";
    set_console_color(con_st, DEFAULT);
    std::cout << "" << std::endl;
    
    parse_params(argc, argv, params, prompt, interactive, continuous, memory);




    auto future = std::async(std::launch::async, display_loading);

    //handle stderr for now
    int stderr_copy = dup(fileno(stderr));
    #ifdef _WIN32
       // std::freopen("NUL", "w", stderr);
    #else
        std::freopen("/dev/null", "w", stderr);
    #endif


    llmodel_model model;
    bool use_animation = true;
    if (containsSubstring(params.model.c_str(), "gpt4all-j")) {
         model = llmodel_gptj_create();
    } else {
         model = llmodel_llama_create();
    }

    std::cout << "\r" << "LlamaGPTJ-chat: loading " << params.model.c_str()  << std::endl;
    
    auto check_model = llmodel_loadModel(model, params.model.c_str());
    

    std::cout << "Model type: " << llmodel_getModelType(model) << std::endl;


    //bring back stderr for now
    #ifdef _WIN32
    #else
        dup2(stderr_copy, fileno(stderr));
        close(stderr_copy);
    #endif
    
    if (check_model == false) {
        stop_display = true;
        future.wait();
        stop_display= false;   

        std::cerr << "Error loading: " << params.model.c_str() << std::endl;
        std::cout << "Press any key to exit..." << std::endl;
        std::cin.get();
        return 0;
    } else {
        stop_display = true;
        future.wait();
        stop_display= false;
        std::cout << "\r" << "LlamaGPTJ-chat: done loading!" << std::flush;   
    }

    set_console_color(con_st, PROMPT);
    std::cout << " " << prompt.c_str() << std::endl;
    set_console_color(con_st, DEFAULT);

    std::string default_prefix = "### Instruction:\n The prompt below is a question to answer, a task to complete, or a conversation to respond to; decide which and write an appropriate response.";
    std::string default_header = "\n### Prompt: ";
    std::string default_footer = "\n### Response: ";
  
  
  
    //////////////////////////////////////////////////////////////////////////
    ////////////            PROMPT LAMBDA FUNCTIONS               ////////////
    //////////////////////////////////////////////////////////////////////////


    auto lambda_prompt = [](int32_t token_id, const char *promptchars)  {
	//auto lambda_prompt = [](int32_t token_id) {
	        // You can handle prompt here if needed
	        //std::cout << token_id << std::flush;
	        //std::cout << promptchars << std::flush;
	        return true;
	    };


    auto lambda_response = [](int32_t token_id, const char *responsechars) {
	//auto lambda_response = [&hashstring, &answer](int32_t token_id, std::string response) {
	   		std::string response = responsechars;
	
	        if (!response.empty()) {
	        // stop the animation, printing response
	        stop_display = true;

	        // handle ### token separately
	        if (response == "#" || response == "##") {
	            hashstring += response;
	        } else if (response == "###" || hashstring == "###") {
	            hashstring = "";
	            return false;
	        }
				std::cout << response << std::flush;
	            answer = answer + response;
	        }
	            
	    return true;
	};
	
	auto lambda_recalculate = [](bool is_recalculating) {
	        // You can handle recalculation requests here if needed
	    return is_recalculating;
	};


    //////////////////////////////////////////////////////////////////////////
    ////////////            PROMPT LAMBDA FUNCTIONS               ////////////
    //////////////////////////////////////////////////////////////////////////


    update_struct(prompt_context, params);
    
    
    llmodel_setThreadCount(model, params.n_threads);
    llmodel_setMlock(model, true);

    if (interactive) {
        input = get_input(con_st, model, input);
        if (prompt != "") {
            if (use_animation){ future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (default_prefix + default_header + prompt + " " + input + default_footer).c_str(),
            lambda_prompt, lambda_response, lambda_recalculate, &prompt_context);
            if (use_animation){ stop_display = true; future.wait(); stop_display = false; }
        } else {
            if (use_animation){ future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (default_prefix + default_header + input + default_footer).c_str(),
            lambda_prompt, lambda_response, lambda_recalculate, &prompt_context);
            if (use_animation){ stop_display = true; future.wait(); stop_display = false; }

        }
        //answer = response.c_str();

        while (continuous) {
            std::string memory_string = default_prefix;
            if (memory > 1) {
                memory_string = default_prefix + default_header + input.substr(0, memory) + default_footer + answer.substr(0, memory);
            }
            answer = ""; //New prompt. We stored previous answer in memory so clear it.
            input = get_input(con_st, model, input);
            if (use_animation){ future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (memory_string + default_header + input + default_footer).c_str(), 
            lambda_prompt, lambda_response, lambda_recalculate, &prompt_context);
            if (use_animation){ stop_display = true; future.wait(); stop_display = false; }

            //answer = response.c_str();
        }
    } else {
        if (use_animation){ future = std::async(std::launch::async, display_frames); }
        llmodel_prompt(model, (default_prefix + default_header + prompt + default_footer).c_str(), 
        lambda_prompt, lambda_response, lambda_recalculate, &prompt_context);
        if (use_animation){ stop_display = true; future.wait(); stop_display = false; }

    }




    set_console_color(con_st, DEFAULT);

    std::string check_model_type = llmodel_getModelType(model);
    if (check_model_type == "LLamaModel") {
        llmodel_llama_destroy(model);
    }
    if (check_model_type == "GPTJ") {
        llmodel_gptj_destroy(model);
    }

    return 0;
}
