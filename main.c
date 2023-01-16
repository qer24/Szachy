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

//zwraca ruchy oraz przypisuje ilosc ruchow do moveCount
square** get_possible_moves(square* s, int* moveCount) {
    square** moves = malloc(sizeof(square*) * 8);
    int i = 0;
    int x = s->x;
    int y = s->y;
    int piece = s->piece;
    int color = piece > 0 ? 1 : -1;
    int type = abs(piece);
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
                if (chessboard[x][y + 2].piece == 0)
                {
                    moves[i] = &chessboard[x][y + 2];
                    i++;
                }
            }
            
            if (y == 6 && color == 1) 
            {
                if (chessboard[x][y - 2].piece == 0)
                {
                    moves[i] = &chessboard[x][y - 2];
                    i++;
                }
            }
            break;
    }
    
    *moveCount = i;
    return moves;
}

//zwraca ruchy atakujące (które będą od możliwych ruchów tylko dla pionka) oraz przypisuje ilosc ruchow do moveCount
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
            if (chessboard[x - 1][y - color].piece != 0 && chessboard[x - 1][y - color].piece * color < 0)
            {
                moves[i] = &chessboard[x - 1][y - color];
                i++;
            }
        }
        
        if (x + 1 < CHESSBOARD_SIZE && y - color >= 0 && y - color < CHESSBOARD_SIZE)
        {
            if (chessboard[x + 1][y - color].piece != 0 && chessboard[x + 1][y - color].piece * color < 0)
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
        return get_possible_moves(s, moveCount);
    }
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
                //turn *= -1;

                redraw_chessboard();
            }
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