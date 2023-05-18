
add_executable(chat chat.cpp header.h utils.h parse_json.h ../gpt4all-backend/llmodel_c.h)
target_link_libraries(chat PRIVATE  llmodel llama)


