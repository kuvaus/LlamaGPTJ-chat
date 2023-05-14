#include "./header.h"

#include "../../gpt4all-backend/llmodel_c.h"
#include "../../gpt4all-backend/llmodel_c.cpp"
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
////////////                   /ANIMATION                     ////////////
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
////////////                 LLAMA FUNCTIONS                  ////////////
//////////////////////////////////////////////////////////////////////////

//recognize and create correct model type automatically using context size
llmodel_model llmodel_create_model(std::string modelPath) {

    uint32_t magic;
    llmodel_model model;

        FILE *f = fopen(modelPath.c_str(), "rb");
        fread(&magic, sizeof(magic), 1, f);
    //std::ifstream f(modelPath, std::ios::binary);
    //f.read(reinterpret_cast<char*>(&magic), sizeof(magic));

    if (magic == 0x67676d6c)       { model = llmodel_gptj_create();  }
    if (magic == 0x67676a74)       { model = llmodel_llama_create(); }
    if (magic == 0x67676d6d)       { model = llmodel_mpt_create();   }
    else  {std::cerr << "Model is not of any supported type" << std::endl;}
    //f.close();
    fclose(f);
    return model;
}

//free correct model type automatically using model typeid
void llmodel_free_model(llmodel_model model) {

    LLModelWrapper *wrapper = reinterpret_cast<LLModelWrapper*>(model);
    const std::type_info &modelTypeInfo = typeid(*wrapper->llModel);

    if (modelTypeInfo == typeid(GPTJ))       { llmodel_gptj_destroy(model);  }
    if (modelTypeInfo == typeid(LLamaModel)) { llmodel_llama_destroy(model); }
    if (modelTypeInfo == typeid(MPT))        { llmodel_mpt_destroy(model);   }
}

std::string hashstring = "";
std::string answer = "";


