int menu();
int initialise();
void layout();
void cycle();
int move(int src_col, int src_row, int dst_col, int dst_row);
void collect();

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