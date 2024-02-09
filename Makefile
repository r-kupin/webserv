NAME = webserv
NAME_LIB = $(NAME)_lib
ASAN = $(NAME)_asan
TEST = $(NAME)_test

SRCS = src/main.cpp \
			src/Config/config/Config.cpp \
        	src/Server/Server.cpp \
        	src/Config/config/ConfigParse.cpp \
        	src/Config/config/ConfigCreate.cpp \
        	src/Server/ServerManager.cpp \
        	src/Config/server_configuration/ServerConfiguration.cpp \
        	src/Server/request/ClientRequest.cpp \
        	src/Config/location/Location.cpp \
        	src/Config/location/ErrPage.cpp \
        	src/Server/response/ServerResponse.cpp \
        	src/Config/location/LimitExcept.cpp \
        	src/Config/server_configuration/ServerConfigurationLocationHandler.cpp \
        	src/Config/Node.cpp \
        	src/Config/location/LocationHandleReturn.cpp \
        	src/Server/ServerInit.cpp \
        	src/Server/ServerExceptions.cpp \
        	src/Server/request/RequestExceptions.cpp \
        	src/Server/request/ClientRequestURLHandlers.cpp \
        	src/Server/request/ClientRequestRequestHandlers.cpp \
        	src/Server/request/ClientRequestURLParamsHandlers.cpp \
        	src/Config/server_configuration/ServerConfigurationLocationSearch.cpp \
        	src/Server/location_synth/SynthFoundExact.cpp \
        	src/Server/location_synth/SynthNotFound.cpp \
        	src/Server/location_synth/LocationSynth.cpp \
        	src/Server/response/ResponseStaticUtils.cpp \
        	src/utils/Utils.cpp
LIB_SRCS = 	src/Config/config/Config.cpp \
            src/Server/Server.cpp \
            src/Config/config/ConfigParse.cpp \
            src/Config/config/ConfigCreate.cpp \
            src/Server/ServerManager.cpp \
            src/Config/server_configuration/ServerConfiguration.cpp \
            src/Server/request/ClientRequest.cpp \
            src/Config/location/Location.cpp \
            src/Config/location/ErrPage.cpp \
            src/Server/response/ServerResponse.cpp \
            src/Config/location/LimitExcept.cpp \
            src/Config/server_configuration/ServerConfigurationLocationHandler.cpp \
            src/Config/Node.cpp \
            src/Config/location/LocationHandleReturn.cpp \
            src/Server/ServerInit.cpp \
            src/Server/ServerExceptions.cpp \
            src/Server/request/RequestExceptions.cpp \
            src/Server/request/ClientRequestURLHandlers.cpp \
            src/Server/request/ClientRequestRequestHandlers.cpp \
            src/Server/request/ClientRequestURLParamsHandlers.cpp \
            src/Config/server_configuration/ServerConfigurationLocationSearch.cpp \
            src/Server/location_synth/SynthFoundExact.cpp \
            src/Server/location_synth/SynthNotFound.cpp \
            src/Server/location_synth/LocationSynth.cpp \
            src/Server/response/ResponseStaticUtils.cpp \
            src/utils/Utils.cpp
TEST_SRCS = unit_tests/tests/utils_test/UtilsTest.cpp \
            unit_tests/tests/server_test/request_test/RequestLineLevelTest.cpp \
            unit_tests/tests/server_test/request_test/URLLevelTest.cpp \
            unit_tests/tests/server_test/request_test/SocketLevelTest.cpp \
            unit_tests/tests/server_test/request_test/ParsedRequestLevelTest.cpp \
            unit_tests/tests/server_test/functionality_test/SynthIndexTest.cpp \
            unit_tests/tests/server_test/functionality_test/FindIndexTest.cpp \
            unit_tests/tests/server_test/functionality_test/LocationSynthUtilsTest.cpp \
            unit_tests/tests/server_test/functionality_test/LocationSynthTest_SimpleIndex.cpp \
            unit_tests/tests/server_test/functionality_test/SynthFileTest.cpp \
            unit_tests/tests/config_test/location_class_test/setup_test/HandleReturnTest.cpp \
            unit_tests/tests/config_test/location_class_test/setup_test/HandleAddErrPagesTest.cpp \
            unit_tests/tests/config_test/location_class_test/setup_test/HandleUpdateIndexTest.cpp \
            unit_tests/tests/config_test/location_class_test/setup_test/LocationStaticToolsTest.cpp \
            unit_tests/tests/config_test/location_class_test/setup_test/LocationMarkDirectiveTest.cpp \
            unit_tests/tests/config_test/location_class_test/setup_test/HandleLimitExceptTest.cpp \
            unit_tests/tests/config_test/location_class_test/setup_test/LocationProcessDirectivesTest.cpp \
            unit_tests/tests/config_test/location_class_test/functionality_test/LocationFindSublocationTest.cpp \
            unit_tests/tests/config_test/config_class_test/setup_from_file_config/ConfigSetupFromFileTest.cpp \
            unit_tests/tests/config_test/config_class_test/setup_from_file_config/ConfigFileCheckTest.cpp \
            unit_tests/tests/config_test/config_class_test/setup_from_nodes/ConfigServerSubcontextsHandlingTest.cpp \
            unit_tests/tests/config_test/config_class_test/setup_from_nodes/ConfigMainContextCheck.cpp \
            unit_tests/tests/config_test/server_configuration_class_test/setup_test/HandleDirectivesTest.cpp \
            unit_tests/tests/config_test/server_configuration_class_test/setup_test/HandleMultistepLocation.cpp \
            unit_tests/tests/config_test/server_configuration_class_test/setup_test/HandleSingleStepLocation.cpp \
            unit_tests/tests/config_test/server_configuration_class_test/functionality_test/NestedConfigLocationSearchTest.cpp \
            unit_tests/tests/config_test/server_configuration_class_test/functionality_test/CheckCorrectLocationContextTest.cpp \
            unit_tests/tests/config_test/server_configuration_class_test/functionality_test/SimpleConfigLocationSearchTest.cpp \
            unit_tests/tests/config_test/config_class_test/setup_from_file_config/ParsingToolsTest.cpp

TEST_LIB_DIR = unit_tests/lib
TEST_LIB_INCL_DIR =	$(TEST_LIB_DIR)/googletest/include
TEST_LIB_BILD_DIR =	$(TEST_LIB_DIR)/build
TEST_LIB_LIB_DIR =	$(TEST_LIB_BILD_DIR)/lib
TEST_LIB_CMAKE =	$(TEST_LIB_DIR)/Makefile
TEST_LIBS =			$(TEST_LIB_LIB_DIR)/libgtest.a \
					$(TEST_LIB_LIB_DIR)/libgtest_main.a

OBJS = $(SRCS:.cpp=.o)
LIB_OBJS = $(LIB_SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

CXX = clang++
GXX = g++
LIB_CXX = ar rvs

CXXFLAGS = -Wall -Wextra -Werror -std=c++98
LINKER_FLAGS = -lgtest -lgtest_main -pthread
ASANFLAGS = -g -fsanitize=address

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