#include "homework.h"

int main(int argc, char** argv) {
    std::cout << "=== HOMEWORK TESTS ===" << std::endl << std::endl;

    std::vector<Student> db = load_students("students.csv");
    std::cout << "Loaded " << db.size() << " students total.\n\n";

    // Variant A
    std::cout << "=== Variant A (unordered_map + unordered_map) ===" << std::endl;
    {
        StudentDBVariantA storage(db);

        auto born_8_13 = storage.findByBirthday(8, 13);
        std::cout << "People with birthday 8/13: " << born_8_13.size() << "\n";

        std::cout << "Initially best group: "
                  << storage.groupWithMostSameBirthday() << "\n";

        if (!born_8_13.empty()) {
            const Student* someone = born_8_13[0];
            std::string email = someone->m_email;

            storage.changeGroupByEmail(email, "ZZZ-99");

            std::cout << "After moving " << email
                      << " to ZZZ-99, best group is now: "
                      << storage.groupWithMostSameBirthday() << "\n";
        }
        std::cout << std::endl;
    }

    // Variant B
    std::cout << "=== Variant B (map + map) ===" << std::endl;
    {
        StudentDBVariantB storage(db);

        auto born_8_13 = storage.findByBirthday(8, 13);
        std::cout << "People with birthday 8/13: " << born_8_13.size() << "\n";

        std::cout << "Initially best group: "
                  << storage.groupWithMostSameBirthday() << "\n";

        if (!born_8_13.empty()) {
            const Student* someone = born_8_13[0];
            std::string emailToMove = someone->m_email;

            storage.changeGroupByEmail(emailToMove, "ZZZ-99");

            std::cout << "After moving " << emailToMove
                      << " to ZZZ-99, best group is now: "
                      << storage.groupWithMostSameBirthday() << "\n";
        }
        std::cout << std::endl;
    }

    // Variant C
    std::cout << "=== Variant C (unordered_map + vector buckets) ===" << std::endl;
    {
        StudentDBVariantC storage(db);

        auto born_8_13 = storage.findByBirthday(8, 13);
        std::cout << "People with birthday 8/13: " << born_8_13.size() << "\n";

        std::cout << "Initially best group (op3): "
                  << storage.groupWithMostSameBirthday() << "\n";

        if (!born_8_13.empty()) {
            std::string emailToMove = born_8_13[0]->m_email;
            storage.changeGroupByEmail(emailToMove, "ZZZ-99");

            std::cout << "After moving " << emailToMove
                      << " to ZZZ-99, best group is now: "
                      << storage.groupWithMostSameBirthday() << "\n";
        }
        std::cout << std::endl;
    }

    //Task III
    std::cout << "=== STask III ===" << std::endl;
    
    {
        auto v = sort_by_phone_std_copy(db);
        write_students_csv(v, "sorted_by_phone_std.csv");
        std::cout << "Saved sorted (std::sort) to sorted_by_phone_std.csv\n";
    }

    {
        auto v = sort_by_phone_radix_copy(db);
        write_students_csv(v, "sorted_by_phone_radix.csv");
        std::cout << "Saved sorted (radix sort) to sorted_by_phone_radix.csv\n";
    }

    std::vector<size_t> sort_sizes = {100, 1000, 10000, 100000};
    benchmark_sorting_noio_to_file(db, sort_sizes, "sorting_benchmark.csv");
    std::cout << std::endl;

    //Task II
    std::cout << "=== Task II ===" << std::endl;
    
    std::string files_csv = "students_100.csv,students_1000.csv,students_10000.csv,students_100000.csv";
    std::string variant = "ALL";
    int seconds = 10;
    std::string out_csv = "bench_results.csv";

    for (int i = 1; i < argc; ++i) {
        if (std::strncmp(argv[i], "--files=", 8) == 0)
            files_csv = argv[i] + 8;
        else if (std::strncmp(argv[i], "--variant=", 10) == 0)
            variant = argv[i] + 10;
        else if (std::strncmp(argv[i], "--seconds=", 10) == 0)
            seconds = std::atoi(argv[i] + 10);
        else if (std::strncmp(argv[i], "--out=", 6) == 0)
            out_csv = argv[i] + 6;
    }

    std::cout << "Parameters:\n"
              << "  Files: " << files_csv << "\n"
              << "  Variant: " << variant << "\n"
              << "  Duration: " << seconds << " seconds\n"
              << "  Output: " << out_csv << "\n\n";

    std::vector<std::string> files;
    {
        size_t i = 0;
        while (i < files_csv.size()) {
            size_t j = i;
            while (j < files_csv.size() && files_csv[j] != ',') ++j;
            files.push_back(files_csv.substr(i, j - i));
            i = (j == files_csv.size() ? j : j + 1);
        }
    }

    std::ofstream out(out_csv);
    out << "variant,N,ops,seconds,rss_bytes\n";

    std::random_device rd;
    std::mt19937 gen(rd());

    for (const auto& file : files) {
        auto db_subset = load_students(file);
        if (db_subset.empty()) {
            std::cerr << "Skip '" << file << "' (empty or failed to load)\n";
            continue;
        }
        const size_t N = db_subset.size();
        std::cout << "Testing with N=" << N << " students from " << file << "\n";

        auto run_one = [&](char v) {
            size_t rss_before = current_rss_bytes();
            size_t ops = 0;
            size_t rss_after = 0;

            if (v == 'A') {
                StudentDBVariantA storage(db_subset);
                rss_after = current_rss_bytes();
                ops = run_mix(storage, db_subset, seconds, gen);
            } else if (v == 'B') {
                StudentDBVariantB storage(db_subset);
                rss_after = current_rss_bytes();
                ops = run_mix(storage, db_subset, seconds, gen);
            } else {
                StudentDBVariantC storage(db_subset);
                rss_after = current_rss_bytes();
                ops = run_mix(storage, db_subset, seconds, gen);
            }

            size_t rss_delta = (rss_after > rss_before) ? (rss_after - rss_before) : 0;
            out << v << "," << N << "," << ops << "," << seconds << "," << rss_delta << "\n";
            
            std::cout << "  [Variant " << v << "] "
                      << ops << " ops/" << seconds << "s, "
                      << "memory: " << (rss_delta / (1024.0 * 1024.0)) << " MiB\n";
        };

        if (variant == "A" || variant == "ALL") run_one('A');
        if (variant == "B" || variant == "ALL") run_one('B');
        if (variant == "C" || variant == "ALL") run_one('C');
        
        std::cout << std::endl;
    }

    std::cout << "Benchmark results saved to " << out_csv << "\n";
    std::cout << "\n=== ALL TESTS COMPLETED ===" << std::endl;
    
    return 0;
}