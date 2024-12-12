#ifndef TREAP_DICTIONARY_H
#define TREAP_DICTIONARY_H

#include <cstdint>
#include <stdexcept>
#include <random>
#include <algorithm>

static const uint32_t NULL_INDEX = 0xffffffff;
static const uint32_t DEFAULT_INITIAL_CAPACITY = 16;

template <typename KeyType, typename ValueType>
class TreapDictionary {
public:
    explicit TreapDictionary(uint32_t _cap = DEFAULT_INITIAL_CAPACITY);
    ~TreapDictionary();

    void clear();
    uint32_t size();
    uint32_t height();
    bool isEmpty();

    ValueType &search(const KeyType &k);
    ValueType &operator[](const KeyType &k);
    void remove(const KeyType &k);

private:
    uint32_t prvAllocate();
    void prvFree(uint32_t n);
    void prvClear(uint32_t r);
    void prvAdjust(uint32_t r);
    uint32_t prvRotateLeft(uint32_t r);
    uint32_t prvRotateRight(uint32_t r);
    uint32_t prvInsert(uint32_t r, uint32_t &n, const KeyType &k);
    uint32_t prvRemove(uint32_t r, uint32_t &ntbd, const KeyType &k);

    uint32_t root;

    static uint32_t *counts, *heights, *left, *right, *heapVals;
    static uint32_t nTreaps, capacity, freeListHead;
    static std::random_device *rd;
    static std::mt19937 *mt;
    static KeyType *keys;
    static ValueType *values;
};

// Static member initialization
template <typename KeyType, typename ValueType>
uint32_t *TreapDictionary<KeyType, ValueType>::counts = nullptr;

template <typename KeyType, typename ValueType>
uint32_t *TreapDictionary<KeyType, ValueType>::heights = nullptr;

template <typename KeyType, typename ValueType>
uint32_t *TreapDictionary<KeyType, ValueType>::left = nullptr;

template <typename KeyType, typename ValueType>
uint32_t *TreapDictionary<KeyType, ValueType>::right = nullptr;

template <typename KeyType, typename ValueType>
uint32_t *TreapDictionary<KeyType, ValueType>::heapVals = nullptr;

template <typename KeyType, typename ValueType>
uint32_t TreapDictionary<KeyType, ValueType>::nTreaps = 0;

template <typename KeyType, typename ValueType>
uint32_t TreapDictionary<KeyType, ValueType>::capacity = 0;

template <typename KeyType, typename ValueType>
uint32_t TreapDictionary<KeyType, ValueType>::freeListHead = NULL_INDEX;

template <typename KeyType, typename ValueType>
std::random_device *TreapDictionary<KeyType, ValueType>::rd = nullptr;

template <typename KeyType, typename ValueType>
std::mt19937 *TreapDictionary<KeyType, ValueType>::mt = nullptr;

template <typename KeyType, typename ValueType>
KeyType *TreapDictionary<KeyType, ValueType>::keys = nullptr;

template <typename KeyType, typename ValueType>
ValueType *TreapDictionary<KeyType, ValueType>::values = nullptr;

// Constructor
template <typename KeyType, typename ValueType>
TreapDictionary<KeyType, ValueType>::TreapDictionary(uint32_t _cap) : root(NULL_INDEX) {
    if (nTreaps == 0) {
        capacity = _cap;
        counts = new uint32_t[capacity]{};
        heights = new uint32_t[capacity]{};
        left = new uint32_t[capacity]{};
        right = new uint32_t[capacity]{};
        heapVals = new uint32_t[capacity]{};
        keys = new KeyType[capacity];
        values = new ValueType[capacity];
        freeListHead = 0;

        for (uint32_t i = 0; i < capacity - 1; i++) {
            left[i] = i + 1;
        }
        left[capacity - 1] = NULL_INDEX;

        rd = new std::random_device();
        mt = new std::mt19937((*rd)());
    }
    ++nTreaps;
}

// Destructor
template <typename KeyType, typename ValueType>
TreapDictionary<KeyType, ValueType>::~TreapDictionary() {
    if (--nTreaps == 0) {
        delete[] counts;
        delete[] heights;
        delete[] left;
        delete[] right;
        delete[] heapVals;
        delete[] keys;
        delete[] values;
        delete rd;
        delete mt;
    }
}

// Clear
template <typename KeyType, typename ValueType>
void TreapDictionary<KeyType, ValueType>::clear() {
    prvClear(root);
    root = NULL_INDEX;
}

// Check if empty
template <typename KeyType, typename ValueType>
bool TreapDictionary<KeyType, ValueType>::isEmpty() {
    return root == NULL_INDEX;
}

// Size
template <typename KeyType, typename ValueType>
uint32_t TreapDictionary<KeyType, ValueType>::size() {
    return root == NULL_INDEX ? 0 : counts[root];
}

// Height
template <typename KeyType, typename ValueType>
uint32_t TreapDictionary<KeyType, ValueType>::height() {
    return root == NULL_INDEX ? -1 : heights[root];
}

// Search
template <typename KeyType, typename ValueType>
ValueType &TreapDictionary<KeyType, ValueType>::search(const KeyType &k) {
    uint32_t n = root;
    while (n != NULL_INDEX) {
        if (keys[n] == k) return values[n];
        n = (k < keys[n]) ? left[n] : right[n];
    }
    throw std::domain_error("Key not found");
}

// Operator []
template <typename KeyType, typename ValueType>
ValueType &TreapDictionary<KeyType, ValueType>::operator[](const KeyType &k) {
    uint32_t tmp = prvAllocate();
    root = prvInsert(root, tmp, k);
    return values[tmp];
}

