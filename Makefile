NAME =		codexion

SRC_DIR =	coders/
SRC =		main.c \
			parser.c
HDR =		$(SRC_DIR)codexion.h

OBJ_DIR =	obj/
OBJ =		$(addprefix $(OBJ_DIR), $(SRC:.c=.o))

CC =		cc

CFLAGS =	-Wall -Wextra -Werror -pthread

all:			$(NAME)

$(NAME):		$(OBJ)
				$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR):
				mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o:	$(SRC_DIR)%.c $(HDR) | $(OBJ_DIR)
				$(CC) $(CFLAGS) -c $< -o $@

clean:
				rm -rf $(OBJ_DIR)

fclean:			clean
				rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re
