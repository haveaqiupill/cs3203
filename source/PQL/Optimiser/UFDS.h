#include <unordered_set>
#include <vector>
#include <string>

class UFDS {
 public:
    explicit UFDS(int n);
    int findSet(int i);
    bool isSameSet(int i, int j);
    void unionSet(int i, int j);
    int getSetSize(int i);
    int getNumDisjointSets();

 private:
    std::vector<int> p;
    std::vector<int> rank;
    std::vector<int> sizeList;
    int numSets;
};
