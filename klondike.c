#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <conio.h>

int menu();
int initialise();
void layout();
void cycle();
int getstack(int pX);
int move(int oX, int oY, int dX, int dY);
void collect();

enum {SPADES, HEARTS, CLUBS, DIAMONDS} suits;
enum {ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING} cards;
enum {PILE, TABLEAU, ACES} locations;

const char suitchar[] = {'S','H','C','D'};
const char cardchar[] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};

_Bool wincon;

struct card
{   int suitval;
    int cardval;
};

struct stack
{   int count;
    int visible;
    struct card *stack;
} tableau[7], aces[4], pile;

struct stack *pta[3] = {&pile, &tableau[0], &aces[0]};

int main(void)
{   printf("\033[H\033[2J"); //"cls"

    while(menu() == 0)
    {   if(initialise() != 0) return -1;

        layout();
        printf("\033[H");

        int inp;
        int posX = 1, posY = 1;
        int originX, originY;

        _Bool selected = 0, persist = 1;
        
        wincon = 0;

        clock_t start = clock() / 60;

        while((inp = getch()) != '\b' && inp != 3 && wincon == 0)
        {   switch(inp)
            {   case '\t': cycle(); selected = 0; posX = 1; posY = 1; break;
 
                case '`': selected = 0; posX = 1; posY = 1; break;
                
                case '1': posY = 1; posX = 49; break;
                case '2': posY = 1; posX = 65; break;
                case '3': posY = 1; posX = 81; break;
                case '4': posY = 1; posX = 97; break;

                case 'A':
                case 'a': 
                    if(posY < 10)
                    {   if(posX == 49) posX -= 48; 
                        else if(posX > 49) posX -= 16;
                    }
                    else
                    {   if(posX > 1)
                        {   int gs = getstack(posX) - 1;
                            posX -= 16; 

                            if(selected == 0) posY = 11 + (tableau[gs].visible * 3);
                            else if(tableau[gs].count == 0) posY = 11;
                            else posY = 11 + ((tableau[gs].count - 1) * 3);
                        }
                    }
                    break;

                case 'D':
                case 'd':
                    if(posY < 11)
                    {   if(posX < 49) posX += 48;
                        else if(posX < 97) posX += 16;
                    } 
                    else if(posX < 97)
                    {   int gs = getstack(posX) + 1;
                        posX += 16; 

                        if(selected == 0) posY = 11 + (tableau[gs].visible * 3);
                        else if(tableau[gs].count == 0) posY = 11;
                        else posY = 11 + ((tableau[gs].count - 1) * 3);
                    }
                    break;

                case 'W':
                case 'w': 
                    if(posY == 11)
                    {   if(posX == 17) posX = 1, posY = 1;
                        else if(posX == 33) posX = 49, posY = 1;
                        else posY -= 10;
                    }
                    else if(posY > 11)
                    {   if(posY == 11 + (tableau[getstack(posX)].visible * 3))
                        {   if(posX == 17) posX = 1, posY = 1;
                            else if(posX == 33) posX = 49, posY = 1;
                            else posY -= 10 + (tableau[getstack(posX)].visible * 3);
                        }
                        else posY -= 3;
                    }
                    break;

                case 'S':
                case 's': 
                    if(posY == 1)
                    {   if(selected == 1 || pile.count == 0)
                        {   posY += 10 + ((tableau[getstack(posX)].count - 1) * 3);
                            if(posY < 11) posY = 11;
                        }
                        else posY += 10 + (tableau[getstack(posX)].visible * 3);
                    }
                    else if(posY < 11 + ((tableau[getstack(posX)].count - 1) * 3)) posY += 3;
                    
                    break;

                case ' ':
                case '\n':
                case '\r':
                if(selected == 0)
                {   originX = posX;
                    originY = posY;
                    selected = 1;
                }
                else
                {   if(move(originX, originY, posX, posY) == 0)
                    {   if(pile.count == 0 && persist == 1)
                        {   for(int i = 0; i < 7; i++)
                            {   if(tableau[i].visible != 0) break;
                                if(i == 6)
                                {   printf("\033[H\033[2JAuto-complete? (Any key but backspace): ");
                                    int response = getch();
                                    if(response == 3) exit(-1);
                                    else if(response != '\b') wincon = 1;
                                    else persist = 0;
                                }
                            }
                            if(wincon == 1) break;
                        }   
                        layout();
                    }
                    else
                    {   printf("\033[H\033[8B");
                        printf("\n\t\t\t\t\t\tInvalid move.");
                        int block = getch();
                        if(block == '\b' || block == 3) inp = '\b';
                        printf("\033[2K\033[A");
                    }
                    selected = 0;
                    break;
                }
                
                default: break;
            }
            printf("\033[%d;%dH", posY, posX);
        }
        printf("\033[H\033[2J");

        collect();

        if(wincon == 1)
        {   printf("Game complete.\n");
            printf("Elapsed time: %.1f minutes", (double) (start - (clock() / 60)) / CLOCKS_PER_SEC); //prone to overflow
            int block = getch();
            if(block == '\b' || block == 3) break;
            printf("\033[H\033[2J");
        }
    }
    return 0;
}

