//
// Created by Admin on 24.10.2025.
//

#include "homework.h"
#include <fstream>
#include <sstream>
#include <cctype>

static std::string trim(const std::string &s) {
    size_t left = 0;
    size_t right = s.size();

    while (left < right && std::isspace(s[left])) {
        left++;
    }

    while (right > left && std::isspace(s[right - 1])) {
        right--;
    }

    return s.substr(left, right - left);
}

static std::vector<std::string> parse_one_student(const std::string &line) {
    std::vector<std::string> result;
    std::string cur_word;
    result.reserve(9);

    for (const char i : line) {
        if (i != ',') {
            cur_word += i;
        } else {
            result.push_back(cur_word);
            cur_word.clear();
        }
    }
    result.push_back(cur_word);

    for (std::string &field : result) {
        field = trim(field);
    }

    return result;
}

static Student build_student(const std::vector<std::string> &fields) {
    Student student;

    student.m_name = fields[0];
    student.m_surname = fields[1];
    student.m_email = fields[2];

    student.m_birth_year = std::stoi(fields[3]);
    student.m_birth_month = std::stoi(fields[4]);
    student.m_birth_day = std::stoi(fields[5]);

    student.m_group = fields[6];

    student.m_rating = std::stof(fields[7]);

    student.m_phone_number = fields[8];

    return student;
}

std::vector<Student> load_students(const std::string &file_name) {
    std::vector<Student> students;
    students.reserve(100000);

    std::ifstream fin(file_name);

    std::string line;

    if (!getline(fin, line)) {
        return students;
    }

    while (getline(fin, line)) {
        std::string new_string = trim(line);

        if (new_string.empty()) {
            continue;
        }

        std::vector<std::string> fields = parse_one_student(new_string);

        Student student = build_student(fields);
        students.push_back(std::move(student));
    }
    return students;
}

//Variant A task II
StudentDBVariantA::StudentDBVariantA(std::vector<Student> &all)
    : allStudents(all),
      globalBestValue(0),
      globalBestGroup("")
{
    byEmail.reserve(allStudents.size());
    byBirthday.reserve(allStudents.size());

    for (Student &st : allStudents) {
        byEmail[st.m_email] = &st;

        int key = st.m_birth_month * 100 + st.m_birth_day;
        byBirthday[key].push_back(&st);

        int idx = birthday_index(st.m_birth_month, st.m_birth_day);
        GroupStats &gs = groupStats[st.m_group];

        gs.counts[idx] += 1;
        if (gs.counts[idx] > gs.currentMax) {
            gs.currentMax = gs.counts[idx];
        }

        if (gs.currentMax > globalBestValue) {
            globalBestValue = gs.currentMax;
            globalBestGroup = st.m_group;
        }
    }
}

std::vector<const Student*> StudentDBVariantA::findByBirthday(int month, int day) const {
    int key = month * 100 + day;
    auto it = byBirthday.find(key);
    if (it == byBirthday.end()) {
        return {};
    }
    return it->second;
}

void StudentDBVariantA::changeGroupByEmail(const std::string &email, const std::string &newGroup) {
    auto pos = byEmail.find(email);
    if (pos == byEmail.end()) {
        return;
    }

    Student *st = pos->second;
    std::string oldGroup = st->m_group;
    if (oldGroup == newGroup) {
        return;
    }

    int idx = birthday_index(st->m_birth_month, st->m_birth_day);

    {
        GroupStats &gsOld = groupStats[oldGroup];
        gsOld.counts[idx] -= 1;

        int newMax = 0;
        for (int c : gsOld.counts) {
            if (c > newMax) newMax = c;
        }
        gsOld.currentMax = newMax;
    }

    {
        GroupStats &gsNew = groupStats[newGroup];
        gsNew.counts[idx] += 1;
        if (gsNew.counts[idx] > gsNew.currentMax) {
            gsNew.currentMax = gsNew.counts[idx];
        }
    }

    st->m_group = newGroup;

    {
        globalBestValue = 0;
        globalBestGroup.clear();

        for (const auto &kv : groupStats) {
            const std::string &gName = kv.first;
            const GroupStats   &gs   = kv.second;
            if (gs.currentMax > globalBestValue) {
                globalBestValue = gs.currentMax;
                globalBestGroup = gName;
            }
        }
    }
}

