#pragma once
#include <iostream>

using Lehmer32_t = uint32_t;

/*
 * Random number Generator based on Lehmer algorithm
 */
class RandomNumber {

private:
    Lehmer32_t nLehmer;

public:

    RandomNumber(Lehmer32_t seed) {
        nLehmer = seed;
    }

    RandomNumber() {
        nLehmer = rand();
    }

    /*
     * Algorithm implementation 
     */
    Lehmer32_t generate() {
        nLehmer += 0xe120fc15;
        uint64_t tmp = (uint64_t)nLehmer * 0x4a39b70d;
        Lehmer32_t m1 = (tmp >> 32) ^ tmp;
        tmp = (uint64_t)m1 * 0x12fad5c9;
        Lehmer32_t m2 = (tmp >> 32) ^ tmp;
        return m2;
    }

    /*
     * Allows you to set the seed for the algorithm (the Lehmer number) 
     */
    void setSeed(Lehmer32_t seed) {
        this->nLehmer = seed;
    }

    /*
     * Random number as integer 
     * Args:
     *  min: int
     *  max: int
     */
    int rndInt(int min, int max) {
        return (generate() % ((max+1) - min)) + min;
    }

    /*
     * Random number as double 
     * Args:
     *  min: double
     *  max: double
     */
    double rndDouble(double min, double max) {
        return ((double)generate() / (double)UINT32_MAX) * (max - min) + min;
    }
};