#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>
#include "treapDictionary.h"

using namespace std;

#define REPI(ctr, start, limit) for (uint32_t ctr = (start); ctr < (limit); ctr++)
#define OPF(b) ((b) ? "pass" : "fail")

const uint32_t DEFAULT_NUM_DICTIONARIES = 4, DEFAULT_NUM_ENTRIES = 4096, STRING_LENGTH = 8, TEST_NAME_WIDTH = 16;

uint32_t nDictionaries = DEFAULT_NUM_DICTIONARIES, nEntries = DEFAULT_NUM_ENTRIES;

int main(int argc, char *argv[]) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<> dis(0, 25);
    TreapDictionary<uint32_t, string> *dictionaries = new TreapDictionary<uint32_t, string>[nDictionaries];
    vector<pair<uint32_t, string>> *rawData = new vector<pair<uint32_t, string>>[nDictionaries];
    vector<pair<uint32_t, string>> *sortedData = new vector<pair<uint32_t, string>>[nDictionaries];
    pair<uint32_t, string> entry;
    bool okay;

    REPI(i, 1, argc) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'd') nDictionaries = strtol(argv[i] + 2, nullptr, 10);
            if (argv[i][1] == 'e') nEntries = strtol(argv[i] + 2, nullptr, 10);
        }
    }

    cout << "Dictionaries: " << nDictionaries << endl;
    cout << "Entries per dictionary: " << nEntries << endl;

    for (uint32_t i = 0; i < nDictionaries; i++) {
        for (uint32_t j = 0; j < nEntries; j++) {
            entry.first = mt();
            entry.second.clear();
            for (uint32_t k = 0; k < STRING_LENGTH; k++) {
                entry.second += "abcdefghijklmnopqrstuvwxyz"[dis(mt)];
            }
            rawData[i].push_back(entry);
            sortedData[i].push_back(entry);
        }
        sort(sortedData[i].begin(), sortedData[i].end());
    }

    okay = true;
    REPI(i, 0, nDictionaries) if (!dictionaries[i].isEmpty()) okay = false;
    cout << setw(TEST_NAME_WIDTH) << "Initial empty: " << OPF(okay) << endl;

    okay = true;
    REPI(i, 0, nDictionaries) if (dictionaries[i].size() != 0) okay = false;
    cout << setw(TEST_NAME_WIDTH) << "Initial size: " << OPF(okay) << endl;

    okay = true;
    REPI(i, 0, nDictionaries) if (dictionaries[i].height() != -1) okay = false;
    cout << setw(TEST_NAME_WIDTH) << "Initial height: " << OPF(okay) << endl;

    REPI(i, 0, nDictionaries) REPI(j, 0, nEntries) dictionaries[i][rawData[i][j].first] = rawData[i][j].second;
    cout << setw(TEST_NAME_WIDTH) << "Insert: pass" << endl;

    REPI(i, 0, nDictionaries) cout << setw(TEST_NAME_WIDTH) << "Height " + to_string(i) + ": " << dictionaries[i].height() << endl;

    okay = true;
    REPI(i, 0, nDictionaries) {
        REPI(j, 0, nEntries) {
            try {
                if (dictionaries[i].search(rawData[i][j].first) != rawData[i][j].second) okay = false;
            } catch (const domain_error &e) {
                okay = false;
            }
        }
    }
    cout << setw(TEST_NAME_WIDTH) << "Search: " << OPF(okay) << endl;

    REPI(i, 0, nDictionaries) REPI(j, 0, nEntries / 2) dictionaries[i].remove(rawData[i][2 * j].first);

    okay = true;
    REPI(i, 0, nDictionaries) if (dictionaries[i].size() != nEntries / 2) okay = false;
    cout << setw(TEST_NAME_WIDTH) << "Size after removal: " << OPF(okay) << endl;

    REPI(i, 0, nDictionaries) cout << setw(TEST_NAME_WIDTH) << "Height after removal " + to_string(i) + ": " << dictionaries[i].height() << endl;

    okay = true;
    REPI(i, 0, nDictionaries) {
        REPI(j, 0, nEntries / 2) {
            try {
                dictionaries[i].search(rawData[i][2 * j].first);
                okay = false;
            } catch (const domain_error &) {
            }
            try {
                if (dictionaries[i].search(rawData[i][2 * j + 1].first) != rawData[i][2 * j + 1].second) okay = false;
            } catch (const domain_error &) {
                okay = false;
            }
        }
    }
    cout << setw(TEST_NAME_WIDTH) << "Search after removal: " << OPF(okay) << endl;

    dictionaries[0].clear();
    REPI(i, 0, nEntries) dictionaries[0][sortedData[0][i].first] = sortedData[0][i].second;

    okay = dictionaries[0].size() == nEntries;
    cout << setw(TEST_NAME_WIDTH) << "Sorted size: " << OPF(okay) << endl;

    cout << setw(TEST_NAME_WIDTH) << "Sorted height: " << dictionaries[0].height() << endl;

    okay = true;
    REPI(i, 0, nEntries) {
        try {
            if (dictionaries[0].search(sortedData[0][i].first) != sortedData[0][i].second) okay = false;
        } catch (const domain_error &) {
            okay = false;
        }
    }
    cout << setw(TEST_NAME_WIDTH) << "Sorted search: " << OPF(okay) << endl;

    delete[] dictionaries;
    delete[] rawData;
    delete[] sortedData;

    return 0;
}
