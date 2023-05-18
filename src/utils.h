#pragma once

#ifndef UTILS_H
#define UTILS_H

#include "header.h" 

//Need this for Windows colors
#ifdef _WIN32
    #include <windows.h> 
#endif


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
////////////                 CHAT FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////



std::string get_input(ConsoleState& con_st, std::string& input, llmodel_model model = nullptr) {
    set_console_color(con_st, USER_INPUT);

    std::cout << "\n> ";
    std::getline(std::cin, input);
    set_console_color(con_st, DEFAULT);

    if (input == "exit" || input == "quit") {       
        llmodel_model_destroy(model);
        exit(0);
    }

    return input;
}


void save_chat_log(std::string save_log, std::string prompt, std::string answer) {
  std::ofstream logfile(save_log, std::ios::app);
  if (logfile.is_open()) {
    logfile << prompt;
    logfile << answer+"\n";
    logfile.close();
    }
}


std::string random_prompt(int32_t seed) {
    const std::vector<std::string> prompts = {
        "So", "Once upon a time", "When", "The", "After", "If", "import", "He", "She", "They"
    };

    std::mt19937 rng(seed);
    return prompts[rng() % prompts.size()];
}


//////////////////////////////////////////////////////////////////////////
////////////                /CHAT FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
////////////                 UTIL FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////


bool containsSubstring(const std::string &str, const std::string &substr) {
    return str.find(substr) != std::string::npos;
}

void check_avx_support_at_startup() {
#if defined(__x86_64__) || defined(__i386__)
    const bool avx(__builtin_cpu_supports("avx"));
    const bool avx2(__builtin_cpu_supports("avx2"));
    const bool avx512(__builtin_cpu_supports("avx512f"));
    const bool fma(__builtin_cpu_supports("fma"));
    if (avx512 && avx && avx2 && fma) {std::cout << "Your computer supports AVX512" << std::endl;}
    else if (avx && avx2 && fma)      {std::cout << "Your computer supports AVX2" << std::endl;}
    else if (avx)                     {std::cout << "Your computer only supports AVX1" << std::endl;}
    else                    {std::cout << "Your computer does not support AVX1 or AVX2\nThe program will likely not run." << std::endl;} 
#endif
}

//////////////////////////////////////////////////////////////////////////
////////////                /UTIL FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
////////////            READ PROMPT TEMPLATE FILE             ////////////
//////////////////////////////////////////////////////////////////////////

//This is a bit messy function but it should parse the template file into header, prompt, and footer.
std::tuple<std::string, std::string, std::string> read_prompt_template_file(const std::string& file_path) {
    std::string prompt, header, footer;
    std::ifstream file(file_path);

    std::vector<std::string> lines;
    std::string line;

    //store all lines of prompt template into a vector
    if (file.is_open()) {
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    } else {
        std::cerr << "Unable to open the prompt template file." << std::endl;
        std::cerr << "Reverting to default prompt template." << std::endl;
        return std::make_tuple("### Instruction:\n The prompt below is a question to answer, a task to complete, or a conversation to respond to; decide which and write an appropriate response.", "\n### Prompt: ", "\n### Response: "); 
    }

    //find line containing %1 and store its index.
    int input_index;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find("%1") != std::string::npos) {
            input_index = i;
        }
    }
    //If there is only 1 line above %1, that will be ### prompt.
    if (input_index == 1) {
        prompt = lines[0];
        header = " ";
    } else {
        
        //Put lines above the prompt-line into header.
        header = lines[0];
        for (size_t i = 1; i < input_index-1; ++i) {
            header = header + "\n" + lines[i];
        }
        header = header  + " ";

        //store prompt-line (line above input-line)
        prompt = "\n" + lines[input_index-1] + " ";

        //Put lines below the input-line into footer.
        footer = "\n";
        for (size_t i = input_index+1; i < lines.size(); ++i) {
             footer = footer + lines[i]+" ";
        }
    }

    return std::make_tuple(header, prompt, footer);
}


//////////////////////////////////////////////////////////////////////////
////////////           /READ PROMPT TEMPLATE FILE             ////////////
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
////////////             CONSOLE COLOR FUNCTION               ////////////
//////////////////////////////////////////////////////////////////////////


void set_console_color(ConsoleState &con_st, ConsoleColor color) {
    if (con_st.use_color && con_st.color != color) {
        //Windows handles colors differently.
        #ifdef _WIN32
          WORD windows_colors[] = {
            7, 14, 10, 15
        };          
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, windows_colors[color]);
        #else
        //ANSI colors, works for unix.
        const char* ansi_colors[] = {
            //DEFAULT, PROMPT, USER_INPUT,   BOLD
            //default, yellow, bright_green, bold
            "\x1b[0m", "\x1b[33m", "\x1b[1m\x1b[32m", "\x1b[1m"
        };
        printf("%s", ansi_colors[color]);
        #endif
        con_st.color = color;
    }
}

//////////////////////////////////////////////////////////////////////////
////////////            /CONSOLE COLOR FUNCTION               ////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
////////////                PARSE PARAMETERS                  ////////////
//////////////////////////////////////////////////////////////////////////


