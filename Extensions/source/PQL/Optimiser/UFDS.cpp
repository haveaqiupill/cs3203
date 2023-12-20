#include "UFDS.h"

UFDS::UFDS(int n) {
    numSets = n;
    rank.assign(n, 0);  // all rank 0 at start
    p.assign(n, 0);     // all belong to set 0 at the start
    for (int i = 0; i < n; i++) {
        p[i] = i;
    }
    sizeList.assign(n, 1);
}

int UFDS::findSet(int i) {
    if (p[i] == i) {
        return i;
    }
    p[i] = findSet(p[i]);
    return p[i];
}

bool UFDS::isSameSet(int i, int j) {
    return findSet(i) == findSet(j);
}

void UFDS::unionSet(int i, int j) {
    if (!isSameSet(i, j)) {
        int x = findSet(i);
        int y = findSet(j);
        if (rank[x] > rank[y]) {
            sizeList[findSet(x)] += sizeList[findSet(y)];
            p[y] = x;
        } else {
            sizeList[findSet(y)] += sizeList[findSet(x)];
            p[x] = y;
            if (rank[x] == rank[y]) {
                rank[y]++;
            }
        }
        numSets--;
    }
}

int UFDS::getSetSize(int i) {
    return sizeList[findSet(i)];
}

int UFDS::getNumDisjointSets() {
    return numSets;
}