std::string StudentDBVariantA::groupWithMostSameBirthday() const {
    return globalBestGroup;
}

//Variant B task II
StudentDBVariantB::StudentDBVariantB(std::vector<Student>& all)
    : allStudents(all),
      globalBestValue(0),
      globalBestGroup("")
{
    for (Student& st : allStudents) {
        byEmail[st.m_email] = &st;

        int bdayKey = st.m_birth_month * 100 + st.m_birth_day;
        byBirthday[bdayKey].push_back(&st);

        int idx = birthday_index(st.m_birth_month, st.m_birth_day);

        GroupStats& gs = groupStats[st.m_group];
        gs.counts[idx] += 1;
        if (gs.counts[idx] > gs.currentMax) {
            gs.currentMax = gs.counts[idx];
        }

        if (gs.currentMax > globalBestValue) {
            globalBestValue = gs.currentMax;
            globalBestGroup = st.m_group;
        }
    }
}

std::vector<const Student*> StudentDBVariantB::findByBirthday(int month, int day) const {
    int key = month * 100 + day;
    auto it = byBirthday.find(key);
    if (it == byBirthday.end()) {
        return {};
    }
    return it->second;
}

void StudentDBVariantB::changeGroupByEmail(const std::string& email, const std::string& newGroup) {
    auto pos = byEmail.find(email);
    if (pos == byEmail.end()) {
        return;
    }

    Student* st = pos->second;
    const std::string oldGroup = st->m_group;
    if (oldGroup == newGroup) {
        return;
    }

    int idx = birthday_index(st->m_birth_month, st->m_birth_day);

    {
        GroupStats& gsOld = groupStats[oldGroup];
        gsOld.counts[idx] -= 1;

        int newMax = 0;
        for (int c : gsOld.counts) {
            if (c > newMax) newMax = c;
        }
        gsOld.currentMax = newMax;
    }

    {
        GroupStats& gsNew = groupStats[newGroup];
        gsNew.counts[idx] += 1;
        if (gsNew.counts[idx] > gsNew.currentMax) {
            gsNew.currentMax = gsNew.counts[idx];
        }
    }

    st->m_group = newGroup;

    globalBestValue = 0;
    globalBestGroup.clear();

    for (const auto& kv : groupStats) {
        const std::string& gName = kv.first;
        const GroupStats&  gs    = kv.second;
        if (gs.currentMax > globalBestValue) {
            globalBestValue = gs.currentMax;
            globalBestGroup = gName;
        }
    }
}

std::string StudentDBVariantB::groupWithMostSameBirthday() const {
    return globalBestGroup;
}

//Variant C task II
StudentDBVariantC::StudentDBVariantC(std::vector<Student>& all)
    : allStudents(all),
      globalBestValue(0),
      globalBestGroup("")
{
    byBirthdayBuckets.resize(372);

    for (Student& st : allStudents) {
        byEmail[st.m_email] = &st;

        int idx = birthday_index(st.m_birth_month, st.m_birth_day);
        byBirthdayBuckets[idx].push_back(&st);

        GroupStats& gs = groupStats[st.m_group];
        if (gs.counts.empty()) {
            gs.counts.assign(372, 0);
            gs.currentMax = 0;
        }

        gs.counts[idx] += 1;
        if (gs.counts[idx] > gs.currentMax) {
            gs.currentMax = gs.counts[idx];
        }

        if (gs.currentMax > globalBestValue) {
            globalBestValue = gs.currentMax;
            globalBestGroup = st.m_group;
        }
    }
}

std::vector<const Student*> StudentDBVariantC::findByBirthday(int month, int day) const {
    int idx = birthday_index(month, day);
    if (idx < 0 || idx >= (int)byBirthdayBuckets.size()) {
        return {};
    }
    return byBirthdayBuckets[idx];
}