void print_usage(int argc, char** argv, const chatParams& params) {
    // Print usage information
    fprintf(stderr, "usage: %s [options]\n", argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "A simple chat program for GPT-J, LLaMA, and MPT models.\n");
    fprintf(stderr, "You can set specific initial prompt with the -p flag.\n");
    fprintf(stderr, "Runs default in interactive and continuous mode.\n");
    fprintf(stderr, "Type 'quit', 'exit' or, 'Ctrl+C' to quit.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "  -h, --help            show this help message and exit\n");
    fprintf(stderr, "  --run-once            disable continuous mode\n");
    fprintf(stderr, "  --no-interactive      disable interactive mode altogether (uses given prompt only)\n");
    fprintf(stderr, "  --no-animation        disable chat animation\n");
    fprintf(stderr, "  -s SEED, --seed SEED  RNG seed (default: -1)\n");
    fprintf(stderr, "  -t N, --threads    N  number of threads to use during computation (default: %d)\n", params.n_threads);
    fprintf(stderr, "  -p PROMPT, --prompt PROMPT\n");
    fprintf(stderr, "                        prompt to start generation with (default: empty)\n");
    fprintf(stderr, "  --random-prompt       start with a randomized prompt.\n");
    fprintf(stderr, "  -n N, --n_predict  N  number of tokens to predict (default: %d)\n", params.n_predict);
    fprintf(stderr, "  --top_k            N  top-k sampling (default: %d)\n", params.top_k);
    fprintf(stderr, "  --top_p            N  top-p sampling (default: %.1f)\n", params.top_p);
    fprintf(stderr, "  --temp             N  temperature (default: %.1f)\n", params.temp);
    fprintf(stderr, "  --n_ctx            N  number of tokens in context window (default: %d)\n", params.n_ctx);
    fprintf(stderr, "  -b N, --batch_size N  batch size for prompt processing (default: %d)\n", params.n_batch);
    fprintf(stderr, "  --repeat_penalty   N  repeat_penalty (default: %.1f)\n", params.repeat_penalty);
    fprintf(stderr, "  --repeat_last_n    N  last n tokens to penalize  (default: %d)\n", params.repeat_last_n);
    fprintf(stderr, "  --context_erase    N  percent of context to erase  (default: %.1f)\n", params.context_erase);
    fprintf(stderr, "  --openai_api_key  FNAME or $OPENAI_API_KEY\n");
    fprintf(stderr, "                        Optional: for -m \"gpt-3-turbo\" or -m \"gpt-4\" (default: empty/no)\n");
    fprintf(stderr, "  -j,   --load_json FNAME\n");
    fprintf(stderr, "                        load options instead from json at FNAME (default: empty/no)\n");
    fprintf(stderr, "  --load_template   FNAME\n");
    fprintf(stderr, "                        load prompt template from a txt file at FNAME (default: empty/no)\n");
    fprintf(stderr, "  --save_log        FNAME\n");
    fprintf(stderr, "                        save chat log to a file at FNAME (default: empty/no)\n");
    fprintf(stderr, "  -m FNAME, --model FNAME\n");
    fprintf(stderr, "                        model path (current: %s)\n", params.model.c_str());
    fprintf(stderr, "\n");
}

bool parse_params(int argc, char** argv, chatParams& params) { 

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-j" || arg == "--load_json") {
            params.load_json = argv[++i];
            if (!params.load_json.empty()) {
                std::cout << APPNAME << ": parsing options from json: " << params.load_json << std::endl;
                get_params_from_json(params);
            } else {
                std::cout << APPNAME << ": trying to parse options from json but got empty filename." << std::endl;
            }
        } else if (arg == "--run-once") {
            params.run_once = true;
        } else if (arg == "--no-interactive") {
            params.no_interactive = true;
        } else if (arg == "--no-animation") {
            params.use_animation = false;
        } else if (arg == "-s" || arg == "--seed") {
            params.seed = static_cast<int32_t>(std::stoi(argv[++i]));
        } else if (arg == "-t" || arg == "--threads") {
            params.n_threads = static_cast<int32_t>(std::stoi(argv[++i]));
        } else if (arg == "-p" || arg == "--prompt") {
            params.prompt = argv[++i];
        } else if (arg == "--random-prompt") {
            params.prompt = random_prompt(params.seed);
        } else if (arg == "-n" || arg == "--n_predict") {
            params.n_predict = static_cast<int32_t>(std::stoi(argv[++i]));
        } else if (arg == "--top_k") {
            params.top_k = static_cast<int32_t>(std::stoi(argv[++i]));
        } else if (arg == "--top_p") {
            params.top_p = static_cast<float>(std::stof(argv[++i]));
        } else if (arg == "--temp") {
            params.temp = static_cast<float>(std::stof(argv[++i]));
        } else if (arg == "-b" || arg == "--batch_size") {
            params.n_batch = static_cast<int32_t>(std::stoi(argv[++i]));
        } else if (arg == "--n_ctx") {
            params.n_ctx = static_cast<int>(std::stoi(argv[++i]));
        } else if (arg == "--repeat_penalty") {
            params.repeat_penalty = static_cast<float>(std::stof(argv[++i]));
        } else if (arg == "--repeat_last_n") {
            params.repeat_last_n = static_cast<int>(std::stoi(argv[++i]));
        } else if (arg == "--context_erase") {
            params.context_erase = static_cast<float>(std::stof(argv[++i]));
        } else if (arg == "--load_template") {
            params.load_template = argv[++i];
        } else if (arg == "--save_log") {
            params.save_log = argv[++i];
        } else if (arg == "--openai_api_key") {
            params.openai_api_key = argv[++i];
        } else if (arg == "-m" || arg == "--model") {
            params.model = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            print_usage(argc, argv, params);
            exit(0);
        } else {
            fprintf(stderr, "error: unknown argument: %s\n", arg.c_str());
            print_usage(argc, argv, params);
            exit(0);
        }
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
////////////               /PARSE PARAMETERS                  ////////////
//////////////////////////////////////////////////////////////////////////

#endif
