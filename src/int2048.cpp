#include "include/int2048.h"

namespace sjtu
{
    const size_t M = 10000; // 规定每个单元模数是10000
    const unsigned DIGITAL = 4; // 规定每个单元是四位数
    const unsigned OUTPUT_M = 10; // 规定输出是十进制

    int2048::int2048() {
        // digits.push_back(0);
    }
    int2048::int2048(long long input) {
        if (input < 0) {
            input *= -1;
            is_positive = false;
        }
        while (input != 0) {
            size_t cell = input % M;
            digits.push_back(cell);
            input /= M;
        }
    }
    int2048::int2048(const std::string & input) {
        int end = 0, cnt = 0;
        if (input == "0" || input == "-0") return;
        if (input[0] == '-') {
            is_positive = false;
            end = 1;
        }
        size_t cell = 0, base = 1;
        for (int i = input.size() - 1; i >= end; i--) {
            cell += (base * (input[i] - 48));
            base *= 10;
            cnt ++;
            if (cnt == DIGITAL || i == end) {
                digits.push_back(cell);
                cnt = 0;
                cell = 0;
                base = 1;
            }
        }
    }
    int2048::int2048(const int2048 &right) {
        this->is_positive = right.is_positive;
        for (auto& cell : right.digits) {
            this->digits.push_back(cell);
        }
    }

    // 正负号
    int2048 int2048::operator+() const {
        int2048 new_instance = *this;
        return new_instance;
    }
    int2048 int2048::operator-() const {
        int2048 new_instance = *this;
        if(this->get_digits() == 0) return *this;
        new_instance.is_positive ^= 1;
        return new_instance;
    }

    // 加法（默认处理两个正数）
    int2048& int2048::operator+=(const int2048 &right) {
        // 条件判断
        if (!this->is_positive && !right.is_positive) {
            *this = -(*this);
            *this += (-right);
            *this = -(*this);
            return *this;
        }
        if (this->is_positive && !right.is_positive) {
            *this -= (-right);
            return *this;
        }
        if (!this->is_positive && right.is_positive) {
            *this = -(*this);
            *this -= right;
            *this = -(*this);
            return *this;
        }
        // 主体逻辑，处理两个正数
        size_t carry = 0;
        while (right.get_digits() > this->get_digits()) {
            this->digits.push_back(0);
        }
        int i = 0;
        while (i < right.get_digits()) {
            size_t new_val = (this->digits[i] + right.digits[i] + carry) % M;
            carry = (this->digits[i] + right.digits[i] + carry) / M;
            this->digits[i] = new_val;
            i++;
        }
        while (carry != 0 && i < this->get_digits()) {
            size_t new_val = (this->digits[i] + carry) % M;
            carry = (this->digits[i] + carry) / M;
            this->digits[i] = new_val;
            i++;
        }
        if (carry != 0) this->digits.push_back(carry);
        return *this;
    }
    int2048 operator+(int2048 left, const int2048 &right) {
        int2048 res = left;
        res += right;
        return res;
    }

    // 减法：默认处理大的正数减去小的正数
    int2048& int2048::operator-=(const int2048 &right) {
        // 条件判断
        if (!this->is_positive && right.is_positive) {
            *this = -(*this);
            *this += right;
            *this = -(*this);
            return *this;
        }
        if (this->is_positive && !right.is_positive) {
            *this += (-right);
            return *this;
        }
        if (!this->is_positive && !right.is_positive) {
            *this = -(*this);
            *this -= (-right);
            *this = -(*this);
            return *this;
        }
        if (*this < right) {
            int2048 tmp = *this;
            *this = right;
            *this -= tmp;
            *this = -(*this);
            return *this;
        }
        // 主体：大的正数减去小的正数
        size_t carry = 0, carry_this_digit = 0;
        int i = 0;
        while (i < right.get_digits()) {
            carry_this_digit = carry;
            carry = (this->digits[i] < right.digits[i] + carry);
            size_t new_val = (this->digits[i] + carry * M - carry_this_digit - right.digits[i]) % M; // 防止 unsigned 溢出
            this->digits[i] = new_val;
            i++;
        }
        while (carry != 0 && i < this->get_digits()) {
            carry_this_digit = carry;
            if (this->digits[i] < carry) carry = 1;
            size_t new_val = (this->digits[i] + carry * M - carry_this_digit) % M; // 防止 unsigned 溢出
            this->digits[i] = new_val;
            i++;
        }
        while (this->digits.back() == 0) this->digits.pop_back();
        return *this;
    }

    int2048 operator-(int2048 left, const int2048 &right) {
        int2048 res = left;
        return res -= right;
    }

