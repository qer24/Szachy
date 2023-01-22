#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <string.h>
#include <stdio.h>

#define CHESSBOARD_SIZE 8
#define CELL_SIZE 120

//definicja struktury przechowujacej dane o polach szachownicy
typedef struct square
{
    int x;
    int y;
    int piece; //0 = puste, +0 to białe, -0 to czarne, np. biały pionek +1, czarny pionek −1, biały skoczek +2, czarny skoczek −2, itd.
    GtkButton *button;
} square;

//globalna tablica przechowująca stan szachownicy
square chessboard[CHESSBOARD_SIZE][CHESSBOARD_SIZE];

int turn = 1; //1 = białe, −1 = czarne
int check = 0; //1 = szach biały, −1 = szach czarny, 0 = brak szacha
square *selected_square = NULL; //wskaźnik na aktualnie wybrane pole

//funkcja pomocnicza do rysowania szachownicy
char *piece_to_image_path(int piece)
{
    char *piece_icon;
    switch(piece)
    {
        case 1:
            piece_icon = "pieces/w_pawn.png";
            break;
        case -1:
            piece_icon = "pieces/b_pawn.png";
            break;
        case 2:
            piece_icon = "pieces/w_knight.png";
            break;
        case -2:
            piece_icon = "pieces/b_knight.png";
            break;
        case 3:
            piece_icon = "pieces/w_bishop.png";
            break;
        case -3:
            piece_icon = "pieces/b_bishop.png";
            break;
        case 4:
            piece_icon = "pieces/w_rook.png";
            break;
        case -4:
            piece_icon = "pieces/b_rook.png";
            break;
        case 5:
            piece_icon = "pieces/w_queen.png";
            break;
        case -5:
            piece_icon = "pieces/b_queen.png";
            break;
        case 6:
            piece_icon = "pieces/w_king.png";
            break;
        case -6:
            piece_icon = "pieces/b_king.png";
            break;
        default:
            piece_icon = " ";
            break;
    }
    return piece_icon;
}

void redraw_chessboard()
{
    int x, y;
    for (x = 0; x < CHESSBOARD_SIZE; x++)
    {
        for (y = 0; y < CHESSBOARD_SIZE; y++)
        {
            int piece = chessboard[x][y].piece;
            GtkButton *button = chessboard[x][y].button;
            if (piece != 0)
            {
                GtkWidget *image = gtk_image_new_from_file(piece_to_image_path(piece));
                gtk_button_set_child (GTK_BUTTON (button), image);
            }
            else
            {
                gtk_button_set_label(GTK_BUTTON(button), " ");
            }
        }
    }
}

void diagonal_moves(int x, int y, int color, square ***moves, int* i, int* canMoveBottomRight, int* canMoveTopRight, int* canMoveBottomLeft, int* canMoveTopLeft)
{
    for (int j = 1; j < CHESSBOARD_SIZE; j++)
    {
        //prawy dolny
        if (x + j < CHESSBOARD_SIZE && y + j < CHESSBOARD_SIZE)
        {
            if (*canMoveBottomRight)
            {
                if ((chessboard[x + j][y + j].piece == 0 || chessboard[x + j][y + j].piece * color < 0))
                {
                    if (chessboard[x + j][y + j].piece * color < 0)
                    {
                        *canMoveBottomRight = 0;
                    }

                    (*moves)[*i] = &chessboard[x + j][y + j];
                    (*i)++;
                }
                else
                {
                    *canMoveBottomRight = 0;
                }
            }
        }

        //prawy gorny
        if (x + j < CHESSBOARD_SIZE && y - j >= 0)
        {
            if (*canMoveTopRight)
            {
                if (chessboard[x + j][y - j].piece == 0 || chessboard[x + j][y - j].piece * color < 0)
                {
                    if (chessboard[x + j][y - j].piece * color < 0)
                    {
                        *canMoveTopRight = 0;
                    }

                    (*moves)[*i] = &chessboard[x + j][y - j];
                    (*i)++;
                }
                else
                {
                    *canMoveTopRight = 0;
                }
            }
        }

        //lewy dolny
        if (x - j >= 0 && y + j < CHESSBOARD_SIZE)
        {
            if (*canMoveBottomLeft)
            {
                if (chessboard[x - j][y + j].piece == 0 || chessboard[x - j][y + j].piece * color < 0)
                {
                    if (chessboard[x - j][y + j].piece * color < 0)
                    {
                        *canMoveBottomLeft = 0;
                    }

                    (*moves)[*i] = &chessboard[x - j][y + j];
                    (*i)++;
                }
                else
                {
                    *canMoveBottomLeft = 0;
                }
            }
        }

        //lewy gorny
        if (x - j >= 0 && y - j >= 0)
        {
            if (*canMoveTopLeft)
            {
                if (chessboard[x - j][y - j].piece == 0 || chessboard[x - j][y - j].piece * color < 0)
                {
                    if (chessboard[x - j][y - j].piece * color < 0)
                    {
                        *canMoveTopLeft = 0;
                    }

                    (*moves)[*i] = &chessboard[x - j][y - j];
                    (*i)++;
                }
                else
                {
                    *canMoveTopLeft = 0;
                }
            }
        }
    }
}

