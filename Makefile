CFLAGS = -g -Wall -Werror 

OBJS = \
character.o \
core.o \
corridor.o \
debug.o \
dice.o \
dijkstra.o \
dungeon.o \
equipment.o \
heap.o \
inventory.o \
item.o \
main.o \
monsterfactory.o \
move.o \
npc.o \
objectfactory.o \
parser.o \
pc.o \
point.o \
room.o \
turn.o \
ui.o \

NAME = rlg327

all: $(NAME)

clean:
	rm -f $(NAME) *.o .DS_Store *.log

$(NAME): $(OBJS) 
	g++ $(CFLAGS) -o $(NAME) $(OBJS) -lncurses

%.o: %.cpp
	g++ $(CFLAGS) -c $<