    // 乘法
    int2048& int2048::operator*=(const int2048 &right) {
        // 确认符号
        bool sig = !(this->is_positive ^ right.is_positive);
        // 主要部分
        int2048 res("0");
        for (int i=0; i<this->get_digits(); i++) {
            int2048 tmp("0");
            for (int j=0; j<right.get_digits(); j++) {
                int2048 mul(this->digits[i] * right.digits[j]);
                tmp += lshift(mul, j);
            }
            int2048 tmp_shifted = lshift(tmp, i);
            res += tmp_shifted;
        }
        res.is_positive = sig;
        *this = res;
        return *this;
    }
    int2048 operator*(int2048 left, const int2048 &right) {
        int2048 res = left;
        return res *= right;
    }
    // 除法
    int2048& int2048::operator/=(const int2048 &right) {
        // 确认符号
        bool sig = !(this->is_positive ^ right.is_positive);
        // 主要部分
        int2048 res("0");
        int2048 remaining = *this;
        remaining.is_positive = true;
        bool filled = false; // 第一次找到大于0的商时确定最高位，将res填充相应的零
        int k = this->get_digits() - 1; // 被除数参与长除法的最低位
        while (k >= 0) {
            int2048 divided = rshift(remaining, k);
            while (divided < right && k >= 0) {
                k--;
                divided = rshift(remaining, k);   
            }
            if (k < 0) break;
            if (!filled) {
                filled = true;
                for (int i=0; i<=k; i++) {
                    res.digits.push_back(0);
                }
            }
            // 试商 (Naive)
            for (size_t j = M; j > 0; j--) {
                int2048 d(j);
                if (d * right <= divided) {
                    res.digits[k] = j;
                    remaining -= lshift((d * right), k);
                    break;
                }
            }
        }
        res.is_positive = sig;
        if (remaining.get_digits() != 0 && !sig) res -= int2048(1); // 如果负数没除完，向下取整-1
        *this = res;
        return *this;
    }
    int2048 operator/(int2048 left, const int2048 &right) {
        int2048 res = left;
        return res /= right;
    }

    // 赋值等号
    int2048& int2048::operator=(const int2048 &right) {
        this->is_positive = right.is_positive;
        this->digits.clear();
        for (auto& cell : right.digits) {
            this->digits.push_back(cell);
        }
        return *this;
    }
    // 输入
    std::istream &operator>>(std::istream &is, int2048 &instance) {
        std::string input;
        is >> input;
        instance = int2048(input);
        return is;
    }

    // 输出
    std::ostream &operator<<(std::ostream &os, const int2048 &instance) {
        if (!instance.is_positive) os << '-';
        if (instance.digits.empty()) {
            os << "0";
        }
        for (int i = instance.digits.size() - 1; i >= 0; i--) {
            int cnt = 0;
            size_t cell = instance.digits[i];
            size_t cell_copy = cell;
            while (cell_copy != 0) {
                cell_copy /= OUTPUT_M;
                cnt ++;
            }

            // 输出正负
            // 除最高位外输出前导0
            if (i != instance.digits.size() - 1) {
                for (int j = DIGITAL; j > cnt; j--) {
                    os << '0';
                }
            }
            // 输出数字，如果非零
            if (cell != 0) os << cell;
        }
        return os;
    }

    // 关系比较
    bool operator==(const int2048 &left, const int2048 &right) {
        if (left.is_positive != right.is_positive || left.get_digits() != right.get_digits()) return false;
        for (int i = 0; i < left.get_digits(); i ++) {
            if (left.digits[i] != right.digits[i]) return false;
        }
        return true;
    }
    bool operator!=(const int2048 &left, const int2048 &right) {
        return !(left == right);
    }
    bool operator<(const int2048 &left, const int2048 &right) {
        // 小于情况默认比较两个正数
        if (left.is_positive && !right.is_positive) return false;
        if (!left.is_positive && right.is_positive) return true;
        if (!left.is_positive && !right.is_positive) return (-right) < (-left);
        if (left.get_digits() < right.get_digits()) return true;
        if (left.get_digits() > right.get_digits()) return false;
        for (int i = left.get_digits()-1; i >= 0; i --) {
            if (left.digits[i] < right.digits[i]) return true;
            if (left.digits[i] > right.digits[i]) return false;
        }
        return false;
    }
    bool operator>(const int2048 &left, const int2048 &right) {
        return right < left;
    }
    bool operator<=(const int2048 &left, const int2048 &right) {
        return left < right || left == right;
    }
    bool operator>=(const int2048 &left, const int2048 &right) {
        return left > right || left == right;
    }
} // namespace sjtu
