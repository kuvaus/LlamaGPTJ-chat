#include "./header.h"
#include "../gpt4all-backend/llmodel_c.h"
#include "./utils.h"
#include "./openai.h"
#include "./parse_json.h"


std::string hashstring = "";
std::string answer = "";

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
    check_avx_support_at_startup();


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

    //animation
    std::future<void> future;

    //ChatGPT interaction
    if(params.model == "gpt-3.5-turbo" || params.model == "gpt-4") { openai_chatgpt(params, con_st); }

    stop_display = true;
    if(params.use_animation) {stop_display = false; future = std::async(std::launch::async, display_loading);}

    //handle stderr for now
    //this is just to prevent printing unnecessary details during model loading.
    int stderr_copy = dup(fileno(stderr));
    #ifdef _WIN32
        std::freopen("NUL", "w", stderr);
    #else
        std::freopen("/dev/null", "w", stderr);
    #endif


    llmodel_model model = llmodel_model_create(params.model.c_str());
    std::cout << "\r" << APPNAME << ": loading " << params.model.c_str()  << std::endl;
    
    //bring back stderr for now
    dup2(stderr_copy, fileno(stderr));
    close(stderr_copy);
    
    

    //check if model is loaded
    auto check_model = llmodel_loadModel(model, params.model.c_str());

    if (check_model == false) {
        if(params.use_animation) {
            stop_display = true;
            future.wait();
            stop_display= false;
        }

        std::cerr << "Error loading: " << params.model.c_str() << std::endl;
        std::cout << "Press any key to exit..." << std::endl;
        std::cin.get();
        return 0;
    } else {
        if(params.use_animation) {
            stop_display = true;
            future.wait();
        }
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
        input = get_input(con_st, input, model);

        //Interactive mode. We have a prompt.
        if (params.prompt != "") {
            if (params.use_animation){ stop_display = false; future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (default_prefix + default_header + params.prompt + " " + input + default_footer).c_str(),
            prompt_callback, response_callback, recalculate_callback, &prompt_context);
            if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
            if (params.save_log != ""){ save_chat_log((default_prefix + default_header + params.prompt + " " + input + default_footer).c_str(), params.save_log, answer.c_str()); }

        //Interactive mode. Else get prompt from input.
        } else {
            if (params.use_animation){ stop_display = false; future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (default_prefix + default_header + input + default_footer).c_str(),
            prompt_callback, response_callback, recalculate_callback, &prompt_context);
            if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
            if (params.save_log != ""){ save_chat_log(params.save_log, (default_prefix + default_header + input + default_footer).c_str(), answer.c_str()); }
        }
        //Interactive and continuous mode. Get prompt from input.

        while (!params.run_once) {
            answer = ""; //New prompt. We stored previous answer in memory so clear it.
            input = get_input(con_st, input, model);
            if (params.use_animation){ stop_display = false; future = std::async(std::launch::async, display_frames); }
            llmodel_prompt(model, (default_prefix + default_header + input + default_footer).c_str(), 
            prompt_callback, response_callback, recalculate_callback, &prompt_context);
            if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
            if (params.save_log != ""){ save_chat_log(params.save_log, (default_prefix + default_header + input + default_footer).c_str(), answer.c_str()); }

        }

    //No-interactive mode. Get the answer once from prompt and print it.
    } else {
        if (params.use_animation){ stop_display = false; future = std::async(std::launch::async, display_frames); }
        llmodel_prompt(model, (default_prefix + default_header + params.prompt + default_footer).c_str(), 
        prompt_callback, response_callback, recalculate_callback, &prompt_context);
        if (params.use_animation){ stop_display = true; future.wait(); stop_display = false; }
        if (params.save_log != ""){ save_chat_log(params.save_log, (default_prefix + default_header + params.prompt + default_footer).c_str(), answer.c_str()); }
        std::cout << std::endl;
    }


    set_console_color(con_st, DEFAULT);
    llmodel_model_destroy(model);
    return 0;
}
