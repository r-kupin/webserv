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
ASAN = $(NAME)_asan

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

OBJS =		$(SRCS:.cpp=.o)
DEPS =		$(OBJS:.o=.d)		# Dependency files for each object file

CXX =		c++

CXXFLAGS =		-Wall -Wextra -Werror -std=c++98
ASANFLAGS =		-g -fsanitize=address
DFLAGS = 		-MMD -MP

all: $(NAME)

asan: $(ASAN)
	./$(ASAN)

valgrnd: $(NAME)
	valgrind ./$(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# Rule to compile object files and generate dependency files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DFLAGS) -c $< -o $@

# Include dependency rules
-include $(DEPS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -fr $(OBJS) $(DEPS)

fclean: clean
	@rm -fr $(NAME)
	@rm -fr $(ASAN)

re:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re