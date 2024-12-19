#include <bits/stdc++.h>
#include <ranges>
#include <csignal>
using namespace std;


template <int FieldN, int FieldM, typename PTYPE, typename VTYPE, typename VFLOWTYPE>
class Simulation{
public:
    static constexpr std::array<pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

    template <typename T>
    struct VectorField {
        array<T, deltas.size()> v[FieldN][FieldM];
        T &add(int x, int y, int dx, int dy, T dv) {
            return get(x, y, dx, dy) += dv;
        }

        T &get(int x, int y, int dx, int dy) {
//            int ansi = -1;
//            for (int i = 0; i < 4; i++) {
//                if (deltas[i] == make_pair(dx, dy)) {
//                    ansi = i;
//                    break;
//                }
//            }
//            size_t i = ansi;
            size_t i = ranges::find(deltas, pair(dx, dy)) - deltas.begin();
            assert(i < deltas.size());
            return v[x][y][i];
        }
    };

    struct ParticleParams {
        char type;
        PTYPE cur_p;
        array<VTYPE, deltas.size()> v;

        void swap_with(int x, int y, Simulation *sim) {
            swap(sim->field[x][y], type);
            swap(sim->p[x][y], cur_p);
            swap(sim->velocity.v[x][y], v);
        }
    };

    int tick_i = 0;
    char field[FieldN][FieldM + 1];
    VTYPE g;
    std::map<char, PTYPE> rho;
    PTYPE p[FieldN][FieldM]{}, old_p[FieldN][FieldM];
    VectorField<VTYPE> velocity{};
    VectorField<VFLOWTYPE> velocity_flow{};
    int last_use[FieldN][FieldM]{};
    int UT = 0;
    int dirs[FieldN][FieldM]{};
    mt19937 rnd;


    void save_to_file(std::ofstream &&out, bool is_for_next_run) {
        out << (double)this->g << '\n' << rho[' '] << '\n' << rho.size() << '\n';


        for (auto it : this->rho) {
            if (it.first != ' ') {
                out << it.first << " " << it.second << '\n';
            }
        }

        out << FieldN << " " << FieldM << '\n';
        out << '\n';

        for (int i = 0; i < FieldN; i++) {
            for (int j = 0; j < FieldM; j++) {
                out << field[i][j];
            }
            //out << '\n';
            out << '\n';
        }

        if (!is_for_next_run) {
            out << "NEW" << '\n';
        } else {
            out << "OLD" << '\n';
            for (int i = 0; i < FieldN; i++) {
                for (int j = 0; j < FieldM; j++) {
                    out << (double)p[i][j].v << " ";
                }
                out << '\n';
            }

            for (int i = 0; i < FieldN; i++) {
                for (int j = 0; j < FieldM; j++) {
                    out << (double)dirs[i][j] << " ";
                }
                out << '\n';
            }

            for (int i = 0; i < FieldN; i++) {
                for (int j = 0; j < FieldM; j++) {
                    out << last_use[i][j] << " ";
                }
                out << '\n';
            }

            for (int i = 0; i < FieldN; i++) {
                for (int j = 0; j < FieldM; j++) {
                    for (auto it : velocity.v[i][j]) {
                        out << (double)it << " ";
                    }
                    out << " ";
                }

                out << '\n';
            }
        }
    }