int menu()
{   printf("Start | Help | Exit\r");

    int inp, response = 1;
    
    while(inp = getch())
    {   switch(inp)
        {   case 3:
            case '\b':
                printf("\033[H\033[2J");
                return -1;
            
            case '\r':
            case '\n':
                if(response == 1)
                {   printf("\r\033[K");
                    return 0;
                }
                else if(response == 9) inp = 'h';
                else
                {   printf("\r\033[K");
                    return -1;
                }

            case 'H':
            case 'h': 
                printf("\r\033[KKlondike ruleset. WASD to move, Enter or Space to select. TAB shuffles free pile. Backspace to exit at any point. Keys 1-4 move to the respective ace pile. Backtick/grave to deselect and move to the free pile.");
                int block = getch();
                if(block == '\b' || block == 3) return -1;
                printf("\033[H\033[2J");
                printf("Start | Help | Exit\r");
                break;

            case 'A':
            case 'a':
                if(response == 9) response = 1;
                else if(response == 16) response = 9;
                break;

            case 'D':
            case 'd':
                if(response == 1) response = 9;
                else if(response == 9) response = 16;
                break;
            
            default: break;
        }
        printf("\033[1;%dH", response);
    }
}

int initialise()
{   struct card *deck = malloc(52 * sizeof(struct card)); //gen deck of 52
    if(deck == NULL) return -1;

    for(int c = 0, s = 0; c < 52; c++)
    {   deck[c].suitval = s;
        deck[c].cardval = c % 13;
        if(deck[c].cardval == 12) s++;
    }
    
    srand(time(NULL)); //shuffle

    for(int i = 0; i < 52; i++)
    {   struct card temp = {deck[i].suitval, deck[i].cardval};
        int rcard = rand() % 52;
        deck[i] = deck[rcard];
        deck[rcard] = temp;
    }
    
    pile.stack = malloc(24 * sizeof(struct card)); //gen pile
    if(pile.stack == NULL)
    {   free(deck);
        return -1;
    }
    memcpy(pile.stack, deck + 28, (24 * sizeof(struct card))); //move rear 24 cards to pile
    pile.count = 24;

    for(int i = 0, nthtri = 0; i < 7; i++) //define tableau
    {   tableau[i].stack = calloc((13 + i), sizeof(struct card)); //ace to king on stack[i] unturned card
        if(tableau[i].stack == NULL)
        {   free(deck);
            free(pile.stack);
            for(int j = 0; j < i; j++) free(tableau[j].stack);
            return -1;
        }
        tableau[i].count = i + 1;
        memcpy(tableau[i].stack, deck + nthtri, (tableau[i].count * sizeof(struct card)));  
        nthtri += tableau[i].count;
        tableau[i].visible = i; 
    }

    free(deck);

    for(int i = 0; i < 4; i++) //declare ace card piles
    {   aces[i].stack = calloc(13, sizeof(struct card));
        if(aces[i].stack == NULL)
        {   collect();
            return -1;
        }
        aces[i].count = 0;
    }

    return 0;
}

void cycle()
{   if(pile.count < 2) return;

    struct card last;
    memcpy(&last, &pile.stack[pile.count - 1], sizeof(struct card));

    for(int i = pile.count - 1; i > -1; i--)
    {   memcpy(&pile.stack[i], &pile.stack[i - 1], sizeof(struct card));
    }
    memcpy(&pile.stack[0], &last, sizeof(struct card));

    printf("\033[H");

    printf("+----------+\033[B\r| %c        |\033[B\r| %c        |\033[B\r|          |\033[B\r|          |\033[B\r|        %c |\033[B\r|        %c |\033[B\r+----------+\033[7A\t\t\t\t\t", cardchar[pile.stack[pile.count - 1].cardval], suitchar[pile.stack[pile.count - 1].suitval], suitchar[pile.stack[pile.count - 1].suitval], cardchar[pile.stack[pile.count - 1].cardval]);
}

