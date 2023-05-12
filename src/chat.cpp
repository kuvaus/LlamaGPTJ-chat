#include "./header.h"

#include "../gpt4all-backend/llmodel.h"
#include "../gpt4all-backend/llmodel_c.h"
#include "../gpt4all-backend/llmodel_c.cpp"
#include "../gpt4all-backend/llamamodel.h" 
#include "../gpt4all-backend/gptj.h" 
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
        std::cerr << "\r" << frames[frame_index % 4] << std::flush;
        frame_index++;
        set_console_color(con_st, DEFAULT);
        if (!stop_display){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::cerr << "\r" << " " << std::flush;
            std::cerr << "\r" << std::flush;
        }
    }
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

void llmodel_free_model(llmodel_model model) {

    LLModelWrapper *wrapper = reinterpret_cast<LLModelWrapper*>(model);
    const std::type_info &modelTypeInfo = typeid(*wrapper->llModel);

    if (modelTypeInfo == typeid(GPTJ))       { llmodel_gptj_destroy(model);  }
    if (modelTypeInfo == typeid(LLamaModel)) { llmodel_llama_destroy(model); }
    if (modelTypeInfo == typeid(MPT))        { llmodel_mpt_destroy(model);   }

}

void update_struct(llmodel_prompt_context  &prompt_context, chatParams &params){
    // TODO: handle this better
    prompt_context.n_predict = params.n_predict;
    prompt_context.top_k = params.top_k;
    prompt_context.top_p = params.top_p;
    prompt_context.temp = params.temp;
    prompt_context.n_batch = params.n_batch;
    prompt_context.repeat_penalty = params.repeat_penalty;  
    prompt_context.repeat_last_n = params.repeat_last_n;  
    prompt_context.context_erase = params.context_erase; 
    }

    // Set up the prompt context
//llmodel_prompt_context prompt_context;

llmodel_prompt_context prompt_context = {
        .logits = NULL,
        .logits_size = 0,
        .tokens = NULL,
        .tokens_size = 4096,
        .n_past = 0,
        .n_ctx = 1024,
        .n_predict = 50,
        .top_k = 40,
        .top_p = 0.95,
        .temp = 0.28,
        .n_batch = 9,
        .repeat_penalty = 1.1,
        .repeat_last_n = 64,
        .context_erase = 0.75
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
        llmodel_free_model(model);
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

    std::string response;
    response.reserve(10000);
    answer.reserve(10000);
    chatParams params;
    

    //convert the default model path into Windows format if on WIN32
    #ifdef _WIN32
        std::filesystem::path p(params.model);
        params.model = p.make_preferred().string();
    #endif

    //std::string prompt = "";
    std::string input = "";
    uint32_t magic;
   

    set_console_color(con_st, PROMPT);
    set_console_color(con_st, BOLD);
    std::cout << appname;
    set_console_color(con_st, DEFAULT);
    set_console_color(con_st, PROMPT);
    std::cout << " (v. " << VERSION << ")";
    set_console_color(con_st, DEFAULT);
    std::cout << "" << std::endl;
    
    parse_params(argc, argv, params);




    auto future = std::async(std::launch::async, display_loading);

    //handle stderr for now
    //this is just to prevent unnecessary details during model loading.
    int stderr_copy = dup(fileno(stderr));
    #ifdef _WIN32
        //disabled for windows for now. The line below needs MinGW support.
        std::freopen("NUL", "w", stderr);
    #else
        std::freopen("/dev/null", "w", stderr);
    #endif
 

    llmodel_model model;

    //Load the model. The magic value checks if the model type is GPTJ, MPT, or LlaMa
    std::ifstream f(params.model.c_str(), std::ios::binary);
    //uint32_t magic;
    f.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    // Create model
    if (magic == 0x67676d6c) {
        f.close();
        model = llmodel_gptj_create();
    } else if (magic == 0x67676d6d) {
        f.close();
        model = llmodel_mpt_create();
    } else if (magic == 0x67676a74) {
        f.close();
        model = llmodel_llama_create();
    } else {
    	f.close();
    	std::cerr << "Error loading: " << params.model.c_str() << std::endl;
    	std::cerr << "Model is not of any supported type" << std::endl;
    }
    std::cout << "\r" << appname << ": loading " << params.model.c_str()  << std::endl;
    
    auto check_model = llmodel_loadModel(model, params.model.c_str());


    //bring back stderr for now
    dup2(stderr_copy, fileno(stderr));
    close(stderr_copy);
    
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
        std::cout << "\r" << appname << ": done loading!" << std::flush;   
    }

    set_console_color(con_st, PROMPT);
    std::cout << "\n" << params.prompt.c_str() << std::endl;
    set_console_color(con_st, DEFAULT);

    //default prompt template, should work with most instruction-type models
    std::string default_prefix = "### Instruction:\n The prompt below is a question to answer, a task to complete, or a conversation to respond to; decide which and write an appropriate response.";
    std::string default_header = "\n### Prompt: ";
    std::string default_footer = "\n### Response: ";
    
    if (params.load_template != "") {
        std::tie(default_prefix, default_header, default_footer) = read_prompt_template_file(params.load_template);
    }

    //////////////////////////////////////////////////////////////////////////
    ////////////            PROMPT LAMBDA FUNCTIONS               ////////////
    //////////////////////////////////////////////////////////////////////////


    auto lambda_prompt = [](int32_t token_id, const char *promptchars)  {
	        // You can handle prompt here if needed
	        return true;
	    };


    auto lambda_response = [](int32_t token_id, const char *responsechars) {
    
	   		std::string response = responsechars;
	
	        if (!response.empty()) {
	        // stop the animation, printing response
            if (stop_display == false) {
	            stop_display = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                std::cerr << "\r" << " " << std::flush;
                std::cerr << "\r" << std::flush;
            }
	        // handle ### token separately
            // this might not be needed in the fuure
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

    if (!params.no_interactive) {
        input = get_input(con_st, model, input);
        if (params.prompt != "") {
            if (params.use_animation){ future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (default_prefix + default_header + params.prompt + " " + input + default_footer).c_str(),
            lambda_prompt, lambda_response, lambda_recalculate, &prompt_context);
            if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
        } else {
            if (params.use_animation){ future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (default_prefix + default_header + input + default_footer).c_str(),
            lambda_prompt, lambda_response, lambda_recalculate, &prompt_context);
            if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }

        }
        //answer = response.c_str();

        while (!params.run_once) {
            std::string memory_string = default_prefix;
            if (params.remember > 1) {
                memory_string = default_prefix + default_header + input.substr(0, params.remember) + default_footer + answer.substr(0, params.remember);
            }
            answer = ""; //New prompt. We stored previous answer in memory so clear it.
            input = get_input(con_st, model, input);
            if (params.use_animation){ future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (memory_string + default_header + input + default_footer).c_str(), 
            lambda_prompt, lambda_response, lambda_recalculate, &prompt_context);
            if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }

            //answer = response.c_str();
        }
    } else {
        if (params.use_animation){ future = std::async(std::launch::async, display_frames); }
        llmodel_prompt(model, (default_prefix + default_header + params.prompt + default_footer).c_str(), 
        lambda_prompt, lambda_response, lambda_recalculate, &prompt_context);
        if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }

    }




    set_console_color(con_st, DEFAULT);
    llmodel_free_model(model);
    return 0;
}
