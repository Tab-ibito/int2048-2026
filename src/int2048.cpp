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
        is_positive = input >= 0;
        unsigned long long value =
            static_cast<unsigned long long>(input);
        if (input < 0) value = 0ULL - value;
        while (value != 0) {
            digits.push_back(value % M);
            value /= M;
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

    // 只处理两个正数相加
    void int2048::add_abs(const int2048 &right) {
        size_t carry = 0;
        const int right_size = right.get_digits();
        if (right_size > this->get_digits()) {
            this->digits.resize(right_size, 0);
        }
        int i = 0;
        while (i < right_size) {
            size_t sum = this->digits[i] + right.digits[i] + carry;
            this->digits[i] = sum % M;
            carry = sum / M;
            i++;
        }
        while (carry != 0 && i < this->get_digits()) {
            size_t new_val = (this->digits[i] + carry) % M;
            carry = (this->digits[i] + carry) / M;
            this->digits[i] = new_val;
            i++;
        }
        if (carry != 0) this->digits.push_back(carry);
        if (this->digits.empty()) this->is_positive = true;
    }

    // 统一处理 +=
    int2048& int2048::operator+=(const int2048 &right) {
        if (this->is_positive == right.is_positive) {
            add_abs(right);
        } else {
            subtract_abs(right);
        }
        return *this;
    }
    int2048 operator+(int2048 left, const int2048 &right) {
        left += right;
        return left;
    }

    // 只处理两个正数相减
    void int2048::subtract_abs(const int2048 &right) {
        int left_size = this->get_digits(), right_size = right.get_digits();
        bool reverse = left_size < right_size;
        if (left_size == right_size) {
            for (int i = left_size - 1; i >= 0; i--) {
                if (this->digits[i] != right.digits[i]) {
                    reverse = this->digits[i] < right.digits[i];
                    break;
                }
            }
        }
        if (right_size > left_size) {
            this->digits.resize(right_size, 0);
        }
        size_t borrow = 0;
        for (int i = 0; i < right_size || borrow != 0; i++) {
            size_t left_digit = this->digits[i];
            size_t right_digit = i < right_size ? right.digits[i] : 0;
            size_t minu_end = reverse ? right_digit : left_digit;
            size_t subtra_hend = (reverse ? left_digit : right_digit) + borrow;
            borrow = minu_end < subtra_hend;
            this->digits[i] = minu_end + borrow * M - subtra_hend;
        }
        while (!this->digits.empty() && this->digits.back() == 0) this->digits.pop_back();
        if (reverse) this->is_positive = !this->is_positive;
        if (this->digits.empty()) this->is_positive = true;
    }

    // 统一处理 -= 情况
    int2048& int2048::operator-=(const int2048 &right) {
        if (this->is_positive != right.is_positive) {
            add_abs(right);
        } else {
            subtract_abs(right);
        }
        return *this;
    }

    int2048 operator-(int2048 left, const int2048 &right) {
        left -= right;
        return left;
    }

    // 乘法
    int2048& int2048::operator*=(const int2048 &right) {
        // 确认符号
        bool sig = !(this->is_positive ^ right.is_positive);
        if (this->digits.empty() || right.digits.empty()){
            *this = int2048(0);
            return *this;
        }
        // 优化：希望小数 * 大数
        if (this->get_digits() > right.get_digits()) {
            int2048 res = right * *this;
            *this = res;
            return *this;
        }
        // 主要部分
        int2048 res;
        if (this->get_digits() == 1) {
            // 特殊情况
            size_t a = this->digits[0];
            size_t carry = 0;
            res.digits.resize(right.get_digits());
            for (int j=0; j<right.get_digits(); j++) {
                size_t new_val = (a * right.digits[j] + carry) % M;
                carry = (a * right.digits[j] + carry) / M;
                res.digits[j] = new_val;
            }
            if (carry != 0) res.digits.push_back(carry);    
        } else {
            // 一般情况
            for (int i=0; i<this->get_digits(); i++) {
                int2048 tmp;
                size_t carry = 0;
                for (int j=0; j<right.get_digits(); j++) {
                    size_t new_val = (this->digits[i] * right.digits[j] + carry) % M;
                    carry = (this->digits[i] * right.digits[j] + carry) / M;
                    tmp.digits.push_back(new_val);
                }
                if (carry != 0) tmp.digits.push_back(carry);
                int2048 tmp_shifted = lshift(tmp, i);
                res += tmp_shifted;
            }
        }
        res.is_positive = sig;
        *this = res;
        return *this;
    }
    int2048 operator*(int2048 left, const int2048 &right) {
        left *= right;
        return left;
    }
    // 除法
    int2048& int2048::operator/=(const int2048 &right) {
        // 确认符号
        bool sig = !(this->is_positive ^ right.is_positive);
        if (this->digits.empty() || right.digits.empty()){
            *this = int2048(0);
            return *this;
        }
        // 主要部分
        int2048 res;
        int2048 abs_right = right;
        abs_right.is_positive = true;
        bool filled = false; // 第一次找到大于0的商时确定最高位，将res填充相应的零
        int k = this->get_digits() - 1; // 被除数参与长除法的最低位
        if (right.get_digits() == 1) {
            // 特殊情况：处理除数只有一位的情况
            size_t d = right.digits[0], remaining = this->digits[k];
            while (k >= 0) {
                size_t divided = remaining;
                if (divided < d && k >= 0) {
                    // 最多移动一次就能出结果
                    k--;
                    if (k < 0) break;
                    divided = divided * M + this->digits[k];
                }
                if (k < 0) break;
                if (!filled) {
                    filled = true;
                    res.digits.resize(k+1, 0);
                }
                res.digits[k] = divided / d;
                remaining = divided % d;
            }
            if (remaining != 0 && !sig) res += int2048(1); // 如果负数没除完，向下取整-1
        } else {
            // 一般情况
            int2048 remaining = *this;
            remaining.is_positive = true;
            while (k >= 0) {
                int2048 divided = rshift(remaining, k);
                while (divided < abs_right && k >= 0) {
                    k--;
                    if (k < 0) break;
                    divided = rshift(remaining, k);
                }
                if (k < 0) break;
                if (!filled) {
                    filled = true;
                    res.digits.resize(k+1, 0);
                }
                //二分试商
                size_t upper = M, lower = 1;
                int2048 trial;
                while (lower <= upper) {
                    // 直接单独复用 trial 尝试乘积运算，避免不必要复制开销
                    size_t mid = lower + (upper - lower) / 2;
                    trial.digits.resize(abs_right.get_digits(), 0);
                    size_t carry = 0;
                    for (int i = 0; i < abs_right.get_digits(); i++) {
                        size_t new_val = (mid * abs_right.digits[i] + carry) % M;
                        carry = (mid * abs_right.digits[i] + carry) / M;
                        trial.digits[i] = new_val;
                    }
                    if (carry != 0) trial.digits.push_back(carry);
                    while (!trial.digits.empty() && trial.digits.back() == 0) trial.digits.pop_back();
                    if (trial > divided) {
                        upper = mid - 1;
                    } else {
                        lower = mid + 1;
                    }
                }
                res.digits[k] = upper;
                remaining -= lshift(int2048(upper) * abs_right, k);
            }
            if (remaining.get_digits() != 0 && !sig) res += int2048(1); // 如果负数没除完，向下取整-1
        }
        res.is_positive = sig;
        *this = res;
        return *this;
    }
    int2048 operator/(int2048 left, const int2048 &right) {
        left /= right;
        return left;
    }

    // 取模
    int2048& int2048::operator%=(const int2048 &right) {
        int2048 res = *this - (*this / right) * right;
        *this = res;
        return *this;
    }
    int2048 operator%(int2048 left, const int2048 &right) {
        left -= (left / right) * right;
        return left;
    }

    // 赋值等号
    int2048& int2048::operator=(const int2048 &right) {
        if (right == *this) return *this;
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
