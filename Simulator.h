#include <bits/stdc++.h>
#include <ranges>
#include <csignal>
#include <chrono>
#include <queue>
#include <vector>
#include <set>
using namespace std;


template <int FieldN, int FieldM, typename PTYPE, typename VTYPE, typename VFLOWTYPE>
class Simulation{
public:
    struct TimeMeasurer {
        bool active = false;
        int max_tick;

        clock_t start_time = 0;
        clock_t finish_time = 0;

        clock_t propagate_flow_number = 0;
        clock_t sum_pf_time = 0;

        clock_t propagate_stop_number = 0;
        clock_t sum_ps_time = 0;

        clock_t move_prob_number = 0;
        clock_t sum_mp_time = 0;

        clock_t propagate_move_number = 0;
        clock_t sum_pm_time = 0;


        clock_t sum_cycle1 = 0;
        clock_t sum_cycle2 = 0;
        clock_t sum_cycle3 = 0;
        clock_t sum_cycle4 = 0;

        TimeMeasurer () {
            max_tick = 1000;
        }

        void start() {
            start_time = clock();
        }

        void finish() {
            finish_time = clock();
        }

        void print_results() {
            cout << "Simulation time: " << ((double)finish_time - start_time) / CLOCKS_PER_SEC << " seconds\n";
            cout << "Average tick time: " << ((double)finish_time - start_time) / CLOCKS_PER_SEC / max_tick << " seconds\n\n";
            
            cout << "Cycle1 time per tick: " << (double)sum_cycle1 / CLOCKS_PER_SEC / max_tick << "; Total cycle1 time: " << (double)sum_cycle1 / CLOCKS_PER_SEC  << "\n\n";
            cout << "Cycle2 time per tick: " << (double)sum_cycle2 / CLOCKS_PER_SEC / max_tick << "; Total cycle2 time: " << (double)sum_cycle2 / CLOCKS_PER_SEC  << "\n";
            cout << "In cycle2 we call propagate_flow" << '\n';
            cout << "Propagate flow calls: " << propagate_flow_number << "; Average propagate flow time per tick: "<< (double)sum_pf_time / CLOCKS_PER_SEC / max_tick << "; Total propagate flow time: " << (double)sum_pf_time / CLOCKS_PER_SEC  << "\n\n";
            cout << "Cycle3 time per tick: " << (double)sum_cycle3 / CLOCKS_PER_SEC / max_tick << "; Total cycle3 time: " << (double)sum_cycle3 / CLOCKS_PER_SEC  << "\n\n";

            cout << "Cycle4 time per tick: " << (double)sum_cycle4 / CLOCKS_PER_SEC / max_tick << "; Total cycle4 time: " << (double)sum_cycle4 / CLOCKS_PER_SEC  << "\n";
            cout << "In cycle4 we call propagate_stop, move_prob, propagate_move" << '\n';
            cout << "Propagate stop calls: " << propagate_stop_number << "; Average propagate stop time per tick: "<< (double)sum_ps_time / CLOCKS_PER_SEC / max_tick << "; Total propagate stop time: " << (double)sum_ps_time / CLOCKS_PER_SEC  << "\n";
            cout << "Move probability calls: " << move_prob_number << "; Average move probability time per tick: "<< (double)sum_mp_time / CLOCKS_PER_SEC / max_tick << "; Total move probability time: " << (double)sum_mp_time / CLOCKS_PER_SEC  << "\n";
            cout << "Propagate move calls: " << propagate_move_number << "; Average propagate move time per tick: "<< (double)sum_pm_time / CLOCKS_PER_SEC / max_tick << "; Total propagate move time: " << (double)sum_pm_time / CLOCKS_PER_SEC  << "\n\n";
        }
        
    };

    static constexpr std::array<pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

    template <typename T>
    struct VectorField {
        array<T, deltas.size()> v[FieldN][FieldM];
        T &add(int x, int y, int dx, int dy, T dv) {
            return get(x, y, dx, dy) += dv;
        }