void horizontal_moves(int x, int y, int color, square ***moves, int* i, int* canMoveRight, int* canMoveLeft, int* canMoveBottom, int* canMoveTop)
{

    for (int j = 1; j < CHESSBOARD_SIZE; j++)
    {
        //prawo
        if (x + j < CHESSBOARD_SIZE)
        {
            if (*canMoveRight)
            {
                if (chessboard[x + j][y].piece == 0 || chessboard[x + j][y].piece * color < 0)
                {
                    if (chessboard[x + j][y].piece * color < 0)
                    {
                        *canMoveRight = 0;
                    }

                    (*moves)[*i] = &chessboard[x + j][y];
                    (*i)++;
                }
                else
                {
                    *canMoveRight = 0;
                }
            }
        }

        //lewo
        if (x - j >= 0)
        {
            if (*canMoveLeft)
            {
                if (chessboard[x - j][y].piece == 0 || chessboard[x - j][y].piece * color < 0)
                {
                    if (chessboard[x - j][y].piece * color < 0)
                    {
                        *canMoveLeft = 0;
                    }

                    (*moves)[*i] = &chessboard[x - j][y];
                    (*i)++;
                }
                else
                {
                    *canMoveLeft = 0;
                }
            }
        }

        //dol
        if (y + j < CHESSBOARD_SIZE)
        {
            if (*canMoveBottom)
            {
                if (chessboard[x][y + j].piece == 0 || chessboard[x][y + j].piece * color < 0)
                {
                    if (chessboard[x][y + j].piece * color < 0)
                    {
                        *canMoveBottom = 0;
                    }

                    (*moves)[*i] = &chessboard[x][y + j];
                    (*i)++;
                }
                else
                {
                    *canMoveBottom = 0;
                }
            }
        }

        //gora
        if (y - j >= 0)
        {
            if (*canMoveTop)
            {
                if (chessboard[x][y - j].piece == 0 || chessboard[x][y - j].piece * color < 0)
                {
                    if (chessboard[x][y - j].piece * color < 0)
                    {
                        *canMoveTop = 0;
                    }

                    (*moves)[*i] = &chessboard[x][y - j];
                    (*i)++;
                }
                else
                {
                    *canMoveTop = 0;
                }
            }
        }
    }
}