//////////////////////////////////////////////////////////////////////////
////////////                /LLAMA FUNCTIONS                  ////////////
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
////////////                /CHAT FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
////////////                  MAIN PROGRAM                    ////////////
//////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[]) {


    ConsoleState con_st;
    con_st.use_color = true;
    set_console_color(con_st, DEFAULT);

    set_console_color(con_st, PROMPT);
    set_console_color(con_st, BOLD);
    std::cout << APPNAME;
    set_console_color(con_st, DEFAULT);
    set_console_color(con_st, PROMPT);
    std::cout << " (v. " << VERSION << ")";
    set_console_color(con_st, DEFAULT);
    std::cout << "" << std::endl;
    


    chatParams params;
    //convert the default model path into Windows format if on WIN32
    #ifdef _WIN32
        std::filesystem::path p(params.model);
        params.model = p.make_preferred().string();
    #endif
 
    //get all parameters from cli arguments or json
    parse_params(argc, argv, params);
    
    //Create a prompt_context and copy all params from chatParams to prompt_context
    llmodel_prompt_context prompt_context = {
     .logits = params.logits,
     .logits_size = params.logits_size,
     .tokens = params.tokens,
     .tokens_size = params.tokens_size,
     .n_past = params.n_past,
     .n_ctx = params.n_ctx,
     .n_predict = params.n_predict,
     .top_k = params.top_k,
     .top_p = params.top_p,
     .temp = params.temp,
     .n_batch = params.n_batch,
     .repeat_penalty = params.repeat_penalty,  
     .repeat_last_n = params.repeat_last_n,
     .context_erase = params.context_erase,
    }; 
 
    //////////////////////////////////////////////////////////////////////////
    ////////////                 LOAD THE MODEL                   ////////////
    ////////////////////////////////////////////////////////////////////////// 
    auto future = std::async(std::launch::async, display_loading);

    //handle stderr for now
    //this is just to prevent printing unnecessary details during model loading.
    int stderr_copy = dup(fileno(stderr));
    #ifdef _WIN32
        std::freopen("NUL", "w", stderr);
    #else
        std::freopen("/dev/null", "w", stderr);
    #endif


    llmodel_model model = llmodel_create_model(params.model.c_str());
    std::cout << "\r" << APPNAME << ": loading " << params.model.c_str()  << std::endl;
    
    //bring back stderr for now
    dup2(stderr_copy, fileno(stderr));
    close(stderr_copy);
    
    

    //check if model is loaded
    auto check_model = llmodel_loadModel(model, params.model.c_str());

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
        std::cout << "\r" << APPNAME << ": done loading!" << std::flush;   
    }
    //////////////////////////////////////////////////////////////////////////
    ////////////                /LOAD THE MODEL                   ////////////
    ////////////////////////////////////////////////////////////////////////// 



    set_console_color(con_st, PROMPT);
    std::cout << "\n" << params.prompt.c_str() << std::endl;
    set_console_color(con_st, DEFAULT);

    //default prompt template, should work with most instruction-type models
    std::string default_prefix = "### Instruction:\n The prompt below is a question to answer, a task to complete, or a conversation to respond to; decide which and write an appropriate response.";
    std::string default_header = "\n### Prompt: ";
    std::string default_footer = "\n### Response: ";
    
    //load prompt template from file instead
    if (params.load_template != "") {
        std::tie(default_prefix, default_header, default_footer) = read_prompt_template_file(params.load_template);
    }

    //////////////////////////////////////////////////////////////////////////
    ////////////            PROMPT LAMBDA FUNCTIONS               ////////////
    //////////////////////////////////////////////////////////////////////////


    auto prompt_callback = [](int32_t token_id, const char *promptchars)  {
	        // You can handle prompt here if needed
	        return true;
	    };


    auto response_callback = [](int32_t token_id, const char *responsechars) {
    
            if (!(responsechars == nullptr || responsechars[0] == '\0')) {
	        // stop the animation, printing response
            if (stop_display == false) {
	            stop_display = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                std::cerr << "\r" << " " << std::flush;
                std::cerr << "\r" << std::flush;
            }
            
				std::cout << responsechars << std::flush;
	            answer += responsechars;
	        }
	            
	    return true;
	};
	
	auto recalculate_callback = [](bool is_recalculating) {
	        // You can handle recalculation requests here if needed
	    return is_recalculating;
	};


    //////////////////////////////////////////////////////////////////////////
    ////////////         PROMPT TEXT AND GET RESPONSE             ////////////
    //////////////////////////////////////////////////////////////////////////

    llmodel_setThreadCount(model, params.n_threads);

    std::string input = "";

    //main chat loop.
    if (!params.no_interactive) {
        input = get_input(con_st, model, input);
        //Interactive mode. We have a prompt.
        if (params.prompt != "") {
            if (params.use_animation){ future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (default_prefix + default_header + params.prompt + " " + input + default_footer).c_str(),
            prompt_callback, response_callback, recalculate_callback, &prompt_context);
            if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
        //Interactive mode. Else get prompt from input.
        } else {
            if (params.use_animation){ future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (default_prefix + default_header + input + default_footer).c_str(),
            prompt_callback, response_callback, recalculate_callback, &prompt_context);
            if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }

        }
        //Interactive and continuous mode. Get prompt from input.
        //answer = response.c_str();
        while (!params.run_once) {
            answer = ""; //New prompt. We stored previous answer in memory so clear it.
            input = get_input(con_st, model, input);
            if (params.use_animation){ future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (default_prefix + default_header + input + default_footer).c_str(), 
            prompt_callback, response_callback, recalculate_callback, &prompt_context);
            if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
            //answer = response.c_str();
        }
    //No-interactive mode. Get the answer once from prompt and print it.
    } else {
        if (params.use_animation){ future = std::async(std::launch::async, display_frames); }
        llmodel_prompt(model, (default_prefix + default_header + params.prompt + default_footer).c_str(), 
        prompt_callback, response_callback, recalculate_callback, &prompt_context);
        if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }

    }


    set_console_color(con_st, DEFAULT);
    llmodel_free_model(model);
    return 0;
}
