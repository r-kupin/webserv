NAME = webserv
NAME_LIB = $(NAME)_lib
ASAN = $(NAME)_asan
TEST = $(NAME)_test

SRCS = src/main.cpp \
		src/Config/Config.cpp \
		src/Server/Server.cpp \
		src/Config/ConfigParser.cpp \
		src/Config/ChonfigProcessingUtils.cpp \
		src/Config/ConfigChecker.cpp \
		src/Server/ServerManager.cpp \
		src/Config/ConfigLocationChecker.cpp \
		src/Config/ConfigSubmodules.cpp \
		src/Server/ClientRequest.cpp \
		src/Config/Location.cpp \
		src/Config/ErrPage.cpp \
		src/Server/ServerResponse.cpp
LIB_SRCS = src/Config/Config.cpp \
            src/Config/Config.cpp \
			src/Server/Server.cpp \
			src/Config/ConfigParser.cpp \
			src/Config/ChonfigProcessingUtils.cpp \
			src/Config/ConfigChecker.cpp \
			src/Server/ServerManager.cpp \
			src/Config/ConfigLocationChecker.cpp \
			src/Config/ConfigSubmodules.cpp \
			src/Server/ClientRequest.cpp \
			src/Config/Location.cpp \
			src/Config/ErrPage.cpp \
			src/Server/ServerResponse.cpp
TEST_SRCS = test/ConfigParsingTest.cpp \
			test/ConfigProcessingTest.cpp \
			test/LocationNodeTest.cpp \
			test/LocationTest.cpp \
			test/ServerConfigTest.cpp \
			test/LimitExceptNodeTest.cpp \
			test/MainNodeTest.cpp \
			test/ServerNodeTest.cpp \
			test/CheckServerTest.cpp \
            test/ServerTest.cpp

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