int getstack(int pX)
{   switch(pX)
    {   case 1: return 0;
        case 17: return 1;
        case 33: return 2;
        case 49: return 3;
        case 65: return 4;
        case 81: return 5;
        case 97: return 6;
    }
}

int move(int oX, int oY, int dX, int dY)
{   int o_locale, o_stack, o_card, d_locale, d_stack, d_card; 
    
    if(oY == 1)
    {   switch(oX)
        {   case 1: o_locale = PILE; o_stack = 0; o_card = pile.count - 1; break;
            case 49: o_locale = ACES; o_stack = SPADES; o_card = aces[SPADES].count - 1; break;
            case 65: o_locale = ACES; o_stack = HEARTS; o_card = aces[HEARTS].count - 1; break;
            case 81: o_locale = ACES; o_stack = CLUBS; o_card = aces[CLUBS].count - 1; break;
            case 97: o_locale = ACES; o_stack = DIAMONDS; o_card = aces[DIAMONDS].count - 1; break;

            default: return -1;
        }
    }
    else
    {   o_locale = TABLEAU;
        for(int i = 1, s = 0; i <= 97; i += 16, s++)
        {   if(oX == i)
            {   o_stack = s;
                break;
            }
        }
        o_card = (oY - 11) / 3;
    }

    if(dY == 1)
    {   switch(dX)
        {   case 1: return -1; //d_locale == PILE
            case 49: d_locale = ACES; d_stack = SPADES; d_card = aces[SPADES].count; break;
            case 65: d_locale = ACES; d_stack = HEARTS; d_card = aces[HEARTS].count; break;
            case 81: d_locale = ACES; d_stack = CLUBS; d_card = aces[CLUBS].count; break;
            case 97: d_locale = ACES; d_stack = DIAMONDS; d_card = aces[DIAMONDS].count; break;

            default: return -1;
        }
    }
    else
    {   d_locale = TABLEAU;
        for(int i = 1, s = 0; i <= 97; i += 16, s++)
        {   if(dX == i)
            {   d_stack = s;
                break;
            }
        }
        d_card = (dY - 11) / 3;
    }

    if(o_card > pta[o_locale][o_stack].count - 1) return -1; //origin card greater than the count
    if(d_card < pta[d_locale][d_stack].count - 1) d_card = pta[d_locale][d_stack].count - 1; //destination card lower than stack end, destination card becomes end card
    if(d_locale == ACES && (o_card != pta[o_locale][o_stack].count - 1)) return -1; //if destination is aces and origin not last card
    if(o_locale == d_locale == ACES) return -1;
    if(d_locale == TABLEAU)
    {   if(o_card < tableau[o_stack].visible) return -1;
    }
    
    if(d_locale == ACES)
    {   if(pta[o_locale][o_stack].stack[o_card].suitval != d_stack) return -1; //check matching suit
        if(pta[o_locale][o_stack].stack[o_card].cardval != aces[d_stack].count) return -1; //check if card enumerates

        int index = aces[d_stack].count;

        memcpy(&aces[d_stack].stack[index], &pta[o_locale][o_stack].stack[o_card], sizeof(struct card));
        memset(&pta[o_locale][o_stack].stack[o_card], 0, sizeof(struct card));

        if(aces[SPADES].stack[KING].cardval == KING && aces[HEARTS].stack[KING].cardval == KING && aces[CLUBS].stack[KING].cardval == KING && aces[DIAMONDS].stack[KING].cardval == KING)
        {   wincon = 1;
            return 0;
        }
        
        aces[d_stack].count++;
        pta[o_locale][o_stack].count--;
        if(pta[o_locale][o_stack].count == pta[o_locale][o_stack].visible && pta[o_locale][o_stack].visible > 0) pta[o_locale][o_stack].visible--; //visibilty check
        
        return 0;
    }

    //check if the suits are of opposite colours and the destination card # subtract the origin card # equals 1
    //or if the destination stack is empty and the origin card is a king
    if((((pta[o_locale][o_stack].stack[o_card].suitval + pta[d_locale][d_stack].stack[d_card].suitval) & 1) == 1 && (pta[d_locale][d_stack].stack[d_card].cardval - pta[o_locale][o_stack].stack[o_card].cardval) == 1) || (pta[d_locale][d_stack].count == 0 && (pta[o_locale][o_stack].stack[o_card].cardval == KING)))
    {   int limit = pta[o_locale][o_stack].count; //move card & children
        
        for(int i = o_card; i < limit; i++)
        {   memcpy(&pta[d_locale][d_stack].stack[pta[d_locale][d_stack].count], &pta[o_locale][o_stack].stack[i], sizeof(struct card)); //memcpy empty space to moved card
            memset(&pta[o_locale][o_stack].stack[o_card], 0, sizeof(struct card)); //zero moved card

            pta[d_locale][d_stack].count++;
            pta[o_locale][o_stack].count--;
        }
        if(pta[o_locale][o_stack].count == pta[o_locale][o_stack].visible && pta[o_locale][o_stack].visible > 0) pta[o_locale][o_stack].visible--; //visiblity check
        return 0;
    }
    return -1;
}

