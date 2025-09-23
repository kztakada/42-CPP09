NAME			=	PmergeMe

CXX				=	c++
CXXFLAGS		=	-Wall -Wextra -Werror -std=c++98 -pedantic

SRCS			=	main.cpp PmergeMe.cpp

OBJS_PATH		=	objs/
OBJS			=	$(SRCS:%.cpp=objs/%.o)

ISDEBUG = 0
ARGS ?= 

ifeq ($(ISDEBUG), 1)
	CXX +=  -g -fsanitize=address,undefined
	CXXFLAGS += -DDEBUG
endif

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJS_PATH)%.o:	%.cpp
	@mkdir -p $(OBJS_PATH)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
debug:
	make fclean
	$(MAKE) ISDEBUG=1 all

va:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) $(ARGS)

clean:
	rm -rf $(OBJS_PATH)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

init:
	bear -- make

.PHONY:	all clean fclean re debug va init