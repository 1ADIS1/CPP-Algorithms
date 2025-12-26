#include "Hammurabi.h"

const int WHEAT_PER_HUMAN = 20;
const int TOTAL_ROUNDS = 10;
const int POPULATION_DIED_PERCENTAGE_GAME_OVER = 45;

static unsigned int saturating_sub(unsigned int a, unsigned int b) {
    if (a < b) {
        return 0;
    }
    else {
        return a - b;
    }
}

static int clamp(int value, int low, int high) {
    if (value < low) {
        return low;
    }
    else if (value > high) {
        return high;
    }
    else {
        return value;
    }
}

struct Town {
    int population = 0;

    // Accumulates during the game.
    int total_percentage_died = 0;

    int wheat = 0;

    // How many wheat will people eat
    int wheat_to_eat = 0;

    int tiles = 0;

    // Tiles that produce income.
    int sow_tiles = 0;

    int round = 0;

    void print_round_history(std::mt19937& gen) {
        std::uniform_int_distribution<int> tile_cost_dist(17, 26);
        int tile_cost = tile_cost_dist(gen);

        // Collects only from sow tiles. 
        // Wheat collected from 1 tile and total wheat collected 
        std::uniform_int_distribution<int> tile_wheat_collected_dist(1, 6);
        int tile_wheat_collected = tile_wheat_collected_dist(gen);
        int total_wheat_collected = tile_wheat_collected * sow_tiles;

        wheat += total_wheat_collected;

        std::uniform_int_distribution<int> wheat_rats_ate_dist(0, static_cast<int>(0.07 * wheat));
        int wheat_rats_ate = wheat_rats_ate_dist(gen);
        int wheat_after_rats_ate = wheat - wheat_rats_ate;

        wheat = wheat_after_rats_ate;

        // TODO: sanity check for subtraction
        unsigned int died = saturating_sub(population, static_cast<unsigned int>(floor(wheat_to_eat / WHEAT_PER_HUMAN)));
        if (population > 0) {
            total_percentage_died += died / population;
        }
        population -= died;

        is_game_over(died);

        int people_low = 0;
        int people_high = 50;
        int people_came_in = clamp(static_cast<int>(died / 2 + (5 - tile_wheat_collected) * wheat / 600 + 1), people_low, people_high);
        population += people_came_in;

        std::uniform_int_distribution<int> plague_chance_dist(0, 100);
        bool is_plague = plague_chance_dist(gen) <= 15;
        if (is_plague) {
            population = static_cast<int>(floor(population / 2));
        }

        std::cout << "Мой повелитель, соизволь поведать тебе" << std::endl << std::endl;

        std::cout << "в году " << round << " твоего высочайшего правления " << std::endl << std::endl;

        std::cout << died << " человек умерли с голоду, и " << people_came_in << " человек прибыли в наш великий город;" << std::endl << std::endl;

        if (is_plague) {
            std::cout << "Чума уничтожила половину населения; " << std::endl << std::endl;
        }

        std::cout << "Население города сейчас составляет " << population << " человек;" << std::endl << std::endl;

        std::cout << "Мы собрали " << total_wheat_collected << " бушелей пшеницы, по " << tile_wheat_collected << " бушеля с акра;" << std::endl << std::endl;

        std::cout << "Крысы истребили " << wheat_rats_ate << " бушелей пшеницы, оставив " << wheat_after_rats_ate << " бушеля в амбарах;" << std::endl << std::endl;

        std::cout << "Город сейчас занимает " << tiles << " акров;" << std::endl << std::endl;

        std::cout << "1 акр земли стоит сейчас " << tile_cost << " бушель." << std::endl << std::endl;
    }

    void print_game_statistics() {
        float average_died = total_percentage_died / static_cast<float>(TOTAL_ROUNDS);

        int tiles_per_human = tiles == 0 ? 0 : population / tiles;

        if (average_died > 0.33 && tiles_per_human < 7) {
            std::cout << "Из-за вашей некомпетентности в управлении, народ устроил бунт, и изгнал вас из города. Теперь вы вынуждены влачить жалкое существование в изгнании" << std::endl;
        }
        else if (average_died > 0.1 && tiles_per_human < 9) {
            std::cout << "Вы правили рукой, подобно Нерону и Ивану Грозному.Народ вздохнул с облегчением, и никто больше не желает видеть вас правителем" << std::endl;
        }
        else if (average_died > 0.03 && tiles_per_human < 10) {
            std::cout << "Вы справились вполне неплохо, у вас конечно, есть недоброжелатели, но многие хотели бы увидеть вас во главе города снова" << std::endl;
        }
        else {
            std::cout << "Фантастика! Карл Великий, Дизраэли и Джефферсон вместе не справились бы лучше" << std::endl;
        }
    }

    // TODO: sanity check for division and types
    bool is_game_over(unsigned int died) {
        if (population <= 0) return true;
        if (population > 0 && died / population > POPULATION_DIED_PERCENTAGE_GAME_OVER) return true;
        return round > TOTAL_ROUNDS;
    }

