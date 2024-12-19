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

// // Define the template class
// template <typename... Types>
// class TypeBinder {
// public:
//     // Print the types
//     void printTypes() {
//         std::cout << "Types: ";
//         printTypeNames<Types...>();
//         std::cout << std::endl;
//     }

// private:
//     // Helper function to print the type names
//     template <typename T>
//     void printTypeNames() {
//         std::cout << typeid(T).name() << " ";
//     }

//     // Variadic template function to print the type names
//     template <typename T, typename... Rest>
//     void printTypeNames() {
//         printTypeNames<T>();
//         printTypeNames<Rest...>();
//     }

//     // Base case for the variadic template function
    
//     void printTypeNames(){}
// };


// Define the template class
template <typename T1, typename... Types>
class TypeBinder {
public:
    // Print the types
    // void printTypes() {
    //     std::cout << "Types: ";
    //     printTypeNames<T1, U1, Types...>();
    //     std::cout << std::endl;
    // }

    template <typename C>
    bool checkTypes() {
        return checkTypes<C, T1, Types...>();
    }

private:
    
    // template <typename C, typename T>
    // bool checkTypes() {
    //     if (typeid(C) == typeid(T)) {
    //         return true;
    //     } else {
    //         return false;
    //     }
    // }

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
    // // Helper function to print the type names
    // template <typename T>
    // void printTypeNames() {
    //     std::cout << typeid(T).name() << " ";
    // }

    // // Variadic template function to print the type names
    // template <typename T, typename U,typename... Rest>
    // void printTypeNames() {
    //     printTypeNames<T>();
    //     printTypeNames<U, Rest...>();
    // }

    // // Base case for the variadic template function

    // void printTypeNames(){}
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
    file_name = "input";
    std::vector<std::vector<char>> i_field;
    std::tuple<int, std::map<char, double>, int, int> file_info = read_file_new(std::ifstream(file_name), i_field);

    std::cout << std::get<0>(file_info) << std::endl;
    for (auto it : std::get<1>(file_info)) {
        std::cout << it.first << " " << it.second << std::endl;
    }

    std::cout << std::get<2>(file_info) << " " << std::get<3>(file_info) << std::endl;

    Simulation<36, 84, Fixed<64, 16>, Fixed<64, 16>, Fixed<64, 16>> sim;
//    char field[36][85] = {
//            "####################################################################################",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "#                                       .........                                  #",
//            "#..............#            #           .........                                  #",
//            "#..............#            #           .........                                  #",
//            "#..............#            #           .........                                  #",
//            "#..............#            #                                                      #",
//            "#..............#            #                                                      #",
//            "#..............#            #                                                      #",
//            "#..............#            #                                                      #",
//            "#..............#............#                                                      #",
//            "#..............#............#                                                      #",
//            "#..............#............#                                                      #",
//            "#..............#............#                                                      #",
//            "#..............#............#                                                      #",
//            "#..............#............#                                                      #",
//            "#..............#............#                                                      #",
//            "#..............#............#                                                      #",
//            "#..............#............################                     #                 #",
//            "#...........................#....................................#                 #",
//            "#...........................#....................................#                 #",
//            "#...........................#....................................#                 #",
//            "##################################################################                 #",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "#                                                                                  #",
//            "####################################################################################",
//    };
//
//    for (int i = 0; i < 36; i++) {
//        for (int j = 0; j < 85; j++) {
//            sim.field[i][j] = i_field[i][j];
//        }
//    }

//    for (int i = 0; i < 36; i++) {
//        for(int j = 0 ; j < 85; j++) {
//            sim.field[i][j] = field[i][j];
//        }
//    }


//
//    sim.g = std::get<0>(file_info);
//    //sim.rho['.'] = it.second;
//    for (auto it : std::get<1>(file_info)) {
//        std::cout << "GOT: " << (int)it.first << " " << it.second << std::endl;
//        sim.rho[it.first] = it.second;
//        std::cout << "RC: " << sim.rho[it.first] << std::endl;
//    }

    //  std::cout << "Good enough" << std::endl;


    // for (int i = 0; i < 100; ++i) {
    //     auto t = sim.random01();
    //     std::cout << t << std::endl;
    // }

    file_name = "./files/0";
    sim.load_from_file(std::ifstream(file_name));

    std::cout << "HERE" << std::endl;
    for (int i = 0; i < std::get<2>(file_info); ++i) {
        for (int j = 0; j < std::get<3>(file_info); ++j) {
            std::cout << sim.field[i][j];
        }
        std::cout << std::endl;
    }

    sim.Simulate();



    //Simulator sim(, v_type, v_flow_type, file_name);

    //std::cout << TYPES << " " << std::endl;
    //#define VTYPE v_type
    //TypeBinder<TYPES> tb;

    // for_each_f<ALLPAIRS>(10);

    // int n1, k1, t1;
    // if (v_type == "FLOAT") {
    //     if(tb.checkTypes<FLOAT>()) {
    //         n1 = 0;
    //         k1 = 0;
    //         t1 = 0;
    //     } else {
    //         std::cerr << "Error: Unsupported type for --v-type." << std::endl;
    //     }
    // } else if (v_type == "DOUBLE") {
    //     if(tb.checkTypes<DOUBLE>()) {
    //         n1 = 0;
    //         k1 = 0;
    //         t1 = 1;
    //     } else {
    //         std::cerr << "Error: Unsupported type for --v-type." << std::endl;
    //     }
    // } else if (v_type[0] == 'F' && v_type[1] == 'A') {
    //     int i = 2;
    //     while (v_type[i] != '(') {
    //         ++i;
    //     }
    //     i++;
    //     int j = i;
    //     while (v_type[j] != ',') {++j;}
    //     n1 = std::stoi(v_type.substr(i, j - i));
    //     j++;
    //     i = j;
    //     while (v_type[j] != ')') {++j;}
    //     k1 = std::stoi(v_type.substr(i, j - i));
    //     t1 = 2;

    //     // for(constexpr auto& n: ns) {
    //     //     if (n >= n1) {
    //     //         for (constexpr auto& k: ks) {
    //     //             if (k >= k1) {
    //     //                 using Type = FastFixed<n, k>;
    //     //                 tb.checkTypes<FAST_FIXED(n,k)>()
    //     //                 break;
    //     //             }
    //     //         }

    //     //         ans_j = i;
    //     //         break;
    //     //     }
    //     // }

        

        


    // }

    return 0;
}