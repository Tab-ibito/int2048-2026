#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

// Integer 1:
// 实现一个有符号的大整数类，只需支持简单的加减

// Integer 2:
// 实现一个有符号的大整数类，支持加减乘除，并重载相关运算符

// 请不要使用除了以下头文件之外的其它头文件
#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

// 请不要使用 using namespace std;

namespace sjtu
{
    class int2048
    {
        // todo
        bool is_positive = true;
        std::vector<size_t> digits;

        void add_abs(const int2048 &right);
        void subtract_abs(const int2048 &right);
    public:
        // 构造函数
        int2048();
        int2048(long long);
        int2048(const std::string &);
        int2048(const int2048 &);

        // 得到 digits 大小
        int get_digits() const {
            return this->digits.size();
        }
        // 左移运算，返回一个新的数字，这个过程是O(n)的
        int2048 lshift(const int2048& a, int offset) {
            int2048 shifted;
            shifted.is_positive = a.is_positive;
            shifted.digits.resize(offset + a.get_digits(), 0);
            for (int i=0; i<a.get_digits(); i++) {
                shifted.digits[offset+i] = a.digits[i];
            }
            return shifted;
        }
        // 右移运算
        int2048 rshift(const int2048& a, int offset) {
            int2048 shifted;
            shifted.is_positive = a.is_positive;
            for (int i=offset; i<a.get_digits(); i++) {
                shifted.digits.push_back(a.digits[i]);
            }
            return shifted;
        }

        // 以下给定函数的形式参数类型仅供参考，可自行选择使用常量引用或者不使用引用
        // 如果需要，可以自行增加其他所需的函数
        // ===================================
        // Integer1
        // ===================================

        // 读入一个大整数
        void read(const std::string &input) {
            *this = int2048(input);
        }
        // 输出储存的大整数，无需换行
        void print() {
            std::cout << *this;
        }

        // 加上一个大整数
        int2048 &add(const int2048 right) {
            *this += right;
            return *this;
        }
        // 返回两个大整数之和
        friend int2048 add(int2048 left, const int2048 right) {
            return left + right;
        }

        // 减去一个大整数
        int2048 &minus(const int2048 right) {
            *this -= right;
            return *this;
        }
        // 返回两个大整数之差
        friend int2048 minus(int2048 left, const int2048 right) {
            return left - right;
        }

        // ===================================
        // Integer2
        // ===================================

        int2048 operator+() const;
        int2048 operator-() const;

        int2048 &operator=(const int2048 &);

        int2048 &operator+=(const int2048 &);
        friend int2048 operator+(int2048, const int2048 &);

        int2048 &operator-=(const int2048 &);
        friend int2048 operator-(int2048, const int2048 &);

        int2048 &operator*=(const int2048 &);
        friend int2048 operator*(int2048, const int2048 &);

        int2048 &operator/=(const int2048 &);
        friend int2048 operator/(int2048, const int2048 &);

        int2048 &operator%=(const int2048 &);
        friend int2048 operator%(int2048, const int2048 &);

        friend std::istream &operator>>(std::istream &, int2048 &);
        friend std::ostream &operator<<(std::ostream &, const int2048 &);

        friend bool operator==(const int2048 &, const int2048 &);
        friend bool operator!=(const int2048 &, const int2048 &);
        friend bool operator<(const int2048 &, const int2048 &);
        friend bool operator>(const int2048 &, const int2048 &);
        friend bool operator<=(const int2048 &, const int2048 &);
        friend bool operator>=(const int2048 &, const int2048 &);
    };
} // namespace sjtu

#endif