        T &get(int x, int y, int dx, int dy) {
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
   
    vector<pair<int, int>> flow_candidates;
    int last_use[FieldN][FieldM]{};
    int UT = 2;
    int dirs[FieldN][FieldM]{};
    int last_prob = 0;
    TimeMeasurer tm;
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
                    this->p[i][j] = static_cast<PTYPE>(d);
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

    Simulation() {
        //signal(SIGQUIT, signal_handler);

        rnd = mt19937(1937);

        for (size_t x = 0; x < FieldN; ++x) {
            for (size_t y = 0; y < FieldM; ++y) {
                flow_candidates.push_back({x, y});
            }
        }

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
        clock_t beg = clock();
        
        last_use[x][y] = UT - 1;
        VTYPE ret = 0;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;

            if (field[nx][ny] != '#' && (last_use[nx][ny] < UT || dx == 1)) {
                auto cap = velocity.get(x, y, dx, dy);
                auto flow = velocity_flow.get(x, y, dx, dy);

                if (flow == cap) {
                    continue;
                }
                
                VFLOWTYPE vp = min(VFLOWTYPE(lim), cap - VFLOWTYPE(flow));
                
                if (last_use[nx][ny] == UT - 1) {
                    velocity_flow.add(x, y, dx, dy, vp);
                    last_use[x][y] = UT;

                    clock_t end = clock();
                    tm.sum_pf_time += (end - beg);
                    tm.propagate_flow_number++;

                    return {vp, true, {nx, ny}};
                }
                auto [t, prop, end] = propagate_flow(nx, ny, vp);
                ret += t;
                if (prop) {
                    velocity_flow.add(x, y, dx, dy, VFLOWTYPE(t));
                    last_use[x][y] = UT;

                    clock_t endt = clock();
                    tm.sum_pf_time += (endt - beg);
                    tm.propagate_flow_number++;
                    return {t, prop && end != pair(x, y), end};
                }
            }
        }
        last_use[x][y] = UT;

        clock_t end = clock();
        tm.sum_pf_time += (end - beg);
        tm.propagate_flow_number++;

        return {ret, false, {0, 0}};
    }

    //dangerous for double, float
    VTYPE random01() {
        double rand = ((double)rnd() / UINT_MAX);
        VTYPE ans = static_cast<VTYPE>(rand);
        return ans;
    }

    void propagate_stop(int x, int y, bool force = false) {
        clock_t beg = clock();

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
                clock_t end = clock();
                tm.sum_ps_time += (end - beg);
                tm.propagate_stop_number++;
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

        clock_t end = clock();
        tm.sum_ps_time += (end - beg);
        tm.propagate_stop_number++;
    }

    VTYPE move_prob(int x, int y) {
        clock_t beg = clock();
        VTYPE sum = 0;
        for (size_t i = 0; i < deltas.size(); ++i) {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                continue;
            }

            auto v = velocity.get(x, y, dx, dy);
            
            if (v < 0) {
                continue;
            }
            sum += v;
        }

        clock_t end = clock();
        tm.sum_mp_time += (end - beg);
        tm.move_prob_number++;
        return sum;
    }

    //why unreachable code
    bool propagate_move(int x, int y, bool is_first) {
        clock_t beg = clock();

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

            size_t d = std::ranges::upper_bound(tres, p) - tres.begin();

            auto [dx, dy] = deltas[d];
            nx = x + dx;
            ny = y + dy;
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

        clock_t end = clock();
        tm.sum_pm_time += (end - beg);
        tm.propagate_move_number++;
        return ret;
    }


