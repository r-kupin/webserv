NAME = webserv
NAME_LIB = $(NAME)_lib
ASAN = $(NAME)_asan
TEST = $(NAME)_test

SRCS =	src/main.cpp \
		src/utils/Utils.cpp \
		src/utils/StringUtils.cpp \
		src/utils/FSUtils.cpp \
		src/Server/response/ResponseStaticUtils.cpp \
		src/Server/response/ServerResponse.cpp \
		src/Server/request_processing/UploadHandler.cpp \
		src/Server/request_processing/SynthFoundExact.cpp \
		src/Server/request_processing/LocationSynth.cpp \
		src/Server/request_processing/SynthNotFound.cpp \
		src/Server/request_processing/CURLUploadHandler.cpp \
		src/Server/server/ServerExceptions.cpp \
		src/Server/server/MultithreadServer.cpp \
		src/Server/server/Server.cpp \
		src/Server/server/Aserver.cpp \
		src/Server/server/ServerRun.cpp \
		src/Server/server/ServerInit.cpp \
		src/Server/thread_pool/ThreadPool.cpp \
		src/Server/request/ClientRequest.cpp \
		src/Server/request/ClientRequestURLHandlers.cpp \
		src/Server/request/ClientRequestBodyProcessing.cpp \
		src/Server/request/ClientRequestRequestHandlers.cpp \
		src/Server/request/ClientRequestURLParamsHandlers.cpp \
		src/Server/request/RequestExceptions.cpp \
		src/Server/ServerManager.cpp \
		src/Config/config/ConfigCreate.cpp \
		src/Config/config/Config.cpp \
		src/Config/config/ConfigParse.cpp \
		src/Config/Node.cpp \
		src/Config/server_configuration/ServerConfiguration.cpp \
		src/Config/server_configuration/ServerConfigurationLocationSearch.cpp \
		src/Config/server_configuration/ServerConfigurationLocationHandler.cpp \
		src/Config/location/ErrPage.cpp \
		src/Config/location/Location.cpp \
		src/Config/location/LocationHandleReturn.cpp \
        src/Config/location/LimitExcept.cpp
LIB_SRCS = 	src/utils/Utils.cpp \
			src/utils/StringUtils.cpp \
			src/utils/FSUtils.cpp \
			src/Server/response/ResponseStaticUtils.cpp \
			src/Server/response/ServerResponse.cpp \
			src/Server/request_processing/UploadHandler.cpp \
			src/Server/request_processing/SynthFoundExact.cpp \
			src/Server/request_processing/LocationSynth.cpp \
			src/Server/request_processing/SynthNotFound.cpp \
			src/Server/request_processing/CURLUploadHandler.cpp \
			src/Server/server/ServerExceptions.cpp \
			src/Server/server/MultithreadServer.cpp \
			src/Server/server/Server.cpp \
			src/Server/server/Aserver.cpp \
			src/Server/server/ServerRun.cpp \
			src/Server/server/ServerInit.cpp \
			src/Server/thread_pool/ThreadPool.cpp \
			src/Server/request/ClientRequest.cpp \
			src/Server/request/ClientRequestURLHandlers.cpp \
			src/Server/request/ClientRequestBodyProcessing.cpp \
			src/Server/request/ClientRequestRequestHandlers.cpp \
			src/Server/request/ClientRequestURLParamsHandlers.cpp \
			src/Server/request/RequestExceptions.cpp \
			src/Server/ServerManager.cpp \
			src/Config/config/ConfigCreate.cpp \
			src/Config/config/Config.cpp \
			src/Config/config/ConfigParse.cpp \
			src/Config/Node.cpp \
			src/Config/server_configuration/ServerConfiguration.cpp \
			src/Config/server_configuration/ServerConfigurationLocationSearch.cpp \
			src/Config/server_configuration/ServerConfigurationLocationHandler.cpp \
			src/Config/location/ErrPage.cpp \
			src/Config/location/Location.cpp \
			src/Config/location/LocationHandleReturn.cpp \
			src/Config/location/LimitExcept.cpp