void knight_moves(int x, int y, int color, square ***moves, int* i)
{
    if (x + 2 < CHESSBOARD_SIZE && y + 1 < CHESSBOARD_SIZE)
    {
        if (chessboard[x + 2][y + 1].piece == 0 || chessboard[x + 2][y + 1].piece * color < 0)
        {
            (*moves)[*i] = &chessboard[x + 2][y + 1];
            (*i)++;
        }
    }
    if (x + 2 < CHESSBOARD_SIZE && y - 1 >= 0)
    {
        if (chessboard[x + 2][y - 1].piece == 0 || chessboard[x + 2][y - 1].piece * color < 0)
        {
            (*moves)[*i] = &chessboard[x + 2][y - 1];
            (*i)++;
        }
    }
    if (x - 2 >= 0 && y + 1 < CHESSBOARD_SIZE)
    {
        if (chessboard[x - 2][y + 1].piece == 0 || chessboard[x - 2][y + 1].piece * color < 0)
        {
            (*moves)[*i] = &chessboard[x - 2][y + 1];
            (*i)++;
        }
    }
    if (x - 2 >= 0 && y - 1 >= 0)
    {
        if (chessboard[x - 2][y - 1].piece == 0 || chessboard[x - 2][y - 1].piece * color < 0)
        {
            (*moves)[*i] = &chessboard[x - 2][y - 1];
            (*i)++;
        }
    }
    if (x + 1 < CHESSBOARD_SIZE && y + 2 < CHESSBOARD_SIZE)
    {
        if (chessboard[x + 1][y + 2].piece == 0 || chessboard[x + 1][y + 2].piece * color < 0)
        {
            (*moves)[*i] = &chessboard[x + 1][y + 2];
            (*i)++;
        }
    }
    if (x + 1 < CHESSBOARD_SIZE && y - 2 >= 0)
    {
        if (chessboard[x + 1][y - 2].piece == 0 || chessboard[x + 1][y - 2].piece * color < 0)
        {
            (*moves)[*i] = &chessboard[x + 1][y - 2];
            (*i)++;
        }
    }
    if (x - 1 >= 0 && y + 2 < CHESSBOARD_SIZE)
    {
        if (chessboard[x - 1][y + 2].piece == 0 || chessboard[x - 1][y + 2].piece * color < 0)
        {
            (*moves)[*i] = &chessboard[x - 1][y + 2];
            (*i)++;
        }
    }
    if (x - 1 >= 0 && y - 2 >= 0)
    {
        if (chessboard[x - 1][y - 2].piece == 0 || chessboard[x - 1][y - 2].piece * color < 0)
        {
            (*moves)[*i] = &chessboard[x - 1][y - 2];
            (*i)++;
        }
    }
}

square** simulate_board(square board[8][8], square *from, square *to)
{
    square **new_board = malloc(sizeof(square*) * CHESSBOARD_SIZE);
    for (int x = 0; x < CHESSBOARD_SIZE; x++)
    {
        new_board[x] = malloc(sizeof(square) * CHESSBOARD_SIZE);
        for (int y = 0; y < CHESSBOARD_SIZE; y++)
        {
            new_board[x][y].piece = board[x][y].piece;
        }
    }
    new_board[to->x][to->y].piece = new_board[from->x][from->y].piece;
    new_board[from->x][from->y].piece = 0;
    return new_board;
}

int is_board_in_check(square board[8][8], int color)
{
    square *king = NULL;
    for (int x = 0; x < CHESSBOARD_SIZE; x++)
    {
        for (int y = 0; y < CHESSBOARD_SIZE; y++)
        {
            if (board[x][y].piece == color * 6)
            {
                king = &board[x][y];
                break;
            }
        }
    }
    
    square **possible_moves = malloc(sizeof(square*) * 32);
    int moveCount = 0;
    
    int canCheckRight = 1, canCheckLeft = 1, canCheckUp = 1, canCheckDown = 1;
    horizontal_moves(king->x, king->y, color, &possible_moves, &moveCount, &canCheckRight, &canCheckLeft, &canCheckDown, &canCheckUp);
    
    for (int i = 0; i < moveCount; i++)
    {
        if (possible_moves[i]->piece * color < 0)
        {
            int type = abs(possible_moves[i]->piece);
            if (type == 4 || type == 5)
            {
                free(possible_moves);
                return color;
            }
        }
    }

    free(possible_moves);
    possible_moves = malloc(sizeof(square*) * 32);
    moveCount = 0;
    
    canCheckRight = 1, canCheckLeft = 1, canCheckUp = 1, canCheckDown = 1;
    diagonal_moves(king->x, king->y, color, &possible_moves, &moveCount, &canCheckRight, &canCheckLeft, &canCheckDown, &canCheckUp);
    
    for (int i = 0; i < moveCount; i++)
    {
        if (possible_moves[i]->piece * color < 0)
        {
            int type = abs(possible_moves[i]->piece);
            if (type == 1)
            {
                int x = possible_moves[i]->x, y = possible_moves[i]->y;
                if (x == king->x + 1 && y == king->y - color || x == king->x - 1 && y == king->y - color)
                {
                    free(possible_moves);
                    return color;
                }
            }
            if (type == 3 || type == 5)
            {
                free(possible_moves);
                return color;
            }
        }
    }

    free(possible_moves);
    possible_moves = malloc(sizeof(square*) * 8);
    moveCount = 0;
    
    knight_moves(king->x, king->y, color, &possible_moves, &moveCount);
    for (int i = 0; i < moveCount; ++i) 
    {
        if (possible_moves[i]->piece * color == -2) 
        {
            free(possible_moves);
            return color;
        }
    }

    free(possible_moves);
    return 0;
}

