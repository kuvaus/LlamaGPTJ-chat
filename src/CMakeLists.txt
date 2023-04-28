
add_executable(chat chat.cpp header.h utils.h parse_json.h ../llmodel/llamamodel.h ../llmodel/llamamodel.cpp   ../llmodel/gptj.h  ../llmodel/gptj.cpp   ../llmodel/llmodel.h   ../llmodel/llmodel_c.h  )
target_link_libraries(chat PRIVATE  llmodel llama)