    void Simulate() {
        PTYPE total_delta_p = 0;

        while (true) {
            //this cycle is fast. 
            // Apply external forces
            for (size_t x = 0; x < FieldN; ++x) {
                for (size_t y = 0; y < FieldM; ++y) {
                    if (field[x][y] == '#') {
                        continue;
                    }
                    if (field[x + 1][y] != '#') {
                        velocity.add(x, y, 1, 0, g);
                    }
                }
            }


            // Apply forces from p
            //Just calculations for velocity
            clock_t beg = clock();
            memcpy(old_p, p, sizeof(p));
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

                            force -= PTYPE(contr * VTYPE(rho[(int) field[nx][ny]]));
                            contr = 0;
   
                            velocity.add(x, y, dx, dy, VTYPE(force / rho[(int) field[x][y]]));


                            p[x][y] -= force / (double)dirs[x][y];
                            total_delta_p -= force / (double)dirs[x][y];
                        }
                    }
                }
            }
            clock_t endt = clock();
            tm.sum_cycle1 += (endt - beg);


            // cycle 2. prop_flow
            // Make flow from velocities
            beg = clock();
            velocity_flow = {};
            bool prop = false;

            int ref = 0;

            do {
                UT += 2;
                prop = 0;

                int ln = flow_candidates.size();

                set<pair<int, int>> inside_queue;
                vector <pair<int, int>> newq;

                for (auto [x, y] : flow_candidates){
                    if (last_use[x][y] != UT) {
                        auto [t, local_prop, _] = propagate_flow(x, y, 1);
                        if (t > 0) {
                            prop = 1;

                            if (inside_queue.count({x, y}) == 0) {
                                newq.push_back({x, y});
                                inside_queue.insert({x, y});
                            }

                            vector<pair<int, int>> new_delt{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

                            for (int u = 1; u < 3; u++) {
                                for (auto [dx, dy]: new_delt) {
                                int nx = x + dx * u, ny = y + dy * u;

                                if (nx > 0 && ny > 0 && nx < FieldN && ny < FieldM && field[nx][ny] != '#') {
                                    if (inside_queue.count({nx, ny}) == 0) {
                                        newq.push_back({nx, ny});
                                        inside_queue.insert({nx, ny});
                                    }
                            
                                }
                                }
                            }
                            
                        }
                    } else if (last_use[x][y] == UT) {
                        if (inside_queue.count({x, y}) == 0) {
                            newq.push_back({x, y});
                            inside_queue.insert({x, y});
                        }
                    }
                }

                swap(newq, flow_candidates);
                inside_queue.clear();
                newq.clear();
                if (flow_candidates.size() < 75 || (last_prob == 0 && prop == 0) ) {
                    ref = 1;
                    for (size_t x = 0; x < FieldN; ++x) {
                        for (size_t y = 0; y < FieldM; ++y) {
                            flow_candidates.push_back({x, y});
                        }
                    }
                }

                if (prop) ref = 1; 

            } while (prop);

            last_prob = ref;

            endt = clock();
            tm.sum_cycle2 += (endt - beg);


            //cycle 3. just calculations for p
            // Recalculate p with kinetic energy
            beg = clock();
            for (size_t x = 0; x < FieldN; ++x) {
                for (size_t y = 0; y < FieldM; ++y) {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy] : deltas) {
                        auto old_v = velocity.get(x, y, dx, dy);
                        auto new_v = VTYPE(velocity_flow.get(x, y, dx, dy));

                        if (old_v > 0) {
                            assert(new_v <= old_v);
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
            endt = clock();
            tm.sum_cycle3 += (endt - beg);


            //cycle 4; prop_move, prop_stop
            beg = clock();
            UT += 2;
            prop = false;
            for (size_t x = 0; x < FieldN; ++x) {
                for (size_t y = 0; y < FieldM; ++y) {
                    if (field[x][y] != '#' && last_use[x][y] != UT) {
                        if (random01() < move_prob(x, y)) {
                            prop = true;
                            propagate_move(x, y, true);
                        } else {
                            propagate_stop(x, y, true);
                        }
                    }
                }
            }
            endt = clock();
            tm.sum_cycle4 += (endt - beg);

            if (prop) {
                cout << "Tick " << tick_i << ":\n";
                for (size_t x = 0; x < FieldN; ++x) {
                    for (size_t y = 0; y < FieldM; ++y) {
                        std::cout << field[x][y];
                    }
                    std::cout << std::endl;
                }
            }

            tick_i++;

            if (tm.active && tm.max_tick == tick_i) {
                break;
            }
        }
    }
};

