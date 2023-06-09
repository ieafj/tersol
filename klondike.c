#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#include "klondike.h"

struct stack tableau[7], aces[4], pile;
struct stack *area[3] = {&pile, &tableau[0], &aces[0]};
struct data carddata[2];

_Bool wincon, persist, selected;

int main(void)
{   printf("\033[H\033[2J"); //"cls"

    while(menu() == 0)
    {   if(initialise() != 0) return -1;

        layout();
        printf("\033[H");
        
        selected = 0;
        wincon = 0;
        persist = 1;

        int inp;
        int column = 1, row = 1;
        int src_column, src_row;

        clock_t start = clock();

        while(wincon == 0 && (inp = getch()) != '\b' && inp != 3)
        {   switch(inp)
            {   case '\t': cycle(); selected = 0; column = 1; row = 1; break;
 
                case '`': 
                    if(selected == 0)
                    {   column = 1;
                        row = 1;
                    }
                    else selected = 0; 
                    break;
                
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
                        {   int gs = (column - 1 >> 4) - 1; // - 1: stack to left
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
                    {   int gs = (column - 1 >> 4) + 1; // + 1: right stack
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
                {   src_column = column;
                    src_row = row;
                    extract(column, row, &carddata[selected]);
                    selected = 1;
                }
                else
                {   extract(column, row, &carddata[selected]);

                    if(move(src_column, src_row, &carddata[0], &carddata[1]) == 0)
                    {   if(pile.count == 0 && persist == 1)
                        {   for(int i = 0; i < 7; i++)
                            {   if(tableau[i].visible != 0) break;
                                if(i == 6)
                                {   printf("\033[H\033[2JAuto-complete? ");
                                    int response = getch();
                                    if(response == 3) exit(-1);
                                    else if(response != '\b') wincon = 1;
                                    else persist = 0;
                                }
                            }
                            if(wincon == 1) break;
                        }   
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
            printf("Elapsed time: %.1f minutes", ((double) clock() - start / CLOCKS_PER_SEC) / 60); //still dont know if this works
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
                printf("\r\033[KKlondike ruleset. WASD to move, Enter or Space to select. TAB shuffles free pile. Backspace to exit at any point. Keys 1-4 move to the respective ace pile. Backtick/grave to deselect, twice to move to the free pile.");
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

void extract(int column, int row, struct data *carddata)
{   carddata->stack = (column - 1) >> 4;

    if(row == 1)
    {   if(carddata->stack == 0)
        {   carddata->area = PILE;
            carddata->card = pile.count - 1;
        }
        else
        {   carddata->area = ACES;
            carddata->stack -= 3;
            carddata->card = aces[carddata->stack].count - (!selected);
        }
    }
    else
    {   carddata->area = TABLEAU;
        carddata->card = (row - 11) / 3;
    }
}

int move(int src_column, int src_row, struct data *src, struct data *dst)
{   if(dst->card < area[dst->area][dst->stack].count - 1) dst->card = area[dst->area][dst->stack].count - 1; //destination card lower than stack end, destination card becomes end card
    
    if(dst->area == PILE) return -1;
    
    else if(dst->area == ACES)
    {   if(src->area == ACES) return -1;
        if(src->card != area[src->area][src->stack].count - 1) return -1; //if moving from not last card in stack
        if(area[src->area][src->stack].stack[src->card].suitval != dst->stack) return -1; //check matching suit
        if(area[src->area][src->stack].stack[src->card].cardval != aces[dst->stack].count) return -1; //check if card enumerates

        int index = aces[dst->stack].count;

        memcpy(&aces[dst->stack].stack[index], &area[src->area][src->stack].stack[src->card], sizeof(struct card));
        memset(&area[src->area][src->stack].stack[src->card], 0, sizeof(struct card));

        if(src->card == KING)
        {   for(int i = 0; i < 4; i++)
            {   if(!aces[i].stack[KING].cardval) break;
                if(i == 3)
                {   wincon = 1;
                    return 0;
                }
            }
        }
        aces[dst->stack].count++;
        area[src->area][src->stack].count--;
        if(area[src->area][src->stack].count == area[src->area][src->stack].visible && area[src->area][src->stack].visible > 0) area[src->area][src->stack].visible--; //visibilty check
        
        printf(CARD_FULL, cardchar[aces[dst->stack].stack[dst->card].cardval], suitchar[dst->stack], suitchar[dst->stack], cardchar[aces[dst->stack].stack[dst->card].cardval]);
        printf("\033[%d;%dH", src_row, src_column);
        if(src->area == PILE) printf(CARD_FULL, cardchar[pile.stack[pile.count - 1].cardval], suitchar[pile.stack[pile.count - 1].suitval], suitchar[pile.stack[pile.count - 1].suitval], cardchar[pile.stack[pile.count - 1].cardval]);
        else
        {   printf(CARD_EMPTY);
            if(tableau[src->stack].count > 0)
            {   printf("\033[11A");
                printf(CARD_FULL, cardchar[tableau[src->stack].stack[src->card - 1].cardval], suitchar[tableau[src->stack].stack[src->card - 1].suitval], suitchar[tableau[src->stack].stack[src->card - 1].suitval], cardchar[tableau[src->stack].stack[src->card - 1].cardval]);
            }
        }
        return 0;
    }
    else
    {   if((((area[src->area][src->stack].stack[src->card].suitval + area[dst->area][dst->stack].stack[dst->card].suitval) & 1) == 1 && (area[dst->area][dst->stack].stack[dst->card].cardval - area[src->area][src->stack].stack[src->card].cardval) == 1) || (area[dst->area][dst->stack].count == 0 && (area[src->area][src->stack].stack[src->card].cardval == KING)))
        {   int limit = area[src->area][src->stack].count; //move card & children
            
            for(int i = src->card; i < limit; i++)
            {   memcpy(&area[dst->area][dst->stack].stack[area[dst->area][dst->stack].count], &area[src->area][src->stack].stack[i], sizeof(struct card)); //memcpy next card with destination card
                
                if(tableau[dst->stack].count > 0) printf("\033[3B");
                printf(CARD_FULL, cardchar[area[src->area][src->stack].stack[i].cardval], suitchar[area[src->area][src->stack].stack[i].suitval], suitchar[area[src->area][src->stack].stack[i].suitval], cardchar[area[src->area][src->stack].stack[i].cardval]);
                printf("\033[7A\033[12D");
                
                memset(&area[src->area][src->stack].stack[src->card], 0, sizeof(struct card)); //zero source card

                area[dst->area][dst->stack].count++;
                area[src->area][src->stack].count--;
            }
            if(area[src->area][src->stack].count == area[src->area][src->stack].visible && area[src->area][src->stack].visible > 0) area[src->area][src->stack].visible--; //visiblity check
                        
            printf("\033[%d;%dH", src_row, src_column);
            if(src->area == PILE)
            {   if(pile.count > 0) printf(CARD_FULL, cardchar[pile.stack[pile.count - 1].cardval], suitchar[pile.stack[pile.count - 1].suitval], suitchar[pile.stack[pile.count - 1].suitval], cardchar[pile.stack[pile.count - 1].cardval]);
                else printf(CARD_EMPTY);
            }
            else if(src->area == ACES)
            {   if(aces[src->stack].count > 0) printf(CARD_FULL, cardchar[aces[src->stack].stack[aces[src->stack].count - 1].cardval], suitchar[src->stack], suitchar[src->stack], cardchar[aces[src->stack].stack[aces[src->stack].count - 1].cardval]); 
                else printf(CARD_FULL, 'X', suitchar[src->stack], suitchar[src->stack], 'X');
            }
            else
            {   if(tableau[src->stack].count > 0)
                {   printf("\033[3A");
                    printf(CARD_FULL, cardchar[tableau[src->stack].stack[tableau[src->stack].count - 1].cardval], suitchar[tableau[src->stack].stack[tableau[src->stack].count - 1].suitval], suitchar[tableau[src->stack].stack[tableau[src->stack].count - 1].suitval], cardchar[tableau[src->stack].stack[tableau[src->stack].count - 1].cardval]);
                    printf("\033[12D\033[B");
                    for(int i = src->card; i < limit; i++) printf(CARD_EMPTY);
                }
                else printf(CARD_EMPTY);
            }
            return 0;
        }
        else return -1; //not a standard legal move or king to free space
    }
}

void layout()
{   printf(CARD_FULL, cardchar[pile.stack[pile.count - 1].cardval], suitchar[pile.stack[pile.count - 1].suitval], suitchar[pile.stack[pile.count - 1].suitval], cardchar[pile.stack[pile.count - 1].cardval]);
    printf("\033[7A\t\t\t\t\t");
 
    for(int i = 0; i < 4; i++)
    {	printf(CARD_FULL, 'X', suitchar[i], suitchar[i], 'X');
	    printf("\033[7A\t");
    }
    printf("\033[10B\r");

    for(int s = 0; s < 7; s++)
    {   for(int c = 0; c < tableau[s].count; c++)
        {   if(c < tableau[s].visible) printf(CARD_BACK);
            else
            {   printf(CARD_FULL, cardchar[tableau[s].stack[c].cardval], suitchar[tableau[s].stack[c].suitval], suitchar[tableau[s].stack[c].suitval], cardchar[tableau[s].stack[c].cardval]);
                printf("\033[%dA\t", 7 + (3 * (tableau[s].count - 1)));
            }
        }
    }
}

void collect()
{   free(pile.stack);
    for(int i = 0; i < 7; i++) free(tableau[i].stack);
    for(int i = 0; i < 4; i++) free(aces[i].stack);
}