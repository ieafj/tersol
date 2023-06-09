#define CARD_FULL "+----------+\033[B\033[12D| %c        |\033[B\033[12D| %c        |\033[B\033[12D|          |\033[B\033[12D|          |\033[B\033[12D|        %c |\033[B\033[12D|        %c |\033[B\033[12D+----------+"
#define CARD_HALF "+----------+\033[B\033[12D| %c        |\033[B\033[12D| %c        |\033[12D\033[B"
#define CARD_VHALF "+-----\033[B\033[6D| %c    \033[B\033[6D| %c    \033[B\033[6D|      \033[B\033[6D|      \033[B\033[6D|      \033[B\033[6D|      \033[B\033[6D+-----\033[B\033[6D"
#define CARD_EMPTY "            \033[B\033[12D            \033[B\033[12D            \033[B\033[12D            \033[B\033[12D            \033[B\033[12D            \033[B\033[12D            \033[B\033[12D            \033[B\033[12D"
#define CARD_BACK "+----------+\033[B\033[12D|[][][][][]|\033[B\033[12D|[][][][][]|\033[12D\033[B"

enum {SPADES, HEARTS, CLUBS, DIAMONDS};
enum {ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING};
enum {PILE, TABLEAU, ACES};

const char suitchar[] = {'S','H','C','D'};
const char cardchar[] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};

struct stack
{   int count;
    int visible;
    struct card *stack;
};

struct card
{   int suitval;
    int cardval;
};

struct data
{   int area;
    int stack;
    int card;
};

int menu();
int initialise();
void layout();
void cycle();
void extract(int column, int row, struct data *carddata);
int move(int src_column, int src_row, struct data *src, struct data *dst);
void collect();