// Remove
template <typename KeyType, typename ValueType>
void TreapDictionary<KeyType, ValueType>::remove(const KeyType &k) {
    uint32_t ntbd = NULL_INDEX;
    root = prvRemove(root, ntbd, k);
    if (ntbd != NULL_INDEX)
        prvFree(ntbd);
}

// Clear the subtree rooted at `r`
template <typename KeyType, typename ValueType>
void TreapDictionary<KeyType, ValueType>::prvClear(uint32_t r) {
    if (r != NULL_INDEX) {
        prvClear(left[r]);
        prvClear(right[r]);
        prvFree(r);
    }
}

// Free a node
template <typename KeyType, typename ValueType>
void TreapDictionary<KeyType, ValueType>::prvFree(uint32_t n) {
    left[n] = freeListHead;
    freeListHead = n;
}

// Adjust node values
template <typename KeyType, typename ValueType>
void TreapDictionary<KeyType, ValueType>::prvAdjust(uint32_t r) {
    counts[r] = 1 + (left[r] != NULL_INDEX ? counts[left[r]] : 0) + (right[r] != NULL_INDEX ? counts[right[r]] : 0);
    heights[r] = 1 + std::max(left[r] != NULL_INDEX ? heights[left[r]] : 0, right[r] != NULL_INDEX ? heights[right[r]] : 0);
}

// Rotate left
template <typename KeyType, typename ValueType>
uint32_t TreapDictionary<KeyType, ValueType>::prvRotateLeft(uint32_t r) {
    uint32_t newRoot = right[r];
    right[r] = left[newRoot];
    left[newRoot] = r;
    prvAdjust(r);
    prvAdjust(newRoot);
    return newRoot;
}

// Rotate right
template <typename KeyType, typename ValueType>
uint32_t TreapDictionary<KeyType, ValueType>::prvRotateRight(uint32_t r) {
    uint32_t newRoot = left[r];
    left[r] = right[newRoot];
    right[newRoot] = r;
    prvAdjust(r);
    prvAdjust(newRoot);
    return newRoot;
}

// Insert
template <typename KeyType, typename ValueType>
uint32_t TreapDictionary<KeyType, ValueType>::prvInsert(uint32_t r, uint32_t &n, const KeyType &k) {
    if (r == NULL_INDEX) {
        keys[n] = k;
        return n;
    }
    if (k < keys[r]) {
        left[r] = prvInsert(left[r], n, k);
        if (heapVals[left[r]] < heapVals[r]) r = prvRotateRight(r);
    } else if (k > keys[r]) {
        right[r] = prvInsert(right[r], n, k);
        if (heapVals[right[r]] < heapVals[r]) r = prvRotateLeft(r);
    }
    prvAdjust(r);
    return r;
}

// Remove
template <typename KeyType, typename ValueType>
uint32_t TreapDictionary<KeyType, ValueType>::prvRemove(uint32_t r, uint32_t &ntbd, const KeyType &k) {
    if (r == NULL_INDEX) return NULL_INDEX;

    if (k < keys[r]) {
        left[r] = prvRemove(left[r], ntbd, k);
    } else if (k > keys[r]) {
        right[r] = prvRemove(right[r], ntbd, k);
    } else {
        ntbd = r;
        if (left[r] == NULL_INDEX) return right[r];
        if (right[r] == NULL_INDEX) return left[r];

        if (heapVals[left[r]] < heapVals[right[r]]) {
            r = prvRotateRight(r);
            right[r] = prvRemove(right[r], ntbd, k);
        } else {
            r = prvRotateLeft(r);
            left[r] = prvRemove(left[r], ntbd, k);
        }
    }
    prvAdjust(r);
    return r;
}

// Allocate a node
template <typename KeyType, typename ValueType>
uint32_t TreapDictionary<KeyType, ValueType>::prvAllocate() {
    if (freeListHead == NULL_INDEX) {
        uint32_t newCapacity = capacity * 2;
        auto *newCounts = new uint32_t[newCapacity];
        auto *newHeights = new uint32_t[newCapacity];
        auto *newLeft = new uint32_t[newCapacity];
        auto *newRight = new uint32_t[newCapacity];
        auto *newHeapVals = new uint32_t[newCapacity];
        auto *newKeys = new KeyType[newCapacity];
        auto *newValues = new ValueType[newCapacity];

        for (uint32_t i = 0; i < capacity; i++) {
            newCounts[i] = counts[i];
            newHeights[i] = heights[i];
            newLeft[i] = left[i];
            newRight[i] = right[i];
            newHeapVals[i] = heapVals[i];
            newKeys[i] = keys[i];
            newValues[i] = values[i];
        }

        delete[] counts;
        delete[] heights;
        delete[] left;
        delete[] right;
        delete[] heapVals;
        delete[] keys;
        delete[] values;

        counts = newCounts;
        heights = newHeights;
        left = newLeft;
        right = newRight;
        heapVals = newHeapVals;
        keys = newKeys;
        values = newValues;

        freeListHead = capacity;
        for (uint32_t i = capacity; i < newCapacity - 1; i++) {
            left[i] = i + 1;
        }
        left[newCapacity - 1] = NULL_INDEX;

        capacity = newCapacity;
    }

    uint32_t node = freeListHead;
    freeListHead = left[node];
    left[node] = NULL_INDEX;
    right[node] = NULL_INDEX;
    counts[node] = 1;
    heights[node] = 1;
    heapVals[node] = (*mt)();
    return node;
}
#endif
