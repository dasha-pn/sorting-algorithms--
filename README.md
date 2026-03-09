# Algorithms Homework 1 Report

## 1. Student Database Representation

### Goal

The goal of this task is to create an **in-memory representation of a student database** loaded from a CSV file `students.csv`.

Each record contains the following fields:

- first name
- last name
- email
- birth date
- group
- rating
- phone number

The function `load_students()` reads the CSV file and constructs objects of type `Student`.

### Algorithm

The loading process performs the following steps:

1. Skip the header row.
2. Read the file line by line.
3. Split fields using `,`.
4. Remove extra spaces using `trim()`.
5. Convert numeric fields using `stoi` and `stof`.
6. Create a `Student` object and store it in `std::vector<Student>`.

To improve performance, memory is reserved in advance:
```
students.reserve(100000)
```


### Complexity

Parsing each line takes

$$
O(L)
$$

where $L$ is the length of the line.

For $N$ students the total complexity is

$$
O(N)
$$

Memory usage:

$$
O(N)
$$

since exactly $N$ `Student` structures are stored.

---

# 2. Data Structures for Operations (Variant V2)

Three operations must be supported:

1. `findByBirthday(month, day)`
2. `changeGroupByEmail(email, newGroup)`
3. `groupWithMostSameBirthday()`

Operation frequency:

$$
A : B : C = 10 : 100 : 1
$$

which corresponds approximately to:

- $9.1\%$ — search by birthday
- $90.9\%$ — change group by email
- $0.9\%$ — global query

Therefore the structure must support:

- fast access by email
- efficient update of group statistics.

Three implementations were evaluated.

---

# Variant A — Hash Table Based

## Data Structures
```
unordered_map<string, Student*> byEmail
unordered_map<int, vector<const Student*>> byBirthday
unordered_map<string, GroupStats> groupStats
```


Where:

- `byEmail` enables fast lookup by email
- `byBirthday` stores lists of students for each date
- `groupStats` maintains statistics for each group.

Each group stores:

- `counts[372]` — number of students for each possible birthday
- `currentMax` — maximum number of students sharing the same birthday.

The global maximum is stored in
```
globalBestGroup
globalBestValue
```


---

## Complexity

| Operation | Complexity |
|-----------|------------|
| findByBirthday | $O(1)$ |
| changeGroupByEmail | $O(1) + O(372)$ |
| groupWithMostSameBirthday | $O(1)$ |

---

## Advantages

- very fast email lookup
- very fast birthday search

## Disadvantages

- large memory overhead due to multiple hash tables.

Memory usage for $N = 100000$:

$$
\approx 19.7 \text{ MiB}
$$

---

# Variant B — Tree-Based Structure

This version uses `std::map` instead of `unordered_map`.

## Data Structures
```
map<string, Student*> byEmail
map<int, vector<const Student*>> byBirthday
map<string, GroupStats> groupStats
```


---

## Complexity

| Operation | Complexity |
|-----------|------------|
| findByBirthday | $O(\log N)$ |
| changeGroupByEmail | $O(\log N) + O(372)$ |
| groupWithMostSameBirthday | $O(1)$ |

---

## Advantages

- deterministic ordering of elements
- lower memory usage than Variant A.

## Disadvantages

- slower access due to tree-based structures.

---

# Variant C — Vector Bucket Structure

Variant C uses **vector buckets instead of hash tables**.

Instead of mapping dates dynamically, all possible birthdays are mapped to a fixed index.

The index is computed as:

$$
idx = (month - 1) \times 31 + (day - 1)
$$

This creates a grid of

$$
12 \times 31 = 372
$$

possible buckets.

Data structure:
```vector<vector<const Student*>> byBirthdayBuckets```

---

## Complexity

| Operation | Complexity |
|-----------|------------|
| findByBirthday | $O(1)$ |
| changeGroupByEmail | $O(1) + O(372)$ |
| groupWithMostSameBirthday | $O(1)$ |

---

## Advantages

- fastest implementation
- smallest memory consumption
- no hashing overhead.

## Disadvantages

- works only because the domain of dates is limited.

Memory usage for $N = 100000$:

$$
\approx 11.7 \text{ MiB}
$$

---

# Performance Comparison

Throughput results (operations per second):

| Variant | ops/s |
|-------|-------|
| A | $\approx 20000$ |
| B | $\approx 14000$ |
| C | $\approx 23000$ |

Variant C provides the highest throughput.

---

# Memory Usage

| Variant | Memory |
|-------|--------|
| A | 19.7 MiB |
| B | 13.7 MiB |
| C | 11.7 MiB |

Variant C also provides the most efficient memory usage.

---

# 3. Sorting Algorithm Comparison

Two sorting algorithms were compared:

- `std::sort` (introsort)
- radix sort

Experimental results show:

- for small arrays ($N \le 1000$), `std::sort` is faster
- for large arrays ($N \ge 10000$), radix sort becomes faster.

This occurs because:

- `std::sort` has complexity

$$
O(N \log N)
$$

- radix sort approaches

$$
O(N)
$$

for fixed-length keys.

---

# Final Conclusions

- **Variant C** provides the best overall performance in both time and memory.
- **Variant A** offers fast access but requires significantly more memory.
- **Variant B** is slower but maintains deterministic ordering.

For sorting:

- `std::sort` is preferable for small datasets.
- radix sort performs better for large datasets with numeric keys.

These experiments demonstrate that **the choice of data structures strongly affects both performance and memory usage**.
