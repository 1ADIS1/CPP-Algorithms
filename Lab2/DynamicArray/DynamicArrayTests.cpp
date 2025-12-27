#include "DynamicArray.cpp" 
#include <gtest/gtest.h>
#include <vector>
#include <string>

class Player {
public:
    int health;
    std::vector<std::string> inventory;

    Player(int h = 100) : health(h) {}
    Player(int h, const std::vector<std::string>& inv) : health(h), inventory(inv) {}
    Player(const Player& other) = default;
    Player(Player&& other) noexcept = default;
    Player& operator=(const Player& other) = default;
    Player& operator=(Player&& other) noexcept = default;
    ~Player() = default;

    bool operator==(const Player& o) const {
        return health == o.health && inventory == o.inventory;
    }
};

TEST(ArrayBasic, InsertAndIndex) {
    Array<int> a;
    for (int i = 0; i < 10; ++i) a.insert(i + 1);
    ASSERT_EQ(a.size(), 10);
    for (int i = 0; i < a.size(); ++i) a[i] *= 2;
    for (int i = 0; i < 10; ++i) EXPECT_EQ(a[i], (i + 1) * 2);
}

TEST(ArrayIterator, ForwardAndReverse) {
    Array<int> a;
    for (int i = 0; i < 5; ++i) a.insert(i + 1);
    int expected = 1;
    for (auto it = a.iterator(); it.hasNext(); it.next()) {
        EXPECT_EQ(it.get(), expected);
        ++expected;
    }
    expected = 5;
    for (auto it = a.reverseIterator(); it.hasNext(); it.next()) {
        EXPECT_EQ(it.get(), expected);
        --expected;
    }
}

TEST(ArrayInsertRemove, AtIndex) {
    Array<int> a;
    a.insert(0, 1);
    a.insert(1, 3);
    a.insert(1, 2); // 1,2,3
    ASSERT_EQ(a.size(), 3);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 2);
    EXPECT_EQ(a[2], 3);
    a.remove(1); // 1,3
    ASSERT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 3);
}

TEST(ArrayCopyMove, CopyAndMove) {
    Array<std::string> a;
    a.insert(std::string("one"));
    a.insert(std::string("two"));
    Array<std::string> b = a; // copy
    EXPECT_EQ(b.size(), a.size());
    EXPECT_EQ(b[0], "one");
    Array<std::string> c = std::move(a); // move
    EXPECT_EQ(c.size(), 2);
}

TEST(ArrayOperators, AssignmentAndSelfAssign) {
    Array<int> a;
    for (int i = 0; i < 5; ++i) a.insert(i + 1);
    Array<int> b;
    b = a; 
    EXPECT_EQ(b.size(), a.size());
    for (int i = 0; i < 5; ++i) EXPECT_EQ(b[i], i + 1);

    b = b;
    EXPECT_EQ(b.size(), 5);
    for (int i = 0; i < 5; ++i) EXPECT_EQ(b[i], i + 1);
}

TEST(ArrayMove, MoveLeavesSourceEmpty) {
    Array<int> a;
    for (int i = 0; i < 3; ++i) a.insert((i + 1) * 10);
    Array<int> b = std::move(a);
    EXPECT_EQ(b.size(), 3);
    // moved-from 'a' should be in a valid empty state
    EXPECT_EQ(a.size(), 0);
}

TEST(InsertReturnIndex, EndAndAtIndex) {
    Array<int> a;
    int idx1 = a.insert(42);
    EXPECT_EQ(idx1, 0);
    int idx2 = a.insert(0, 7);
    EXPECT_EQ(idx2, 0); // inserted at 0
    EXPECT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], 7);
    EXPECT_EQ(a[1], 42);
}

TEST(ArrayIteratorSet, IteratorSetAndGet) {
    Array<int> a;
    for (int i = 0; i < 4; ++i) a.insert(i);
    for (auto it = a.iterator(); it.hasNext(); it.next()) {
        int val = it.get();
        it.set(val + 100);
    }
    for (int i = 0; i < 4; ++i) EXPECT_EQ(a[i], i + 100);
}

TEST(ArrayConstIterator, ConstIteratorTraversal) {
    Array<int> a;
    for (int i = 0; i < 3; ++i) a.insert(i + 1);
    int sum = 0;
    for (auto it = static_cast<const Array<int>&>(a).iterator(); it.hasNext(); it.next()) {
        sum += it.get();
    }
    EXPECT_EQ(sum, 1 + 2 + 3);
}

TEST(ArrayNonMovable, WorksWithNonMovableType) {
    struct NoMove {
        int v;
        NoMove(int x = 0) : v(x) {}
        NoMove(const NoMove&) = default;
        NoMove& operator=(const NoMove&) = default;
        NoMove(NoMove&&) = delete;
        NoMove& operator=(NoMove&&) = delete;
        bool operator==(const NoMove& o) const { return v == o.v; }
    };

    Array<NoMove> a;
    for (int i = 0; i < 6; ++i) a.insert(NoMove(i * 2)); // should copy, not move
    EXPECT_EQ(a.size(), 6);
    for (int i = 0; i < 6; ++i) EXPECT_EQ(a[i].v, i * 2);
    a.remove(2);
    EXPECT_EQ(a.size(), 5);
}

TEST(ArrayStress, GrowManyElements) {
    Array<int> a;
    const int N = 1000;
    for (int i = 0; i < N; ++i) a.insert(i);
    EXPECT_EQ(a.size(), N);
    for (int i = 0; i < N; ++i) EXPECT_EQ(a[i], i);
    // remove a bunch
    for (int i = 0; i < 500; ++i) a.remove(0);
    EXPECT_EQ(a.size(), 500);
    for (int i = 0; i < 500; ++i) EXPECT_EQ(a[i], i + 500);
}

TEST(SimpleTypes, IntAndString) {
    Array<int> ai;
    ai.insert(10);
    ai.insert(20);
    EXPECT_EQ(ai.size(), 2);
    EXPECT_EQ(ai[0], 10);
    EXPECT_EQ(ai[1], 20);

    Array<std::string> as;
    as.insert(std::string("hello"));
    as.insert(std::string("world"));
    EXPECT_EQ(as.size(), 2);
    EXPECT_EQ(as[0], "hello");
    EXPECT_EQ(as[1], "world");
}

TEST(ComplexTypes, PlayerHealthAndInventory) {
    {
        Array<Player> players;
        players.insert(Player(80, { "sword", "shield" }));
        players.insert(Player(60, { "potion" }));
        players.insert(Player(90, { "bow", "arrow" }));

        EXPECT_EQ(players.size(), 3);
        EXPECT_EQ(players[0].health, 80);
        EXPECT_EQ(players[0].inventory.size(), 2);
        EXPECT_EQ(players[0].inventory[0], "sword");
        EXPECT_EQ(players[1].health, 60);
        EXPECT_EQ(players[1].inventory[0], "potion");

        // remove middle player
        players.remove(1);
        EXPECT_EQ(players.size(), 2);
        EXPECT_EQ(players[0].health, 80);
        EXPECT_EQ(players[1].health, 90);
        EXPECT_EQ(players[1].inventory[0], "bow");
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