TEST_SRCS = test/unit_tests/utils_test/UtilsTest.cpp \
            test/unit_tests/server_test/request_test/RequestLineLevelTest.cpp \
            test/unit_tests/server_test/request_test/URLLevelTest.cpp \
            test/unit_tests/server_test/request_test/SocketLevelTest.cpp \
            test/unit_tests/server_test/request_test/ParsedRequestLevelTest.cpp \
            test/unit_tests/server_test/response_test/SimpleResponseTest.cpp \
            test/unit_tests/server_test/response_test/SimpleIndexConfigResponseTest.cpp \
            test/unit_tests/server_test/functionality_test/SynthIndexTest.cpp \
            test/unit_tests/server_test/functionality_test/FindIndexTest.cpp \
            test/unit_tests/server_test/functionality_test/LocationSynthUtilsTest.cpp \
            test/unit_tests/server_test/functionality_test/SynthFileTest.cpp \
            test/unit_tests/server_test/functionality_test/LocationSynthTest_SimpleIndex.cpp \
            test/unit_tests/config_test/location_class_test/setup_test/HandleReturnTest.cpp \
            test/unit_tests/config_test/location_class_test/setup_test/HandleAddErrPagesTest.cpp \
            test/unit_tests/config_test/location_class_test/setup_test/HandleUpdateIndexTest.cpp \
            test/unit_tests/config_test/location_class_test/setup_test/LocationStaticToolsTest.cpp \
            test/unit_tests/config_test/location_class_test/setup_test/LocationMarkDirectiveTest.cpp \
            test/unit_tests/config_test/location_class_test/setup_test/HandleLimitExceptTest.cpp \
            test/unit_tests/config_test/location_class_test/setup_test/LocationProcessDirectivesTest.cpp \
            test/unit_tests/config_test/location_class_test/functionality_test/LocationFindSublocationTest.cpp \
            test/unit_tests/config_test/config_class_test/setup_from_file_config/ParsingToolsTest.cpp \
            test/unit_tests/config_test/config_class_test/setup_from_file_config/ConfigSetupFromFileTest.cpp \
            test/unit_tests/config_test/config_class_test/setup_from_file_config/ConfigFileCheckTest.cpp \
            test/unit_tests/config_test/config_class_test/setup_from_nodes/ConfigServerSubcontextsHandlingTest.cpp \
            test/unit_tests/config_test/config_class_test/setup_from_nodes/ConfigMainContextCheck.cpp \
            test/unit_tests/config_test/server_configuration_class_test/setup_test/HandleDirectivesTest.cpp \
            test/unit_tests/config_test/server_configuration_class_test/setup_test/HandleMultistepLocation.cpp \
            test/unit_tests/config_test/server_configuration_class_test/setup_test/HandleSingleStepLocation.cpp \
            test/unit_tests/config_test/server_configuration_class_test/functionality_test/NestedConfigLocationSearchTest.cpp \
            test/unit_tests/config_test/server_configuration_class_test/functionality_test/CheckCorrectLocationContextTest.cpp \
            test/unit_tests/config_test/server_configuration_class_test/functionality_test/SimpleConfigLocationSearchTest.cpp

TEST_LIB_DIR =		unit_tests/lib
TEST_LIB_INCL_DIR =	$(TEST_LIB_DIR)/googletest/include
TEST_LIB_BILD_DIR =	$(TEST_LIB_DIR)/build
TEST_LIB_LIB_DIR =	$(TEST_LIB_BILD_DIR)/lib
TEST_LIB_CMAKE =	$(TEST_LIB_DIR)/Makefile
TEST_LIBS =			$(TEST_LIB_LIB_DIR)/libgtest.a \
					$(TEST_LIB_LIB_DIR)/libgtest_main.a

OBJS =		$(SRCS:.cpp=.o)
LIB_OBJS =	$(LIB_SRCS:.cpp=.o)
TEST_OBJS =	$(TEST_SRCS:.cpp=.o)

CXX =		clang++
GXX =		g++
LIB_CXX =	ar rvs

CXXFLAGS =		-Wall -Wextra -Werror -std=c++98
LINKER_FLAGS =	-lgtest -lgtest_main -pthread
ASANFLAGS =		-g -fsanitize=address

all: $(NAME)

asan: $(ASAN)
	./$(ASAN)

valgrnd: $(NAME)
	valgrind ./$(NAME)

test: $(TEST)
	./$(TEST)

$(TEST_LIBS): $(TEST_LIB_DIR)
	make -C $(TEST_LIB_BILD_DIR)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(ASAN): $(OBJS)
	$(CXX) $(ASANFLAGS) $(CXXFLAGS) $(OBJS) -o $(ASAN)

$(NAME_LIB): $(LIB_OBJS)
	$(LIB_CXX) $(NAME_LIB) $(LIB_OBJS)

$(TEST): $(TEST_LIBS) $(TEST_OBJS) $(NAME_LIB)
	$(GXX) -L$(TEST_LIB_LIB_DIR) $(TEST_OBJS) $(NAME_LIB) $(LINKER_FLAGS) -no-pie -o $(TEST)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

unit_tests/%.o: unit_tests/%.cpp
	$(GXX) -I$(TEST_LIB_INCL_DIR) -c $< -o $@

clean:
	@rm -fr $(OBJS)
	@rm -fr $(TEST_OBJS)

fclean: clean
	@rm -fr $(NAME)
	@rm -fr $(ASAN)
	@rm -fr $(TEST)
	@rm -fr $(NAME_LIB)

re:
	@$(MAKE) fclean
	@$(MAKE) all