    void load_from_file(std::ifstream &&in) {
        int n, k, nf;
        double crho;
        char c;

        in >> crho;
        this->g = crho;
        in >> crho;
        this->rho[' '] = crho;
        in >> nf;

        for (int i = 0; i < nf; ++i) {
            in >> c >> crho;
            rho[c] = (PTYPE)crho;
        }

        in >> n >> k;

        in.get(c);
        in.get(c);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < k; ++j) {
                in.get(this->field[i][j]);
            }
            in.get(c);
            in.get(c);
        }

        std::string flag;
        in >> flag;

        if (flag == "NEW") {
            return;
        } else {
            double d;
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < k; ++j) {
                    in >> d;
                    this->p[i][j].v = d;
                }
            }

            for (int i = 0; i < FieldN; i++) {
                for (int j = 0; j < FieldM; j++) {
                    in >> d;
                    dirs[i][j] = d;
                }
            }

            for (int i = 0; i < FieldN; i++) {
                for (int j = 0; j < FieldM; j++) {
                    in >> d;
                    last_use[i][j] = d;
                }
            }

            for (int i = 0; i < FieldN; i++) {
                for (int j = 0; j < FieldM; j++) {
                    for (int u = 0; u < 4; u++) {
                        in >> d;
                        velocity.v[i][j][u] = d;
                    }
                }
            }
            return;
        }

    }

    void signal_handler(int signal_num){
        std::string path = "./files/" + std::to_string(this->tick_i);
        this->save_to_file(std::ofstream(path), true);
    }

    Simulation() {
        //signal(SIGQUIT, signal_handler);

        rnd = mt19937(1937);

        for (size_t x = 0; x < FieldN; ++x) {
            for (size_t y = 0; y < FieldM; ++y) {
                if (field[x][y] == '#') {
                    continue;
                }

                for (auto [dx, dy] : deltas) {
                    dirs[x][y] += (field[x + dx][y + dy] != '#');
                }
            }
        }
    }


    tuple<VTYPE, bool, pair<int, int>> propagate_flow(int x, int y, VTYPE lim) {
        last_use[x][y] = UT - 1;
        VTYPE ret = 0;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT) {
                auto cap = velocity.get(x, y, dx, dy);
                auto flow = velocity_flow.get(x, y, dx, dy);
                if (flow == cap) {
                    continue;
                }

                VFLOWTYPE vp = min(VFLOWTYPE(lim), cap - VFLOWTYPE(flow));

                if (last_use[nx][ny] == UT - 1) {
                    velocity_flow.add(x, y, dx, dy, vp);
                    last_use[x][y] = UT;
                    return {vp, 1, {nx, ny}};
                }
                auto [t, prop, end] = propagate_flow(nx, ny, vp);
                ret += t;
                if (prop) {
                    velocity_flow.add(x, y, dx, dy, VFLOWTYPE(t));
                    last_use[x][y] = UT;
                    return {t, prop && end != pair(x, y), end};
                }
            }
        }
        last_use[x][y] = UT;
        return {ret, 0, {0, 0}};
    }

    //dangerous for double, float
    VTYPE random01() {
        // unsigned int x = rnd();
        // int y = (1 << 16) - 1;
        // unsigned int res = x & y;
        double rand = ((double)rnd() / UINT_MAX);
        VTYPE ans = static_cast<VTYPE>(rand);
        //std::cout << "Random, LOL " <<  ans << std::endl;
        return ans;
    }

    // VFLOWTYPE random01() {
    //     return VFLOWTYPE::from_raw((rnd() & ((1 << 16) - 1)));
    // }

    // PTYPE random01() {
    //     return PTYPE::from_raw((rnd() & ((1 << 16) - 1)));
    // }

    void propagate_stop(int x, int y, bool force = false) {
        if (!force) {
            bool stop = true;
            for (auto [dx, dy] : deltas) {
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) > 0) {
                    stop = false;
                    break;
                }
            }
            if (!stop) {
                return;
            }
        }
        last_use[x][y] = UT;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT || velocity.get(x, y, dx, dy) > 0) {
                continue;
            }
            propagate_stop(nx, ny);
        }
    }

    VTYPE move_prob(int x, int y) {
        VTYPE sum = 0;
        for (size_t i = 0; i < deltas.size(); ++i) {
            //std::cout << "WHYYYYY" << i << std::endl;
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                continue;
            }
            auto v = velocity.get(x, y, dx, dy);
            //std::cout << "???????" << dx << " " << dy << " " << v << std::endl;
            if (v < 0) {
                continue;
            }
            sum += v;
        }
        return sum;
    }

    //why unreachable code
    bool propagate_move(int x, int y, bool is_first) {
        last_use[x][y] = UT - is_first;
        bool ret = false;
        int nx = -1, ny = -1;
        while (!ret) {
            std::array<VTYPE, deltas.size()> tres;
            VTYPE sum = 0;
            for (size_t i = 0; i < deltas.size(); ++i) {
                auto [dx, dy] = deltas[i];
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                    tres[i] = sum;
                    continue;
                }
                auto v = velocity.get(x, y, dx, dy);
                if (v < 0) {
                    tres[i] = sum;
                    continue;
                }
                sum += v;
                tres[i] = sum;
            }

            if (sum == 0) {
                break;
            }

            VTYPE p = random01() * sum;

            //std::ranges here
            size_t d = std::ranges::upper_bound(tres, p) - tres.begin();
//            size_t d = 0;
//
//            for(size_t i = 0; i < tres.size(); i++) {
//                if (tres[i] == p) {
//                    d = i;
//                    break;
//                }
//            }


            auto [dx, dy] = deltas[d];
            nx = x + dx;
            ny = y + dy;
            //std::cout << x << " " << y << " " << dx << " " << dy << " " <<  velocity.get(x, y, dx, dy) << std::endl;
            assert(velocity.get(x, y, dx, dy) > VTYPE(0) && field[nx][ny] != '#' && last_use[nx][ny] < UT);

            ret = (last_use[nx][ny] == UT - 1 || propagate_move(nx, ny, false));
        }
        last_use[x][y] = UT;
        for (size_t i = 0; i < deltas.size(); ++i) {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) < 0) {
                propagate_stop(nx, ny);
            }
        }
        if (ret) {
            if (!is_first) {
                ParticleParams pp{};
                pp.swap_with(x, y, this);
                pp.swap_with(nx, ny, this);
                pp.swap_with(x, y, this);
            }
        }
        return ret;
    }


    void Simulate() {


        PTYPE total_delta_p = 0;

        while (true) {
            //std::cout << "AAAAAAAAAAAAAAA" << std::endl;


            // Apply external forces
            for (size_t x = 0; x < FieldN; ++x) {
                for (size_t y = 0; y < FieldM; ++y) {
                    if (field[x][y] == '#') {
                        continue;
                    }
                    if (field[x + 1][y] != '#')
                        velocity.add(x, y, 1, 0, g);
                }
            }

            // std::cout << "=================V=====" << std::endl;
            // for (int i = 0; i < FieldN; i++) {
            //     for (int j = 0; j < FieldM; j++) {
            //         auto cur = velocity.get(i, j, 1, 0);
            //         std::cout << cur << " ";
            //     }
            //     std::cout << std::endl;
            // }
            // std::cout << "=================V=====" << std::endl;


            // Apply forces from p
            memcpy(old_p, p, sizeof(p));
            //old_p = p;
            for (size_t x = 0; x < FieldN; ++x) {
                for (size_t y = 0; y < FieldM; ++y) {
                    if (field[x][y] == '#') {
                        continue;
                    }

                    for (auto [dx, dy] : deltas) {
                        int nx = x + dx, ny = y + dy;

                        if (field[nx][ny] != '#' && old_p[nx][ny] < old_p[x][y]) {
                            auto delta_p = old_p[x][y] - old_p[nx][ny];
                            auto force = delta_p;
                            auto &contr = velocity.get(nx, ny, -dx, -dy);
                            if (contr * VTYPE(rho[(int) field[nx][ny]]) >= force) {
                                contr -= VTYPE(force / rho[(int) field[nx][ny]]);
                                continue;
                            }
                            //std::cout << "???????" << x << " " << y << " " << dx << " " << dy << " " << rho[(int) field[x][y]] << " " << (int) field[x][y] << std::endl;
                            force -= PTYPE(contr * VTYPE(rho[(int) field[nx][ny]]));
                            contr = 0;
                            velocity.add(x, y, dx, dy, VTYPE(force / rho[(int) field[x][y]]));

                            p[x][y] -= force / (double)dirs[x][y];
                            total_delta_p -= force / (double)dirs[x][y];
                        }
                    }
                }
            }

            // Make flow from velocities
            velocity_flow = {};
//            for (int u = 0; u < deltas.size(); u++) {
//                for (int i = 0; i < FieldN; i++) {
//                    for (int j = 0; j < FieldM; j++) {
//                        velocity_flow.v[u][i][j] = VFLOWTYPE(0);
//                    }
//                }
//            }


            bool prop = false;
            do {
                UT += 2;
                prop = 0;
                for (size_t x = 0; x < FieldN; ++x) {
                    for (size_t y = 0; y < FieldM; ++y) {
                        if (field[x][y] != '#' && last_use[x][y] != UT) {
                            auto [t, local_prop, _] = propagate_flow(x, y, 1);
                            if (t > 0) {
                                prop = 1;
                            }
                        }
                    }
                }
            } while (prop);

            // Recalculate p with kinetic energy

            //why unreachable
            for (size_t x = 0; x < FieldN; ++x) {
                for (size_t y = 0; y < FieldM; ++y) {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy] : deltas) {
                        auto old_v = velocity.get(x, y, dx, dy);
                        auto new_v = VTYPE(velocity_flow.get(x, y, dx, dy));

                        if (old_v > 0) {
                            assert(new_v <= old_v);
                            //std::cout << "This shit is useless " << new_v << std::endl;
                            velocity.get(x, y, dx, dy) = new_v;
                            PTYPE force = (old_v - new_v) * rho[(int) field[x][y]];
                            if (field[x][y] == '.')
                                force *= 0.8;
                            if (field[x + dx][y + dy] == '#') {
                                p[x][y] += force / (double)dirs[x][y];
                                total_delta_p += force / (double)dirs[x][y];
                            } else {
                                p[x + dx][y + dy] += force / (double)dirs[x + dx][y + dy];
                                total_delta_p += force / (double)dirs[x + dx][y + dy];
                            }
                        }
                    }
                }
            }

            UT += 2;
            prop = false;
            for (size_t x = 0; x < FieldN; ++x) {
                for (size_t y = 0; y < FieldM; ++y) {
                    if (field[x][y] != '#' && last_use[x][y] != UT) {
                       // std::cout << "!!!!!!! " << move_prob(x, y) << std::endl;
                        if (random01() < move_prob(x, y)) {
                            prop = true;
                            propagate_move(x, y, true);
                        } else {
                            propagate_stop(x, y, true);
                        }
                    }
                }
            }
            //std::cout << prop << std::endl;
            if (prop) {
                cout << "Tick " << tick_i << ":\n";
                for (size_t x = 0; x < FieldN; ++x) {
                    for (size_t y = 0; y < FieldM; ++y) {
                        std::cout << field[x][y];
                    }
                    std::cout << std::endl;
                }
            }
            if (tick_i % 1000 == 0) {
                this->signal_handler(0);
            }
            tick_i++;
        }
    }
};