//zwraca ruchy oraz przypisuje ilosc ruchow do moveCount
square** get_possible_moves(square* s, int* moveCount) 
{
    square** moves = malloc(sizeof(square*) * 32);
    int i = 0;
    int x = s->x;
    int y = s->y;
    int piece = s->piece;
    int color = piece > 0 ? 1 : -1;
    int type = abs(piece);
    
    //dla ukosnych ruchow
    int canMoveTopRight = 1, canMoveTopLeft = 1, canMoveBottomRight = 1, canMoveBottomLeft = 1;
    //dla poziomych/pionowych ruchow
    int canMoveRight = 1, canMoveLeft = 1, canMoveBottom = 1, canMoveTop = 1;
    
    switch (type) {
        case 1: //pionek (może ruszać się tylko do przodu, i tylko gdy nie ma żadnego pionka na polu docelowym) (jako jedyny bije inaczej niż rusza)
            if (y - color >= 0 && y - color < CHESSBOARD_SIZE) 
            {
                if (chessboard[x][y - color].piece == 0) 
                {
                    moves[i] = &chessboard[x][y - color];
                    i++;
                }
            }
            if (y == 1 && color == -1)
            {
                if (chessboard[x][y + 2].piece == 0 && chessboard[x][y + 1].piece == 0)
                {
                    moves[i] = &chessboard[x][y + 2];
                    i++;
                }
            }
            if (y == 6 && color == 1) 
            {
                if (chessboard[x][y - 2].piece == 0 && chessboard[x][y - 1].piece == 0)
                {
                    moves[i] = &chessboard[x][y - 2];
                    i++;
                }
            }
            break;
        case 2: //skoczek
            knight_moves(x, y, color, &moves, &i);
            break;
        case 3: //goniec
            diagonal_moves(x, y, color, &moves, &i, &canMoveTopRight, &canMoveTopLeft, &canMoveBottomRight, &canMoveBottomLeft);
            break;
        case 4: //wieza
            horizontal_moves(x, y, color, &moves, &i, &canMoveRight, &canMoveLeft, &canMoveBottom, &canMoveTop);
            break;
        case 5: //krolowa
            diagonal_moves(x, y, color, &moves, &i, &canMoveTopRight, &canMoveTopLeft, &canMoveBottomRight, &canMoveBottomLeft);
            horizontal_moves(x, y, color, &moves, &i, &canMoveRight, &canMoveLeft, &canMoveBottom, &canMoveTop);
            break;
        case 6: //krol
            if (x + 1 < CHESSBOARD_SIZE)
            {
                if (chessboard[x + 1][y].piece == 0 || chessboard[x + 1][y].piece * color < 0)
                {
                    moves[i] = &chessboard[x + 1][y];
                    i++;
                }
            }
            if (x - 1 >= 0)
            {
                if (chessboard[x - 1][y].piece == 0 || chessboard[x - 1][y].piece * color < 0)
                {
                    moves[i] = &chessboard[x - 1][y];
                    i++;
                }
            }
            if (y + 1 < CHESSBOARD_SIZE)
            {
                if (chessboard[x][y + 1].piece == 0 || chessboard[x][y + 1].piece * color < 0)
                {
                    moves[i] = &chessboard[x][y + 1];
                    i++;
                }
            }
            if (y - 1 >= 0)
            {
                if (chessboard[x][y - 1].piece == 0 || chessboard[x][y - 1].piece * color < 0)
                {
                    moves[i] = &chessboard[x][y - 1];
                    i++;
                }
            }
            if (x + 1 < CHESSBOARD_SIZE && y + 1 < CHESSBOARD_SIZE)
            {
                if (chessboard[x + 1][y + 1].piece == 0 || chessboard[x + 1][y + 1].piece * color < 0)
                {
                    moves[i] = &chessboard[x + 1][y + 1];
                    i++;
                }
            }
            if (x + 1 < CHESSBOARD_SIZE && y - 1 >= 0)
            {
                if (chessboard[x + 1][y - 1].piece == 0 || chessboard[x + 1][y - 1].piece * color < 0)
                {
                    moves[i] = &chessboard[x + 1][y - 1];
                    i++;
                }
            }
            if (x - 1 >= 0 && y + 1 < CHESSBOARD_SIZE)
            {
                if (chessboard[x - 1][y + 1].piece == 0 || chessboard[x - 1][y + 1].piece * color < 0)
                {
                    moves[i] = &chessboard[x - 1][y + 1];
                    i++;
                }
            }
            if (x - 1 >= 0 && y - 1 >= 0)
            {
                if (chessboard[x - 1][y - 1].piece == 0 || chessboard[x - 1][y - 1].piece * color < 0)
                {
                    moves[i] = &chessboard[x - 1][y - 1];
                    i++;
                }
            }
            break;
    }
    
    //znajdz krola i usun go z listy krokow
    int foundKing = 0;
    for (int j = 0; j < i; j++)
    {
        if (!foundKing && abs(moves[j]->piece) == 6)
        {
            foundKing = 1;
            i--;
            continue;
        } 
        
        if (foundKing)
        {
            moves[j - 1] = moves[j];
        }
    }
    
    *moveCount = i;
    return moves;
}