    void check_user_input(int tiles_to_buy, int tiles_to_sell, int user_wheat_to_eat, int tiles_to_sow) {
        sow_tiles = tiles_to_sow;
        wheat_to_eat = user_wheat_to_eat;

        wheat -= wheat_to_eat;
    }

    // TODO: check input
    void process_user_input(std::function<int(const std::string&)> read_int_fn) {
        int tiles_to_buy;
        int tiles_to_sell;
        int user_wheat_to_eat;
        int tiles_to_sow;

        std::cout << "Что пожелаешь, повелитель?" << std::endl << std::endl;

        tiles_to_buy = read_int_fn("Сколько акров земли повелеваешь купить? ");
        std::cout << std::endl << std::endl;

        tiles_to_sell = read_int_fn("Сколько акров земли повелеваешь продать? ");
        std::cout << std::endl << std::endl;

        user_wheat_to_eat = read_int_fn("Сколько бушелей пшеницы повелеваешь съесть? ");
        std::cout << std::endl << std::endl;

        tiles_to_sow = read_int_fn("Сколько акров земли повелеваешь засеять? ");
        std::cout << std::endl << std::endl;

        check_user_input(tiles_to_buy, tiles_to_sell, user_wheat_to_eat, tiles_to_sow);
    }
};

// Utility: read integer from stdin with validation (no characters allowed)
int read_int_validated(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            // EOF or error: exit gracefully
            std::cout << "Ввод завершён. Выход." << std::endl;
            std::exit(0);
        }

        // allow empty line? treat as retry
        if (line.size() == 0) {
            std::cout << "Ошибка: введите целое число." << std::endl;
            continue;
        }

        std::stringstream ss(line);
        long long value;
        char extra;
        if ((ss >> value) && !(ss >> extra)) {
            return static_cast<int>(value);
        }
        else {
            std::cout << "Ошибка: введите корректное целое число (без букв и лишних символов)." << std::endl;
        }
    }
}

// Utility: yes/no question, accepts y/n (and Russian д/н)
bool read_yes_no(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "Ввод завершён. Выход." << std::endl;
            std::exit(0);
        }
        if (line.empty()) continue;
        char c = line[0];
        if (c == 'y' || c == 'Y' || c == 'д' || c == 'Д') return true;
        if (c == 'n' || c == 'N' || c == 'н' || c == 'Н') return false;
        std::cout << "Пожалуйста, введите y/n (д/н)." << std::endl;
    }
}

// Save/load functions (text file, simple format)
bool save_game(const Town& town, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) return false;
    out << town.population << " "
        << town.total_percentage_died << " "
        << town.wheat << " "
        << town.wheat_to_eat << " "
        << town.tiles << " "
        << town.sow_tiles << " "
        << town.round << std::endl;
    return true;
}

bool load_game(Town& town, const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return false;
    in >> town.population
        >> town.total_percentage_died
        >> town.wheat
        >> town.wheat_to_eat
        >> town.tiles
        >> town.sow_tiles
        >> town.round;
    return true;
}

int main()
{
    SetConsoleOutputCP(65001);

    const std::string save_file = "savegame.txt";

    std::random_device rd;
    std::mt19937 gen(rd());

    Town town{};
    town.population = 100;
    town.wheat = 2800;
    town.tiles = 1000;
    town.round = 0;

    {
        std::ifstream in_check(save_file);
        if (in_check) {
            bool cont = read_yes_no("Найдена сохранённая игра. Продолжить предыдущую игру? (y/n) ");
            if (cont) {
                if (!load_game(town, save_file)) {
                    std::cout << "Не удалось загрузить сохранение. Начинаем новую игру." << std::endl;
                }
                else {
                    std::cout << "Сохранение загружено. Продолжаем игру." << std::endl;
                }
            }
            else {
                std::remove(save_file.c_str());
                std::cout << "Начинаем новую игру." << std::endl;
            }
        }
    }

    for (int i = 0; i < TOTAL_ROUNDS; i++) {
        std::cout << "\n=== Раунд " << (i + 1) << " ===\n";
        bool want_exit = read_yes_no("Прервать игру и сохранить прогресс? (y/n) ");
        if (want_exit) {
            if (save_game(town, save_file)) {
                std::cout << "Прогресс сохранён в файле: " << save_file << std::endl;
            }
            else {
                std::cout << "Ошибка сохранения игры." << std::endl;
            }
            return 0;
        }

        if (i != 0) {
            town.print_round_history(gen);

            town.round++;
        }

        town.process_user_input(read_int_validated);

        if (!save_game(town, save_file)) {
            std::cout << "Внимание: не удалось автоматически сохранить игру." << std::endl;
        }
    }

    town.print_game_statistics();
    std::remove(save_file.c_str());

    return 0;
}
