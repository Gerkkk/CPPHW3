#include <iostream>
#include "FastFixed.h"
#include "Fixed.h"
#include <vector>
#include <string>
#include <typeinfo>
#include "macros.h"
#include <fstream>
#include <bits/stdc++.h>
#include "Simulator.h"


namespace TypeFromString {
    template <int N, int K, int T>
    struct Store {
        using Type = void;
    };

    template <int N, int K>
    struct Store<N, K, 2> {
        using Type = Fixed<N, K>;
    };

    template <int N, int K>
    struct Store<N, K, 3> {
        using Type = FastFixed<N, K>;
    };

    template <int N, int K>
    struct Store<N, K, 0> {
        using Type = float;
    };

    template <int N, int K>
    struct Store<N, K, 1> {
        using Type = double;
    };
}

std::tuple<int, std::map<char, double>, int, int> read_file_new(std::ifstream &&in, std::vector<std::vector<char>> &i_field) {
    int g, n, k, nf;
    double crho;
    double air_rho;
    char c;
    std::map<char, double> fluid_rho;

    in >> g >> air_rho >> nf;
    fluid_rho[' '] = air_rho;
    std::cout << air_rho << " " << fluid_rho[' '] << (int)' ' << std::endl;
    for (int i = 0; i < nf; ++i) {
        in >> c >> crho;
        fluid_rho[c] = crho;
    }

    in >> n >> k;

   
    i_field.resize(n);
    in.get(c);
    in.get(c);
    for (int i = 0; i < n; ++i) {
        i_field[i].resize(k);
        for (int j = 0; j < k; ++j) {
            in.get(i_field[i][j]);
        }
        in.get(c);
        in.get(c);
    }

    return {g, fluid_rho, n, k};
}


// Define the template class
template <typename T1, typename... Types>
class TypeBinder {
public:

    template <typename C>
    bool checkTypes() {
        return checkTypes<C, T1, Types...>();
    }

private:
    template <typename C, typename T, typename... Rest>
    bool checkTypes() {
        if (sizeof...(Rest) == 0) {
            if (std::is_same<C,T>::value) {
                return true;
            } else {
                return false;
            }
        } else {
            return checkTypes<C, T>() || checkTypes<C, Rest...>();
        }   
    }

    //template <typename C>
    bool checkTypes(){
        return false;
    }
};

int main(int argc, char* argv[]) {
    std::string p_type, v_type, v_flow_type, file_name;
    std::vector<std::string> arguments;

    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg.find("--") == 0) {
            int u = arg.find('=');

            arguments.push_back(arg.substr(0, u));
            arguments.push_back(arg.substr(u + 1));
        } else {
            arguments.push_back(arg);
        }
    }

    for (int i = 1; i < arguments.size(); ++i) {
        if (std::string(arguments[i]) == "--p-type") {
            if (i + 1 < arguments.size()) {
                p_type = arguments[i + 1];
                ++i;
            } else {
                std::cerr << "Error: --p-type requires a value." << std::endl;
                return 1;
            }
        } else if (std::string(arguments[i]) == "--v-type") {
            if (i + 1 < arguments.size()) {
                v_type = arguments[i + 1];
                ++i;
            } else {
                std::cerr << "Error: --v-type requires a value." << std::endl;
                return 1;
            }
        } else if (std::string(arguments[i]) == "--v-flow-type") {
            if (i + 1 < arguments.size()) {
                v_flow_type = arguments[i + 1];
                ++i;
            } else {
                std::cerr << "Error: --v-flow-type requires a value." << std::endl;
                return 1;
            }
        } else if (std::string(arguments[i]) == "--file-name") {
            if (i + 1 < arguments.size()) {
                file_name = arguments[i + 1];
                ++i;
            } else {
                std::cerr << "Error: --file-name requires a value." << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Warning: Unknown argument: " << arguments[i] << std::endl;
        }
    }

    std::cout << p_type << " " << v_type << " " << v_flow_type << " " << file_name << std::endl;

    Simulation<36, 84, FIXED(64, 10), FIXED(64, 10), FIXED(64, 10)> sim;
    file_name = "input.txt";
    sim.load_from_file(std::ifstream(file_name));
    sim.tm.active = true;

    sim.Simulate();

    sim.tm.finish();
    sim.tm.print_results();

    return 0;
}