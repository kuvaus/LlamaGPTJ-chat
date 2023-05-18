
add_executable(chat chat.cpp header.h utils.h parse_json.h ../gpt4all-backend/llmodel_c.h  yhirose_cpp_httplib.h olrea_openai_cpp.h nlohmann_json.h)
target_link_libraries(chat PRIVATE  llmodel llama)