//zwraca ruchy atakujące (które będą różne od możliwych ruchów tylko dla pionka) oraz przypisuje ilosc ruchow do moveCount
square** get_attack_moves(square* s, int* moveCount)
{
    int piece = s->piece;
    int type = abs(piece);
    if (type == 1)
    {
        square** moves = malloc(sizeof(square*) * 2);
        int i = 0;
        int x = s->x;
        int y = s->y;
        int color = piece > 0 ? 1 : -1;
        
        if (x - 1 >= 0 && y - color >= 0 && y - color < CHESSBOARD_SIZE)
        {
            if (chessboard[x - 1][y - color].piece != 0 && chessboard[x - 1][y - color].piece * color != -6 && chessboard[x - 1][y - color].piece * color < 0)
            {
                moves[i] = &chessboard[x - 1][y - color];
                i++;
            }
        }
        
        if (x + 1 < CHESSBOARD_SIZE && y - color >= 0 && y - color < CHESSBOARD_SIZE)
        {
            if (chessboard[x + 1][y - color].piece != 0 && chessboard[x + 1][y - color].piece * color != -6 && chessboard[x + 1][y - color].piece * color < 0)
            {
                moves[i] = &chessboard[x + 1][y - color];
                i++;
            }
        }
        
        *moveCount = i;
        return moves;
    }
    else
    {
        int overallMoveCount = 0;
        square** allMoves = get_possible_moves(s, &overallMoveCount);
        square** attackMoves = malloc(sizeof(square*) * overallMoveCount);
        int i = 0;
        int color = piece > 0 ? 1 : -1;

        for (int j = 0; j < overallMoveCount; j++)
        {
            if (abs(allMoves[j]->piece) != 6 && allMoves[j]->piece * color < 0)
            {
                attackMoves[i] = allMoves[j];
                i++;
            }
        }

        free(allMoves);
        
        *moveCount = i;
        return attackMoves;
    }
//    else
//    {
//        return get_possible_moves(s, moveCount);
//    }
}

int exists_in(square** moves, int moveCount, square* s)
{
    for (int i = 0; i < moveCount; i++)
    {
        if (moves[i] == s)
        {
            return 1;
        }
    }
    return 0;
}

