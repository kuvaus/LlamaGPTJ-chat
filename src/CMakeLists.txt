
add_executable(chat chat.cpp header.h utils.h parse_json.h ../gpt4all-backend/llamamodel.h ../gpt4all-backend/llamamodel.cpp   ../gpt4all-backend/gptj.h  ../gpt4all-backend/gptj.cpp ../gpt4all-backend/mpt.cpp ../gpt4all-backend/mpt.h  ../gpt4all-backend/llmodel.h   ../gpt4all-backend/llmodel_c.h  )
target_link_libraries(chat PRIVATE  llmodel llama)


