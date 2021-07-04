#include <gtest/gtest.h>

#include <iostream>
#include "Snap.h"
using namespace std;

template<class TVal>
bool TestTVecEqual(const TVec<TVal>& V1, const TVec<TVal>& V2);


TEST(TVecTest, TIntVMergeV) {

// Merge with unsorted normal TIntV
    TIntV V1; V1.Add(1); V1.Add(2); V1.Add(3); V1.Add(5);
    TIntV V2; V2.Add(7); V2.Add(4); V2.Add(5); V2.Add(2);
    TIntV V3; V3.Add(1); V3.Add(2); V3.Add(3); V3.Add(4); V3.Add(5); V3.Add(7);
    V1.AddVMerged(V2);
    EXPECT_TRUE(TestTVecEqual<TInt>(V1, V3));

    // Merge with sorted normal TIntV
    V1.Clr(); V1.Add(1); V1.Add(2); V1.Add(3); V1.Add(5);
    V2.Clr(); V2.Add(0); V2.Add(2); V2.Add(4); V2.Add(6);
    V3.Clr(); V3.Add(0); V3.Add(1); V3.Add(2); V3.Add(3); V3.Add(4); V3.Add(5); V3.Add(6);
    V1.AddVMerged(V2);
    EXPECT_TRUE(TestTVecEqual<TInt>(V1, V3));

    // Merge with more interleavings
    V1.Clr(); V1.Add(1); V1.Add(3); V1.Add(5); V1.Add(9); V1.Add(10);
    V2.Clr(); V2.Add(13); V2.Add(11); V2.Add(12); V2.Add(0); V2.Add(8); V2.Add(6); V2.Add(4); V2.Add(7); V2.Add(2);
    V3.Clr();
    for (int i = 0; i < 14; i++) {
        V3.Add(i);
    }
    V1.AddVMerged(V2);
    EXPECT_TRUE(TestTVecEqual<TInt>(V1, V3));


    // Merge with more interleavings and without overlapping
    V1.Clr(); V1.Add(1); V1.Add(3); V1.Add(5); V1.Add(9); V1.Add(10);
    V2.Clr(); V2.Add(13); V2.Add(11); V2.Add(12);  V2.Add(0); V2.Add(8); V2.Add(6); V2.Add(4); V2.Add(7); V2.Add(2);
    V2.Add(12);  V2.Add(0); V2.Add(8); V2.Add(6);
    V3.Clr();
    for (int i = 0; i < 14; i++) {
    V3.Add(i);
    }
    V1.AddVMerged(V2);
    EXPECT_TRUE(TestTVecEqual<TInt>(V1, V3));

    // Merge empty TIntV
    V1.Clr(); V1.Add(1); V1.Add(2); V1.Add(3);
    V2.Clr();
    V3 = V1;
    V1.AddVMerged(V2);
    EXPECT_TRUE(TestTVecEqual<TInt>(V1, V3));


    // Empty TIntV merges with another normal TIntV
    V2.AddVMerged(V1);
    EXPECT_TRUE(TestTVecEqual<TInt>(V2, V3));

    // Merge the same TIntV
    V2.AddVMerged(V3);
    EXPECT_TRUE(TestTVecEqual<TInt>(V2, V3));

    // Merge two empty TIntV
    V1.Clr();
    V2.Clr();
    V3.Clr();
    V1.AddVMerged(V3);
    EXPECT_TRUE(TestTVecEqual<TInt>(V2, V3));
}

template<class TVal>
bool TestTVecEqual(const TVec<TVal>& V1, const TVec<TVal>& V2){
    bool Result = true;
    EXPECT_EQ(V1.Len(), V2.Len());
    Result &= V1.Len() == V2.Len();
    for (TInt ValN = 0; Result && ValN < V1.Len(); ValN++) {
        EXPECT_EQ(V1[ValN], V2[ValN]);
        Result = Result && (V1[ValN] == V2[ValN]);
    }
    return Result;
}

#include <chrono>
void TestNewMethodEfficiency(){
    const int Len1 = 10240;
    const int Len2 = 10240;
    const int Round = 1024;
    TIntV V1;
    TIntV V2;
    double old_method_seconds = 0.0;
    double new_method_seconds = 0.0;
    for (int j = 0; j < Round; ++j) {
        std::cout << "* " << j << endl;
        V1.Clr(); V2.Clr();
        for (int k = 0; k < Len1; ++k) {
            V1.Add(rand() % Len1);
        }
        for (int k = 0; k < Len2; ++k) {
            V2.Add(rand() % Len1);
        }
        V1.Sort();
        TIntV V3 = V1;
        auto start = chrono::system_clock::now();
        V1.AddVMerged(V2);
        auto end = chrono::system_clock::now();
        chrono::duration<double> seconds = end - start;
        new_method_seconds += seconds.count();

        start = chrono::system_clock::now();
        for (TInt ValN=0; ValN<V2.Len(); ValN++){V3.AddMerged(V2[ValN]);}
        end = chrono::system_clock::now();
        seconds = end - start;
        old_method_seconds += seconds.count();

        TestTVecEqual<TInt>(V1, V3);
    }
    std::cout << "new method takes " << new_method_seconds << " seconds." << std::endl;
    std::cout << "old method takes " << old_method_seconds << " seconds." << std::endl;
    std::cout << "improvement " << old_method_seconds/new_method_seconds << "." << std::endl;
}