void check_king_squares()
{
    for (int x = 0; x < CHESSBOARD_SIZE; ++x) 
    {
        for (int y = 0; y < CHESSBOARD_SIZE; ++y)
        {
            square* s = &chessboard[x][y];
            int type = abs(s->piece);
            if (type != 6) continue;
            
            int color = s->piece > 0 ? 1 : -1;
            GtkButton *button = s->button;
            GtkWidget *child = gtk_button_get_child(button);
            if (color * check > 0)
            {
                gtk_widget_set_name(child, "checked_king");
            } 
            else
            {
                gtk_widget_set_name(child, "normal");
            }
        }
    }
}

void unselect_square()
{
    if (selected_square == NULL)
        return;
    
    GtkWidget* button = GTK_WIDGET(selected_square->button);
    if ((selected_square->x + selected_square->y) % 2 == 0)
    {
        gtk_widget_set_name(button, "chessboard_white");
    }
    else
    {
        gtk_widget_set_name(button, "chessboard_black");
    }

    int moveCount, attackMoveCount;
    square** moves = get_possible_moves(selected_square, &moveCount);
    square** attackMoves = get_attack_moves(selected_square, &attackMoveCount);
    
    for (int i = 0; i < moveCount; ++i)
    {
        if (exists_in(attackMoves, attackMoveCount, moves[i]))
        {
            continue;
        }
        
        GtkButton* moveButton = moves[i]->button;

        GtkWidget* child = gtk_button_get_child(moveButton);
        if (child != NULL && strcmp(gtk_widget_get_name(child), "selection") == 0)
        {
            gtk_button_set_child(moveButton, NULL);
        }
    }
    
    for (int i = 0; i < attackMoveCount; ++i)
    {
        GtkButton* moveButton = attackMoves[i]->button;

        GtkWidget* child = gtk_button_get_child(moveButton);
        if (child != NULL && strcmp(gtk_widget_get_name(child), "attacked") == 0)
        {
            gtk_widget_set_name(child, "normal");
        }
    }
    
    free(moves);
    free(attackMoves);
    
    selected_square = NULL;
}

//funckja odpowiedzialna za logikę po kliknięciu myszką
void on_square_clicked (GtkWidget *widget, gpointer data)
{
    square *s = (square *)data;

    if (s->piece * turn > 0)
    {
        //jeśli jest już wybrane jakieś pole, to je odznaczamy
        unselect_square();

        selected_square = s;
        
        //zaznaczamy wybraną figure
        gtk_widget_set_name(GTK_WIDGET(s->button), "chessboard_selected");
        
        //zaznaczamy możliwe ruchy
        int moveCount, attackMoveCount;
        square** moves = get_possible_moves(s, &moveCount);
        square** attackMoves = get_attack_moves(s, &attackMoveCount);
        
        for (int i = 0; i < moveCount; ++i) 
        {
            if (exists_in(attackMoves, attackMoveCount, moves[i]))
            {
                continue;
            }
            
            square* moveSquare = moves[i];
            GtkButton* button = moveSquare->button;

            GtkWidget *image = gtk_image_new_from_file("images/selection_dot.png");
            gtk_widget_set_opacity(image, 0.3);
            gtk_widget_set_name(image, "selection");
            gtk_button_set_child (button, image);
        }

        for (int i = 0; i < attackMoveCount; ++i) 
        {
            square* attackSquare = attackMoves[i];
            
            if (attackSquare->piece == 0)
                continue;
            
            GtkButton* button = attackSquare->button;
            GtkWidget *image = gtk_button_get_child(button);
            gtk_widget_set_name(image, "attacked");
        }
        
        free(moves);
        free(attackMoves);
    } 
    else
    {
        //jeśli mamy wybraną figurę, to wykonujemy ruch
        if (selected_square != NULL)
        {
            square* old_square = selected_square;
            unselect_square();

            //szukamy możliwe ruchy
            int moveCount, attackMoveCount;
            square** moves = get_possible_moves(old_square, &moveCount);
            square** attackMoves = get_attack_moves(old_square, &attackMoveCount);
            
            if (exists_in(moves, moveCount, s) || exists_in(attackMoves, attackMoveCount, s))
            {
                //wykonujemy ruch
                s->piece = old_square->piece;
                old_square->piece = 0;
                
                //zmieniamy ture
                turn *= -1;

                check = is_board_in_check(chessboard, turn);
                printf("Check: %s\n", check == 1 ? "bialy" : check == -1 ? "czarny" : "brak");
                redraw_chessboard();
                check_king_squares();
            }
            
            free(moves);
            free(attackMoves);
        }
    }
}

