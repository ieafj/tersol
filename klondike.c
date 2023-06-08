#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#include "klondike.h"

_Bool wincon;

struct stack tableau[7], aces[4], pile;

struct stack *area[3] = {&pile, &tableau[0], &aces[0]};

int main(void)
{   printf("\033[H\033[2J"); //"cls"

    while(menu() == 0)
    {   if(initialise() != 0) return -1;

        layout();
        printf("\033[H");

        int inp;
        int column = 1, row = 1;
        int originX, originY;

        _Bool selected = 0, persist = 1;
        
        wincon = 0;

        clock_t start = clock() / 60;

        while((inp = getch()) != '\b' && inp != 3 && wincon == 0)
        {   switch(inp)
            {   case '\t': cycle(); selected = 0; column = 1; row = 1; break;
 
                case '`': selected = 0; column = 1; row = 1; break;
                
                case '1': row = 1; column = 49; break;
                case '2': row = 1; column = 65; break;
                case '3': row = 1; column = 81; break;
                case '4': row = 1; column = 97; break;

                case 'A':
                case 'a': 
                    if(row < 10)
                    {   if(column == 49) column -= 48; 
                        else if(column > 49) column -= 16;
                    }
                    else
                    {   if(column > 1)
                        {   int gs = (column - 1 >> 4) - 1;
                            column -= 16; 

                            if(selected == 0) row = 11 + (tableau[gs].visible * 3);
                            else if(tableau[gs].count == 0) row = 11;
                            else row = 11 + ((tableau[gs].count - 1) * 3);
                        }
                    }
                    break;

                case 'D':
                case 'd':
                    if(row < 11)
                    {   if(column < 49) column += 48;
                        else if(column < 97) column += 16;
                    } 
                    else if(column < 97)
                    {   int gs = (column - 1 >> 4) + 1;
                        column += 16; 

                        if(selected == 0) row = 11 + (tableau[gs].visible * 3);
                        else if(tableau[gs].count == 0) row = 11;
                        else row = 11 + ((tableau[gs].count - 1) * 3);
                    }
                    break;

                case 'W':
                case 'w': 
                    if(row == 11)
                    {   if(column == 17) column = 1, row = 1;
                        else if(column == 33) column = 49, row = 1;
                        else row -= 10;
                    }
                    else if(row > 11)
                    {   if(row == 11 + (tableau[column - 1 >> 4].visible * 3))
                        {   if(column == 17) column = 1, row = 1;
                            else if(column == 33) column = 49, row = 1;
                            else row -= 10 + (tableau[column - 1 >> 4].visible * 3);
                        }
                        else row -= 3;
                    }
                    break;

                case 'S':
                case 's': 
                    if(row == 1)
                    {   if(selected == 1 || pile.count == 0)
                        {   row += 10 + ((tableau[column - 1 >> 4].count - 1) * 3);
                            if(row < 11) row = 11;
                        }
                        else row += 10 + (tableau[column - 1 >> 4].visible * 3);
                    }
                    else if(row < 11 + ((tableau[column - 1 >> 4].count - 1) * 3)) row += 3;
                    
                    break;

                case ' ':
                case '\n':
                case '\r':
                if(selected == 0)
                {   originX = column;
                    originY = row;
                    selected = 1;
                }
                else
                {   if(move(originX, originY, column, row) == 0)
                    {   if(pile.count == 0 && persist == 1)
                        {   for(int i = 0; i < 7; i++)
                            {   if(tableau[i].visible != 0) break;
                                if(i == 6)
                                {   printf("\033[H\033[2JAuto-complete? "); //requires two keypresses?
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
            if(wincon != 1) printf("\033[%d;%dH", row, column);
        }
        printf("\033[H\033[2J");

        collect();

        if(wincon == 1)
        {   printf("Game complete.\n");
            printf("Elapsed time: %.1f minutes", ((double) clock() - start / CLOCKS_PER_SEC) / 600); //still dont know if this works
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
{   struct card *deck = malloc(52 * sizeof(struct card));
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
    memcpy(pile.stack, deck + 28, (24 * sizeof(struct card)));
    pile.count = 24;

    for(int i = 0, nthtri = 0; i < 7; i++) //define tableau
    {   tableau[i].stack = calloc((13 + i), sizeof(struct card)); //account for unturned cards
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

    for(int i = 0; i < 4; i++) //define ace card piles
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

int move(int src_col, int src_row, int dst_col, int dst_row)
{   
    /* as function
    stack = (col - 1) >> 4;

    if(row == 1)
    {   if(stack == 0)
        {   area = PILE;
            card = pile.count - 1;
        }
        else
        {   area = ACES;
            stack -= 2;
            card = aces[stack].count; // +- 1 depending on in/outbound
        }
    }
    else
    {   area = TABLEAU;
        card = (row - 11) / 3;
    }
    */
   
    int source_area, source_stack, source_card, dst_area, dst_stack, dst_card; 
    
    if(src_row == 1)
    {   switch(src_col)
        {   case 1: source_area = PILE; source_stack = 0; source_card = pile.count - 1; break;
            case 49: source_area = ACES; source_stack = SPADES; source_card = aces[SPADES].count - 1; break;
            case 65: source_area = ACES; source_stack = HEARTS; source_card = aces[HEARTS].count - 1; break;
            case 81: source_area = ACES; source_stack = CLUBS; source_card = aces[CLUBS].count - 1; break;
            case 97: source_area = ACES; source_stack = DIAMONDS; source_card = aces[DIAMONDS].count - 1; break;

            default: return -1;
        }
    }
    else
    {   source_area = TABLEAU;
        for(int i = 1, s = 0; i <= 97; i += 16, s++)
        {   if(src_col == i)
            {   source_stack = s;
                break;
            }
        }
        source_card = (src_row - 11) / 3;
    }

    if(dst_row == 1)
    {   switch(dst_col)
        {   case 1: return -1; //dst_area == PILE
            case 49: dst_area = ACES; dst_stack = SPADES; dst_card = aces[SPADES].count; break;
            case 65: dst_area = ACES; dst_stack = HEARTS; dst_card = aces[HEARTS].count; break;
            case 81: dst_area = ACES; dst_stack = CLUBS; dst_card = aces[CLUBS].count; break;
            case 97: dst_area = ACES; dst_stack = DIAMONDS; dst_card = aces[DIAMONDS].count; break;

            default: return -1;
        }
    }
    else
    {   dst_area = TABLEAU;
        for(int i = 1, s = 0; i <= 97; i += 16, s++)
        {   if(dst_col == i)
            {   dst_stack = s;
                break;
            }
        }
        dst_card = (dst_row - 11) / 3;
    }

    if(source_card > area[source_area][source_stack].count - 1) return -1; //origin card greater than the count
    if(dst_card < area[dst_area][dst_stack].count - 1) dst_card = area[dst_area][dst_stack].count - 1; //destination card lower than stack end, destination card becomes end card
    if(dst_area == ACES && (source_card != area[source_area][source_stack].count - 1)) return -1; //if destination is aces and origin not last card
    if(source_area == dst_area == ACES) return -1;
    if(dst_area == TABLEAU)
    {   if(source_card < tableau[source_stack].visible) return -1;
    }
    
    if(dst_area == ACES)
    {   if(area[source_area][source_stack].stack[source_card].suitval != dst_stack) return -1; //check matching suit
        if(area[source_area][source_stack].stack[source_card].cardval != aces[dst_stack].count) return -1; //check if card enumerates

        int index = aces[dst_stack].count;

        memcpy(&aces[dst_stack].stack[index], &area[source_area][source_stack].stack[source_card], sizeof(struct card));
        memset(&area[source_area][source_stack].stack[source_card], 0, sizeof(struct card));

        if(aces[SPADES].stack[KING].cardval == KING && aces[HEARTS].stack[KING].cardval == KING && aces[CLUBS].stack[KING].cardval == KING && aces[DIAMONDS].stack[KING].cardval == KING)
        {   wincon = 1;
            return 0;
        }
        
        aces[dst_stack].count++;
        area[source_area][source_stack].count--;
        if(area[source_area][source_stack].count == area[source_area][source_stack].visible && area[source_area][source_stack].visible > 0) area[source_area][source_stack].visible--; //visibilty check
        
        return 0;
    }

    //check if the suits are of opposite colours and the destination card # subtract the origin card # equals 1
    //or if the destination stack is empty and the origin card is a king
    if((((area[source_area][source_stack].stack[source_card].suitval + area[dst_area][dst_stack].stack[dst_card].suitval) & 1) == 1 && (area[dst_area][dst_stack].stack[dst_card].cardval - area[source_area][source_stack].stack[source_card].cardval) == 1) || (area[dst_area][dst_stack].count == 0 && (area[source_area][source_stack].stack[source_card].cardval == KING)))
    {   int limit = area[source_area][source_stack].count; //move card & children
        
        for(int i = source_card; i < limit; i++)
        {   memcpy(&area[dst_area][dst_stack].stack[area[dst_area][dst_stack].count], &area[source_area][source_stack].stack[i], sizeof(struct card)); //memcpy empty space to moved card
            memset(&area[source_area][source_stack].stack[source_card], 0, sizeof(struct card)); //zero moved card

            area[dst_area][dst_stack].count++;
            area[source_area][source_stack].count--;
        }
        if(area[source_area][source_stack].count == area[source_area][source_stack].visible && area[source_area][source_stack].visible > 0) area[source_area][source_stack].visible--; //visiblity check
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