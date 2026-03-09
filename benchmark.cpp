
#include "homework.h"

#include <windows.h>
#include <psapi.h>
//ONLY FOR WINDOWS, BECAUSE I USE WINDOWS!!!
size_t current_rss_bytes() {
    PROCESS_MEMORY_COUNTERS pmc{};
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<size_t>(pmc.WorkingSetSize);
    }
    return 0;
}

const std::string& rand_email(const std::vector<Student>& db, std::mt19937& gen) {
    std::uniform_int_distribution<size_t> dist(0, db.size()-1);
    return db[dist(gen)].m_email;
}
void rand_birthday(const std::vector<Student>& db, int& m, int& d, std::mt19937& gen) {
    std::uniform_int_distribution<size_t> dist(0, db.size()-1);
    const auto& s = db[dist(gen)];
    m = s.m_birth_month; d = s.m_birth_day;
}
std::string rand_group(const std::vector<Student>& db, std::mt19937& gen) {
    std::uniform_int_distribution<size_t> dist(0, db.size()-1);
    return db[dist(gen)].m_group;
}

template <class Storage>
size_t run_mix(Storage& storage,
               const std::vector<Student>& db_view,
               int seconds, std::mt19937& gen)
{
    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();
    size_t ops = 0;

    std::uniform_int_distribution<int> pick(0,110);

    while (true) {
        auto now = clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >= seconds) break;

        int r = pick(gen);
        if (r < 10) {
            int m, d; rand_birthday(db_view, m, d, gen);
            auto res = storage.findByBirthday(m, d);
            (void)res;
        } else if (r < 110) {
            const std::string& email = rand_email(db_view, gen);
            std::string g = rand_group(db_view, gen);
            storage.changeGroupByEmail(email, g);
        } else {
            auto g = storage.groupWithMostSameBirthday();
            (void)g;
        }

        ++ops;
    }
    return ops;
}

template size_t run_mix<StudentDBVariantA>(
    StudentDBVariantA&,
    const std::vector<Student>&,
    int,
    std::mt19937&);

template size_t run_mix<StudentDBVariantB>(
    StudentDBVariantB&,
    const std::vector<Student>&,
    int,
    std::mt19937&);

template size_t run_mix<StudentDBVariantC>(
    StudentDBVariantC&,
    const std::vector<Student>&,
    int,
    std::mt19937&);

static inline long long time_ms(std::function<void()> fn) {
      using clock = std::chrono::high_resolution_clock;
      auto t0 = clock::now();
      fn();
      auto t1 = clock::now();
      return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
  }
static inline std::vector<Student> first_n(const std::vector<Student>& all, size_t N) {
      N = std::min(N, all.size());
      return std::vector<Student>(all.begin(), all.begin() + N);
  }

void benchmark_sorting_noio_to_file(const std::vector<Student>& db,
                                           const std::vector<size_t>& sizes,
                                           const std::string& out_csv_path)
  {
      std::ofstream csv(out_csv_path);
      if (!csv.is_open()) {
          std::cerr << "Failed to open " << out_csv_path << " for writing\n";
          return;
      }
      csv << "algo,N,time_ms\n";

      for (size_t N : sizes) {
          if (N == 0 || N > db.size()) continue;

          {
              auto v = first_n(db, N);
              long long ms = time_ms([&]{ sort_by_phone_std_inplace(v); });
              csv << "std," << N << "," << ms << "\n";
              std::cout << "[sort] std N=" << N << " => " << ms << " ms\n";
          }
          {
              auto v = first_n(db, N);
              long long ms = time_ms([&]{ sort_by_phone_radix_inplace(v); });
              csv << "radix," << N << "," << ms << "\n";
              std::cout << "[sort] radix N=" << N << " => " << ms << " ms\n";
          }
      }
      std::cout << "Sorting results saved to " << out_csv_path << "\n";
  }
