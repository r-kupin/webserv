NAME = webserv
NAME_LIB = $(NAME)_lib
ASAN = $(NAME)_asan
TEST = $(NAME)_test

SRCS = src/main.cpp \
		src/Server/response/ServerResponse.cpp \
		src/Server/ServerInit.cpp \
		src/Server/Server.cpp \
		src/Server/ServerLocationSynthesizer.cpp \
		src/Server/request/ClientRequest.cpp \
		src/Server/ServerLocationSearcher.cpp \
		src/Server/ServerManager.cpp \
		src/Logger.cpp \
		src/Config/config/ConfigCreate.cpp \
		src/Config/config/Config.cpp \
		src/Config/config/ConfigParse.cpp \
		src/Config/Node.cpp \
		src/Config/server_configuration/ServerConfiguration.cpp \
		src/Config/server_configuration/ServerConfigurationLocationHandler.cpp \
		src/Config/location/ErrPage.cpp \
		src/Config/location/Location.cpp \
		src/Config/location/LocationHandleReturn.cpp \
		src/Config/location/LimitExcept.cpp
LIB_SRCS = src/Server/response/ServerResponse.cpp \
        src/Server/ServerInit.cpp \
        src/Server/Server.cpp \
        src/Server/ServerLocationSynthesizer.cpp \
        src/Server/request/ClientRequest.cpp \
        src/Server/ServerLocationSearcher.cpp \
        src/Server/ServerManager.cpp \
        src/Logger.cpp \
        src/Config/config/ConfigCreate.cpp \
        src/Config/config/Config.cpp \
        src/Config/config/ConfigParse.cpp \
        src/Config/Node.cpp \
        src/Config/server_configuration/ServerConfiguration.cpp \
        src/Config/server_configuration/ServerConfigurationLocationHandler.cpp \
        src/Config/location/ErrPage.cpp \
        src/Config/location/Location.cpp \
        src/Config/location/LocationHandleReturn.cpp \
        src/Config/location/LimitExcept.cpp
TEST_SRCS = tests/server_test/setup_test/ServerConfigTest.cpp \
		tests/server_test/functionality_test/ServerTest.cpp \
		tests/config_test/location_class_test/setup_test/HandleReturnTest.cpp \
		tests/config_test/location_class_test/setup_test/HandleAddErrPagesTest.cpp \
		tests/config_test/location_class_test/setup_test/HandleUpdateIndexTest.cpp \
		tests/config_test/location_class_test/setup_test/LocationMarkDirectiveTest.cpp \
		tests/config_test/location_class_test/setup_test/HandleLimitExceptTest.cpp \
		tests/config_test/location_class_test/setup_test/LocationProcessDirectivesTest.cpp \
		tests/config_test/location_class_test/setup_test/LocationConstructorTest.cpp \
        tests/config_test/location_class_test/functionality_test/LocationFindSublocationTest.cpp

TEST_LIB_DIR = test/lib
TEST_LIB_INCL_DIR = $(TEST_LIB_DIR)/googletest/include
TEST_LIB_BILD_DIR = $(TEST_LIB_DIR)/build
TEST_LIB_LIB_DIR = $(TEST_LIB_BILD_DIR)/lib
TEST_LIB_CMAKE = $(TEST_LIB_DIR)/Makefile
TEST_LIBS = $(TEST_LIB_LIB_DIR)/libgtest.a \
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

test/%.o: test/%.cpp
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