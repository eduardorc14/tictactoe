#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <random>
#include <ctime>  // Necessário para usar a hora atual como semente

// Classe TicTacToe
class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
    std::mutex board_mutex; // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv; // Variável de condição para alternância de turnos
    char current_player; // Jogador atual ('X' ou 'O')
    bool game_over; // Estado do jogo
    char winner; // Vencedor do jogo

public:
    TicTacToe() : current_player('O'), game_over(false), winner(' ') {
        for (auto& row : board) {
            row.fill(' ');
        }
        // Inicializar o tabuleiro e as variáveis do jogo
    }
    void display_board() {
        // Exibir o tabuleiro no console
        std::lock_guard<std::mutex> lock(board_mutex);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                std::cout << (board[i][j] == ' ' ? '.' : board[i][j]);
                if (j < 2) std::cout << " | "; // Adiciona divisores entre colunas
            }
            std::cout << std::endl;
            if (i < 2) std::cout << "--+---+--" << std::endl; // Adiciona divisores entre linhas
        }
    std::cout << std::endl;
}

    

    bool make_move(char player, int row, int col) {
        // Implementar a lógica para realizar uma jogada no tabuleiro
        // Utilizar mutex para controle de acesso
        // Utilizar variável de condição para alternância de turnos
        std::unique_lock<std::mutex> lock(board_mutex);

        if (game_over) {
            return true;
        }

        while(player != current_player){
            turn_cv.wait(lock);
        }
        //verifica as dimensões do tabuleiro
        if(board[row][col] != ' '){
            std::cout << "Posiçao Invalida" << std::endl;
            return false;
        }
        //Realiza a jogada
        
        board[row][col] = player;
        std::cout << "Jogador " << player << " faz jogada em (" << row << ", " << col << ")" << std::endl;
        
        

        if (check_win(player)) {
            game_over = true;
            winner = player;
        } 
        else if (check_draw()) {
            game_over = true;
            winner = 'D';
        }
        //altera os jogadores
        current_player = (current_player == 'X') ? 'O' : 'X';
        
        
        //notifica a thread que está dormindo
        turn_cv.notify_one();
        return true;

    }

    bool check_win(char player) {
        // Verificar se o jogador atual venceu o jogo
        for (int i = 0; i < 3; ++i) {
            if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
                (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
                return true;
            }
        }
        return (board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
               (board[0][2] == player && board[1][1] == player && board[2][0] == player);
    }

    bool check_draw() {
        // Verificar se houve um empate
        for(int i = 0; i <= 3; i++){
            for(int j = 0; i <= 3; j++){
                if(board[i][j] == ' ')
                    return false;
            }
        }
        return true;
    }

    bool is_game_over() {
        // Retornar se o jogo terminou
        std::lock_guard<std::mutex> lock(board_mutex);
        return game_over;
    }

    char get_winner() {
        // Retornar o vencedor do jogo ('X', 'O', ou 'D' para empate)
        std::lock_guard<std::mutex> lock(board_mutex);
        return winner;
    }
};

// Classe Player
class Player {
private:
    TicTacToe& game; // Referência para o jogo
    char symbol; // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador
    std::default_random_engine generator;
public:
    Player(TicTacToe& g, char s, std::string strat) 
        : game(g), symbol(s), strategy(strat) {}

    void play() {
        // Executar jogadas de acordo com a estratégia escolhida
        while(!game.is_game_over()){
            if(strategy == "random"){
                play_random();
            }
            else{
                play_sequential();
            }
        }

    }

private:
    void play_sequential() {
        // Implementar a estratégia sequencial de jogadas
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                game.make_move(symbol, i, j);   
            }

        }
    }

    void play_random() {
        // Implementar a estratégia aleatória de jogadas
        std::uniform_int_distribution<int> distribution(0, 2);
        while (!game.make_move(symbol, distribution(generator), distribution(generator))) {}
    }
};

// Função principal
int main() {
    // Inicializar o jogo e os jogadores
    TicTacToe game;

    Player player1(game, 'X', "sequential");
    Player player2(game, 'O', "random");

    // Criar as threads para os jogadores
    std::thread t1(&Player::play, &player1);
    std::thread t2(&Player::play, &player2);

    // Aguardar o término das threads
    t1.join();
    t2.join();
    // Exibir o resultado final do jogo
    std::cout << "Resultado Final:" << std::endl;
    game.display_board();

    char winner = game.get_winner();
    if (winner == 'D') {
        std::cout << "O jogo terminou em empate!" << std::endl;
    } else {
        std::cout << "O jogador " << winner << " venceu o jogo!" << std::endl;
    }

    return 0;
    
}