//funkcja generująca szachownicę z pustymi polami
void generate_chessboard()
{
    int i, j;
    for(i = 0; i < CHESSBOARD_SIZE; i++)
    {
        for(j = 0; j < CHESSBOARD_SIZE; j++)
        {
            chessboard[i][j].x = i;
            chessboard[i][j].y = j;
            chessboard[i][j].piece = 0;
            chessboard[i][j].button = NULL;
        }
    }
}

//funckja odczytująca linie szachownicy z pliku
void load_chessboard_row_from_string(char* str, int y)
{
    //split str by |
    char* token = strtok(str, "|");
    int x = 0;
    while (token != NULL)
    {
        //zamien token na int
        int piece = atoi(token);
        chessboard[x][y].piece = piece;

        token = strtok( NULL, "|");
        x++;
    }
}

//funckja ładująca szachownice z pliku
void load_chessboard(char* filename)
{
    //open file called DefaultBoard.txt
    FILE *file;
    file = fopen(filename, "r");
    //read file
    int chessboardMaxSize = CHESSBOARD_SIZE * sizeof (square);
    char buffer[chessboardMaxSize];
    int lineCounter = 0;
    while (fgets(buffer, (int)sizeof(buffer), file) != NULL)
    {
        if (lineCounter > 0)
        {
            load_chessboard_row_from_string(buffer, lineCounter - 1);
        }
        lineCounter++;
    }

    //close file
    fclose(file);
}

//funckja rysująca szachownice
void show_chessboard(GtkWidget *grid)
{
    for (int x = 0; x < CHESSBOARD_SIZE; ++x) {
        for (int y = 0; y < CHESSBOARD_SIZE; ++y) {
            int piece = chessboard[x][y].piece;
            GtkWidget *button = gtk_button_new();
            if (piece != 0)
            {
                GtkWidget *image = gtk_image_new_from_file(piece_to_image_path(piece));
                gtk_button_set_child (GTK_BUTTON (button), image);
            }
            else
            {
                gtk_button_set_label(GTK_BUTTON(button), " ");
            }

            gtk_widget_set_size_request(button, CELL_SIZE, CELL_SIZE);
            if ((x + y) % 2 == 0) 
            {
                gtk_widget_set_name(button, "chessboard_white");
            } 
            else 
            {
                gtk_widget_set_name(button, "chessboard_black");
            }
            
            g_signal_connect (button, "clicked", G_CALLBACK(on_square_clicked), &chessboard[x][y]);
            gtk_grid_attach (GTK_GRID (grid), button, x, y, 1, 1);

            chessboard[x][y].button = GTK_BUTTON(button);
        }
    }
}

//funkcja inicializująca narzędzia do rysowania szachownicy (nie mój kod)
void on_activate (GtkApplication *app) 
{
    GtkWidget *window;
    GtkWidget *grid;

    /* create a new window, and set its title */
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Szachy");

    /* Here we construct the container that is going pack our buttons */
    grid = gtk_grid_new ();

    /* Pack the container in the window */
    gtk_window_set_child (GTK_WINDOW (window), grid);

    show_chessboard(grid);

    gtk_widget_show (window);
}

//funkcja dołączająca style.css do aplikacji (nie mój kod)
void myCSS(void)
{
    GtkCssProvider *provider;
    GdkDisplay *display;
    
    provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_path(provider, "style.css");
     
    display = gdk_display_get_default ();
    
    gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

//funkcja główna
int main (int argc, char *argv[]) 
{
    //Generate chessboard
    generate_chessboard();
    load_chessboard("DefaultBoard.txt");
    
    // Create a new application
    GtkApplication *app = gtk_application_new ("com.FilipB.Szachy", G_APPLICATION_DEFAULT_FLAGS);
    gtk_init();
    myCSS();
    g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    
    return status;
}