void layout()
{   printf("\033[H\033[2J");

    if(pile.count > 0) printf("+----------+\033[B\r| %c        |\033[B\r| %c        |\033[B\r|          |\033[B\r|          |\033[B\r|        %c |\033[B\r|        %c |\033[B\r+----------+\033[7A\t\t\t\t\t", cardchar[pile.stack[pile.count - 1].cardval], suitchar[pile.stack[pile.count - 1].suitval], suitchar[pile.stack[pile.count - 1].suitval], cardchar[pile.stack[pile.count - 1].cardval]);
    else
    {   for(int i = 0; i < 8; i++)
        {   printf("            ");
            if(i != 7) printf("\033[B\r");
        }
        printf("\033[7A\t\t\t\t\t");
    }
 
    for(int i = 0; i < 4; i++)
    {	if(aces[i].count > 0) printf("+----------+\033[B\033[12D| %c        |\033[B\033[12D| %c        |\033[B\033[12D|          |\033[B\033[12D|          |\033[B\033[12D|        %c |\033[B\033[12D|        %c |\033[B\033[12D+----------+\033[7A", cardchar[aces[i].stack[aces[i].count - 1].cardval], suitchar[i], suitchar[i], cardchar[aces[i].stack[aces[i].count - 1].cardval]);
        else printf("+----------+\033[B\033[12D| X        |\033[B\033[12D| %c        |\033[B\033[12D|          |\033[B\033[12D|          |\033[B\033[12D|        %c |\033[B\033[12D|        X |\033[B\033[12D+----------+\033[7A", suitchar[i], suitchar[i]);
	    putchar('\t');
    }
    printf("\033[10B\r");

    for(int s = 0; s < 7; s++)
    {   if(tableau[s].count < 1) printf("            \t");
        else
        {
            for(int c = 0; c < tableau[s].count; c++)
            {   if(c < tableau[s].visible)
                {   printf("+----------+\033[B\033[12D|[][][][][]|\033[B\033[12D|[][][][][]|\033[12D\033[B");
                }
                else if(c < tableau[s].count - 1)
                {   printf("+----------+\033[B\033[12D| %c        |\033[B\033[12D| %c        |\033[12D\033[B", cardchar[tableau[s].stack[c].cardval], suitchar[tableau[s].stack[c].suitval]);
                }
                else
                {   printf("+----------+\033[B\033[12D| %c        |\033[B\033[12D| %c        |\033[B\033[12D|          |\033[B\033[12D|          |\033[B\033[12D|        %c |\033[B\033[12D|        %c |\033[B\033[12D+----------+", cardchar[tableau[s].stack[c].cardval], suitchar[tableau[s].stack[c].suitval], suitchar[tableau[s].stack[c].suitval], cardchar[tableau[s].stack[c].cardval]);
                    printf("\033[%dA\t", 7 + (3 * (tableau[s].count - 1)));
                }
            }
        }
    }
}

void collect()
{   free(pile.stack);
    for(int i = 0; i < 7; i++) free(tableau[i].stack);
    for(int i = 0; i < 4; i++) free(aces[i].stack);
}