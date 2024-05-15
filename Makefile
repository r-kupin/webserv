# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/15 11:45:50 by mede-mas          #+#    #+#              #
#    Updated: 2024/05/15 18:23:59 by mede-mas         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
NAME_LIB = $(NAME)_lib
ASAN = $(NAME)_asan
TEST = $(NAME)_test

SRCS =	src/main.cpp \
		src/utils/Utils.cpp \
        src/utils/StringUtils.cpp \
        src/utils/FSUtils.cpp \
        src/Server/server/response/ResponseStaticUtils.cpp \
        src/Server/server/response/ServerResponse.cpp \
        src/Server/server/PerformUpload.cpp \
        src/Server/server/ServerExceptions.cpp \
        src/Server/server/SynthFoundExact.cpp \
        src/Server/server/Server.cpp \
        src/Server/server/LocationSynth.cpp \
        src/Server/server/CheckUpload.cpp \
        src/Server/server/ServerCGIHandler.cpp \
        src/Server/ServerManagerHandleEvents.cpp \
        src/Server/ServerManagerInit.cpp \
        src/Server/ServerManagerCGI.cpp \
        src/Server/connection/Connection.cpp \
        src/Server/connection/request/ClientRequest.cpp \
        src/Server/connection/request/ClientRequestURLHandlers.cpp \
        src/Server/connection/request/ClientRequestBodyProcessing.cpp \
        src/Server/connection/request/ClientRequestRequestHandlers.cpp \
        src/Server/connection/request/ClientRequestURLParamsHandlers.cpp \
        src/Server/connection/request/RequestExceptions.cpp \
        src/Server/ServerManagerRun.cpp \
        src/Server/ServerManagerUtils.cpp \
        src/Server/ServerManager.cpp \
        src/Config/config/ConfigCreate.cpp \
        src/Config/config/Config.cpp \
        src/Config/config/ConfigParse.cpp \
        src/Config/Node.cpp \
        src/Config/server_configuration/Host.cpp \
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
           	src/Server/server/response/ResponseStaticUtils.cpp \
           	src/Server/server/response/ServerResponse.cpp \
           	src/Server/server/CheckUpload.cpp \
           	src/Server/server/ServerExceptions.cpp \
           	src/Server/server/SynthFoundExact.cpp \
           	src/Server/server/Server.cpp \
           	src/Server/server/LocationSynth.cpp \
           	src/Server/server/PerformUpload.cpp \
           	src/Server/ServerManagerHandleEvents.cpp \
           	src/Server/ServerManagerInit.cpp \
           	src/Server/connection/Connection.cpp \
           	src/Server/connection/request/ClientRequest.cpp \
           	src/Server/connection/request/ClientRequestURLHandlers.cpp \
           	src/Server/connection/request/ClientRequestBodyProcessing.cpp \
           	src/Server/connection/request/ClientRequestRequestHandlers.cpp \
           	src/Server/connection/request/ClientRequestURLParamsHandlers.cpp \
           	src/Server/connection/request/RequestExceptions.cpp \
           	src/Server/ServerManagerRun.cpp \
           	src/Server/ServerManagerUtils.cpp \
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
           	src/Config/location/LimitExcept.cpp \
           	src/Config/server_configuration/Host.cpp

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
DEPS =		$(OBJS:.o=.d)		# Dependency files for each object file
LIB_OBJS =	$(LIB_SRCS:.cpp=.o)
TEST_OBJS =	$(TEST_SRCS:.cpp=.o)

CXX =		c++
GXX =		g++
LIB_CXX =	ar rvs

CXXFLAGS =		-Wall -Wextra -Werror -std=c++98
LINKER_FLAGS =	-lgtest -lgtest_main -pthread
ASANFLAGS =		-g -fsanitize=address
DFLAGS = 		-MMD -MP

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


# Rule to compile object files and generate dependency files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DFLAGS) -c $< -o $@

# Include dependency rules
-include $(DEPS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

unit_tests/%.o: unit_tests/%.cpp
	$(GXX) -I$(TEST_LIB_INCL_DIR) -c $< -o $@

clean:
	@rm -fr $(OBJS) $(DEPS)
	@rm -fr $(TEST_OBJS)

fclean: clean
	@rm -fr $(NAME)
	@rm -fr $(ASAN)
	@rm -fr $(TEST)
	@rm -fr $(NAME_LIB)

re:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re