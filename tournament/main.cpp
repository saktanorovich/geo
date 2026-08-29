/**
 * Sith Tournament — who can possibly win.
 *
 * N Sith fight king-of-the-hill in a random order: two start, the loser is
 * replaced by the next Sith, until one survivor remains. Each Sith has three
 * parameters and beats another when he is greater in at least two of them. All
 * values within a parameter are distinct, so every duel has a definite winner.
 * Task: print every Sith who can win the tournament under some order.
 *
 * Key fact: if x can win, then anyone who beats x can win too (put x right
 * before them). So the answer is the set reachable, along the "is beaten by"
 * relation, from one Sith who is already a possible winner.
 *
 * Algorithm — O(N log N):
 *   1. Rank-compress each parameter independently to 1..N.
 *   2. Find one maximal Sith by a linear scan; he seeds the winner set.
 *   3. BFS the closure: for a winner p, every Sith who beats p is also a winner.
 *      Beating p means winning two of the three parameters — pairs (0,1), (1,2),
 *      (2,0). For each pair (a,b) a max segment tree keyed on parameter a and
 *      holding parameter b answers "among Sith with a > p.a, is the best b > p.b?"
 *      in O(log N). A found winner is removed from the trees, so each is queued
 *      once.
 *
 * I/O uses scanf/printf, ~4x faster than iostream on this input.
 */

#include <algorithm>
#include <array>
#include <cstdio>
#include <iostream>
#include <vector>
using namespace std;

inline void init() {
    ios::sync_with_stdio(false);
}

struct person {
    char name[12];
    int id, power[3];
    bool operator<(person const &p) const {
        return id < p.id;
    }
};

struct comparer {
    int type;
    comparer(int type) : type(type) { }
    bool operator()(person const &a, person const &b) {
        return a.power[type] < b.power[type];
    }
};

// Storage is sized from N, read at startup. The iterative segment tree needs no
// power-of-two padding, so its leaf space is exactly N.
int npersons, head, teil;
vector<person> persons;
vector<array<int, 3>> person_by_power;  // [N + 1], indexed by a parameter value
vector<array<int, 3>> tree;             // [2 * N], one max segment tree per type
vector<int> possible;
vector<int> tournament_queue;

// Iterative max segment tree over positions 1..N. Leaves at tree[N + pos - 1];
// node i holds max(children 2*i, 2*i+1). Point assignment, one O(log N) walk.
void set(int type, int pos, int value) {
    int i = npersons + pos - 1;
    tree[i][type] = value;
    for (i >>= 1; i >= 1; i >>= 1) {
        tree[i][type] = max(tree[2 * i][type], tree[2 * i + 1][type]);
    }
}

// Max over the 1-indexed inclusive range [ql, qh]; 0 when the range is empty.
int get(int type, int ql, int qh) {
    int res = 0;
    for (int l = npersons + ql - 1, r = npersons + qh; l < r; l >>= 1, r >>= 1) {
        if (l & 1) res = max(res, tree[l++][type]);
        if (r & 1) res = max(res, tree[--r][type]);
    }
    return res;
}

bool beats(person const &a, person const &b) {
    int wins = 0;
    for (int i = 0; i < 3; ++i) {
        if (a.power[i] > b.power[i]) {
            ++wins;
        }
    }
    return wins > 1;
}

void remove_from_trees(int p) {
    set(0, persons[p].power[0], 0);
    set(1, persons[p].power[1], 0);
    set(2, persons[p].power[2], 0);
}

// Enqueue every not-yet-seen Sith who beats p on parameters (a, b).
void process(person const &p, int a, int b) {
    while (true) {
        int power = get(a, p.power[a] + 1, npersons);
        if (power <= p.power[b]) {
            break;
        }
        remove_from_trees(person_by_power[power][a]);
        tournament_queue[++teil] = person_by_power[power][a];
    }
}

int main() {
    init();
    scanf("%d", &npersons);
    if (npersons < 1) {
        return 0;
    }
    persons.resize(npersons);
    person_by_power.assign(npersons + 1, {});
    tree.assign(2 * npersons, {});
    possible.assign(npersons, 0);
    tournament_queue.resize(npersons);

    for (int i = 0; i < npersons; ++i) {
        persons[i].id = i;
        scanf("%s", persons[i].name);
        for (int j = 0; j < 3; ++j) {
            scanf("%d", &persons[i].power[j]);
        }
    }

    // Rank-compress each parameter to 1..N, then restore original id order.
    for (int j = 0; j < 3; ++j) {
        sort(persons.begin(), persons.end(), comparer(j));
        for (int i = 0; i < npersons; ++i) {
            persons[i].power[j] = i + 1;
        }
    }
    sort(persons.begin(), persons.end());

    // Seed the segment trees: tree type k is keyed on one parameter and stores
    // the next parameter of the same Sith, so a range max recovers a rival.
    for (int i = 0; i < npersons; ++i) {
        person_by_power[persons[i].power[1]][0] = i;
        person_by_power[persons[i].power[2]][1] = i;
        person_by_power[persons[i].power[0]][2] = i;
        set(0, persons[i].power[0], persons[i].power[1]);
        set(1, persons[i].power[1], persons[i].power[2]);
        set(2, persons[i].power[2], persons[i].power[0]);
    }

    int seed = 0;
    for (int i = 1; i < npersons; ++i) {
        if (beats(persons[i], persons[seed])) {
            seed = i;
        }
    }

    head = teil = 0;
    for (tournament_queue[head] = seed, remove_from_trees(seed); head <= teil;) {
        int cur = tournament_queue[head];
        ++head;
        possible[cur] = 1;
        process(persons[cur], 0, 1);
        process(persons[cur], 1, 2);
        process(persons[cur], 2, 0);
    }

    for (int i = 0; i < npersons; ++i) {
        if (possible[i]) {
            printf("%s\n", persons[i].name);
        }
    }
    return 0;
}
