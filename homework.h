//
// Created by Admin on 03.11.2025.
//

#ifndef HOMEWORK1_HOMEWORK_H
#define HOMEWORK1_HOMEWORK_H

#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <map>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <random>

struct Student {
    std::string m_name;
    std::string m_surname;
    std::string m_email;
    int m_birth_year;
    int m_birth_month;
    int m_birth_day;
    std::string m_group;
    float m_rating;
    std::string m_phone_number;
};

inline int birthday_index(int month, int day) {
    return (month - 1) * 31 + (day - 1);
}

std::vector<Student> load_students(const std::string &file_name);

class StudentDBVariantA {
public:
    explicit StudentDBVariantA(std::vector<Student> &all);
    std::vector<const Student*> findByBirthday(int month, int day) const;
    void changeGroupByEmail(const std::string &email, const std::string &newGroup);
    std::string groupWithMostSameBirthday() const;

private:
    struct GroupStats {
        std::array<int, 372> counts{};
        int currentMax = 0;
    };

    std::vector<Student> &allStudents;
    std::unordered_map<std::string, Student*> byEmail;
    std::unordered_map<int, std::vector<const Student*>> byBirthday;
    std::unordered_map<std::string, GroupStats> groupStats;
    std::string globalBestGroup;
    int globalBestValue;
};

class StudentDBVariantB {
public:
    explicit StudentDBVariantB(std::vector<Student>& all);
    std::vector<const Student*> findByBirthday(int month, int day) const;
    void changeGroupByEmail(const std::string& email, const std::string& newGroup);
    std::string groupWithMostSameBirthday() const;

private:
    struct GroupStats {
        std::array<int, 372> counts{};
        int currentMax = 0;
    };

    std::vector<Student>& allStudents;
    std::map<std::string, Student*> byEmail;
    std::map<int, std::vector<const Student*>> byBirthday;
    std::map<std::string, GroupStats> groupStats;
    std::string globalBestGroup;
    int globalBestValue;
};

class StudentDBVariantC {
public:
    explicit StudentDBVariantC(std::vector<Student>& all);
    std::vector<const Student*> findByBirthday(int month, int day) const;
    void changeGroupByEmail(const std::string& email, const std::string& newGroup);
    std::string groupWithMostSameBirthday() const;

private:
    struct GroupStats {
        std::vector<int> counts;
        int currentMax = 0;
    };

    std::vector<Student>& allStudents;
    std::unordered_map<std::string, Student*> byEmail;
    std::vector<std::vector<const Student*>> byBirthdayBuckets;
    std::unordered_map<std::string, GroupStats> groupStats;
    std::string globalBestGroup;
    int globalBestValue;
};

void write_students_csv(const std::vector<Student>& students,
                        const std::string& out_path);

void sort_by_phone_std_inplace(std::vector<Student>& v);
std::vector<Student> sort_by_phone_std_copy(const std::vector<Student>& src);

void sort_by_phone_radix_inplace(std::vector<Student>& v);
std::vector<Student> sort_by_phone_radix_copy(const std::vector<Student>& src);

std::string digits_only(const std::string& s);

size_t current_rss_bytes();

void benchmark_sorting_noio_to_file(const std::vector<Student>& db,
                                   const std::vector<size_t>& sizes,
                                   const std::string& out_csv_path);

std::string rand_group(const std::vector<Student>& db, std::mt19937& gen);
void rand_birthday(const std::vector<Student>& db, int& m, int& d, std::mt19937& gen);
const std::string& rand_email(const std::vector<Student>& db, std::mt19937& gen);

template <class Storage>
size_t run_mix(Storage& storage,
               const std::vector<Student>& db_view,
               int seconds, std::mt19937& gen);

#endif