void StudentDBVariantC::changeGroupByEmail(const std::string& email, const std::string& newGroup) {
    auto pos = byEmail.find(email);
    if (pos == byEmail.end()) {
        return;
    }

    Student* st = pos->second;
    const std::string oldGroup = st->m_group;
    if (oldGroup == newGroup) {
        return;
    }

    int idx = birthday_index(st->m_birth_month, st->m_birth_day);

    {
        GroupStats& gsOld = groupStats[oldGroup];
        gsOld.counts[idx] -= 1;

        int newMax = 0;
        for (int c : gsOld.counts) {
            if (c > newMax) newMax = c;
        }
        gsOld.currentMax = newMax;
    }

    {
        GroupStats& gsNew = groupStats[newGroup];
        if (gsNew.counts.empty()) {
            gsNew.counts.assign(372, 0);
            gsNew.currentMax = 0;
        }
        gsNew.counts[idx] += 1;
        if (gsNew.counts[idx] > gsNew.currentMax) {
            gsNew.currentMax = gsNew.counts[idx];
        }
    }

    st->m_group = newGroup;

    globalBestValue = 0;
    globalBestGroup.clear();

    for (const auto& kv : groupStats) {
        const std::string& gName = kv.first;
        const GroupStats& gs = kv.second;
        if (gs.currentMax > globalBestValue) {
            globalBestValue = gs.currentMax;
            globalBestGroup = gName;
        }
    }
}

std::string StudentDBVariantC::groupWithMostSameBirthday() const {
    return globalBestGroup;
}

//Task III
void write_students_csv(const std::vector<Student>& students,
                        const std::string& out_path) {
    std::ofstream fout(out_path);
    if (!fout.is_open()) {
        std::cerr << "Failed to open " << out_path << " for writing\n";
        return;
    }
    fout << "m_name,m_surname,m_email,m_birth_year,m_birth_month,"
            "m_birth_day,m_group,m_rating,m_phone_number\n";
    for (const auto& s : students) {
        fout << s.m_name << ","
             << s.m_surname << ","
             << s.m_email << ","
             << s.m_birth_year << ","
             << s.m_birth_month << ","
             << s.m_birth_day << ","
             << s.m_group << ","
             << s.m_rating << ","
             << s.m_phone_number << "\n";
    }
    fout.close();
}

std::string digits_only(const std::string& s) {
    std::string out; out.reserve(s.size());
    for (unsigned char c : s)
        if (std::isdigit(c)) out.push_back(char(c));
    return out;
}

void sort_by_phone_std_inplace(std::vector<Student>& v) {
    std::sort(v.begin(), v.end(),
              [](const Student& a, const Student& b) {
                  return a.m_phone_number < b.m_phone_number;
              });
}

std::vector<Student> sort_by_phone_std_copy(const std::vector<Student>& src) {
    std::vector<Student> v = src;
    sort_by_phone_std_inplace(v);
    return v;
}

void sort_by_phone_radix_inplace(std::vector<Student>& v) {
    if (v.empty()) return;

    std::vector<std::string> keys; keys.reserve(v.size());
    size_t L = 0;
    for (const auto& st : v) {
        keys.push_back(digits_only(st.m_phone_number));
        if (L == 0) L = keys.back().size();
    }
    for (const auto& k : keys) {
        if (k.size() != L) {
            std::sort(v.begin(), v.end(), [](const Student& a, const Student& b){
                return digits_only(a.m_phone_number) < digits_only(b.m_phone_number);
            });
            return;
        }
    }

    const int RADIX = 10;
    const size_t n = v.size();
    std::vector<Student> tmp_v(n);
    std::vector<std::string> tmp_k(n);

    for (int pos = int(L) - 1; pos >= 0; --pos) {
        int count[RADIX] = {0};

        for (size_t i = 0; i < n; ++i)
            ++count[keys[i][pos] - '0'];

        int sum = 0;
        for (int d = 0; d < RADIX; ++d) {
            sum += count[d];
            count[d] = sum;
        }

        for (int i = int(n) - 1; i >= 0; --i) {
            int d = keys[i][pos] - '0';
            int j = --count[d];
            tmp_v[j] = std::move(v[i]);
            tmp_k[j] = std::move(keys[i]);
        }
        v.swap(tmp_v);
        keys.swap(tmp_k);
    }
}

std::vector<Student> sort_by_phone_radix_copy(const std::vector<Student>& src) {
    std::vector<Student> v = src;
    sort_by_phone_radix_inplace(v);
    return v;
}