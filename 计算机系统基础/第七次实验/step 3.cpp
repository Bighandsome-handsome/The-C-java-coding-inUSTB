#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <sstream>
using namespace std;

typedef unsigned char BYTE;

class CAES
{
public:
    /*
    * 功  能：初始化
    */
    CAES();
    ~CAES();

    /*
     * 功  能：加密，加密后的字节串长度只会是16字节的整数倍
     * 参  数：src_data — 需要加密的字节串，不允许为空
     *        src_len — src_data长度，不允许为0
     *        dst_data — 指向加密后字节串的指针，如果该指针为空或者dst_len小于加密后所需的字节长度，函数内部会自动分配空间
     *        dst_len — dst_data长度
     *        release_dst — 函数内部自动分配空间时是否删除现有空间
     * 返回值: 加密字节串长度
     */
    size_t Encrypt(const void* const src_data, size_t src_len, void* dst_data, size_t dst_len, bool release_dst = false);

    /*
    * 功  能：解密
    * 参  数：data — [IN] 需要解密的字节串，不允许为空
    *                [OUT]解密后的字节串
    *        len — 字节串长度，该长度必须是16字节(128bit)的整数倍
    */
    void Decrypt(void* data, size_t len);

    /*
    * 功  能: 获取待加密的字节串被加密后字节长度
    * 参  数: src_len — 需要加密的字节串长度
    * 返回值: 加密后字节串长度 
    */
    size_t GetEncryptDataLen(size_t src_len) const;
    //输出密文
    void printHex(char *ptr, int len);

private:
    // 对data前16字节进行加密
    void Encrypt(BYTE* data);
    // 对data前16字节进行解密
    void Decrypt(BYTE* data);
    // 密钥扩展
    void KeyExpansion(const BYTE* key);
    BYTE FFmul(BYTE a, BYTE b);
    // 轮密钥加变换
    void AddRoundKey(BYTE data[][4], BYTE key[][4]);
    // 加密字节替代
    void EncryptSubBytes(BYTE data[][4]);
    // 解密字节替代
    void DecryptSubBytes(BYTE data[][4]);
    // 加密行移位变换
    void EncryptShiftRows(BYTE data[][4]);
    // 解密行移位变换
    void DecryptShiftRows(BYTE data[][4]);
    // 加密列混淆变换
    void EncryptMixColumns(BYTE data[][4]);
    // 解密列混淆变换
    void DecryptMixColumns(BYTE data[][4]);

private:
    BYTE* encrypt_permutation_table_;   // 加密置换表
    BYTE* decrypt_permutation_table_;   // 解密置换表
    BYTE round_key_[11][4][4];          // 轮密钥
};


CAES::CAES()
    :encrypt_permutation_table_(new BYTE[256]),
    decrypt_permutation_table_(new BYTE[256])
{
    const BYTE encrypt_permutation_table[] = {
        0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
        0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
        0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
        0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
        0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
        0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
        0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
        0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
        0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
        0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
        0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
        0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
        0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
        0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
        0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
        0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
    };

    const BYTE decrypt_permutation_table[256] = {
        0x52,0x09,0x6A,0xD5,0x30,0x36,0xA5,0x38,0xBF,0x40,0xA3,0x9E,0x81,0xF3,0xD7,0xFB,
        0x7C,0xE3,0x39,0x82,0x9B,0x2F,0xFF,0x87,0x34,0x8E,0x43,0x44,0xC4,0xDE,0xE9,0xCB,
        0x54,0x7B,0x94,0x32,0xA6,0xC2,0x23,0x3D,0xEE,0x4C,0x95,0x0B,0x42,0xFA,0xC3,0x4E,
        0x08,0x2E,0xA1,0x66,0x28,0xD9,0x24,0xB2,0x76,0x5B,0xA2,0x49,0x6D,0x8B,0xD1,0x25,
        0x72,0xF8,0xF6,0x64,0x86,0x68,0x98,0x16,0xD4,0xA4,0x5C,0xCC,0x5D,0x65,0xB6,0x92,
        0x6C,0x70,0x48,0x50,0xFD,0xED,0xB9,0xDA,0x5E,0x15,0x46,0x57,0xA7,0x8D,0x9D,0x84,
        0x90,0xD8,0xAB,0x00,0x8C,0xBC,0xD3,0x0A,0xF7,0xE4,0x58,0x05,0xB8,0xB3,0x45,0x06,
        0xD0,0x2C,0x1E,0x8F,0xCA,0x3F,0x0F,0x02,0xC1,0xAF,0xBD,0x03,0x01,0x13,0x8A,0x6B,
        0x3A,0x91,0x11,0x41,0x4F,0x67,0xDC,0xEA,0x97,0xF2,0xCF,0xCE,0xF0,0xB4,0xE6,0x73,
        0x96,0xAC,0x74,0x22,0xE7,0xAD,0x35,0x85,0xE2,0xF9,0x37,0xE8,0x1C,0x75,0xDF,0x6E,
        0x47,0xF1,0x1A,0x71,0x1D,0x29,0xC5,0x89,0x6F,0xB7,0x62,0x0E,0xAA,0x18,0xBE,0x1B,
        0xFC,0x56,0x3E,0x4B,0xC6,0xD2,0x79,0x20,0x9A,0xDB,0xC0,0xFE,0x78,0xCD,0x5A,0xF4,
        0x1F,0xDD,0xA8,0x33,0x88,0x07,0xC7,0x31,0xB1,0x12,0x10,0x59,0x27,0x80,0xEC,0x5F,
        0x60,0x51,0x7F,0xA9,0x19,0xB5,0x4A,0x0D,0x2D,0xE5,0x7A,0x9F,0x93,0xC9,0x9C,0xEF,
        0xA0,0xE0,0x3B,0x4D,0xAE,0x2A,0xF5,0xB0,0xC8,0xEB,0xBB,0x3C,0x83,0x53,0x99,0x61,
        0x17,0x2B,0x04,0x7E,0xBA,0x77,0xD6,0x26,0xE1,0x69,0x14,0x63,0x55,0x21,0x0C,0x7D
    };

    memcpy(encrypt_permutation_table_, encrypt_permutation_table, 256);
    memcpy(decrypt_permutation_table_, decrypt_permutation_table, 256);
    const BYTE key[16] = {0x08, 0x01, 0x04, 0x03, 0x04, 0x09, 0x02, 0x07, 0x09, 0x09, 0x0a, 0x0d, 0x01, 0x0d, 0x0e, 0x0f};
    KeyExpansion(key);
}

CAES::~CAES()
{
    if (encrypt_permutation_table_)
    {
        delete[]encrypt_permutation_table_;
        encrypt_permutation_table_ = 0;
    }

    if (decrypt_permutation_table_)
    {
        delete[]decrypt_permutation_table_;
        decrypt_permutation_table_ = 0;
    }
}


size_t CAES::Encrypt(const void *const src_data, size_t src_len, void* dst_data, size_t dst_len, bool release_dst)
{
    if (0 == src_data)
    {
        assert(src_data);
        return 0;
    }

    if (0 == src_len)
    {
        return 0;
    }

    const size_t len = GetEncryptDataLen(src_len);
    if (0 == dst_data)
    {
        dst_data = new BYTE[len];
    }
    else if (len > dst_len)
    {
        if (release_dst)
        {
            delete[]dst_data;
        }
        dst_data = new BYTE[len];
    }

    dst_len = len;

    memset(dst_data, 0, dst_len);
    memcpy(dst_data, src_data, src_len);

    for (size_t i = 0; i < dst_len; i += 16)
    {
        Encrypt(reinterpret_cast<BYTE*>(dst_data) + i);
    }

    return dst_len;
}

void CAES::Decrypt(void* data, size_t len)
{
    if (0 == data || len % 16 != 0)
    {
        assert(data);
        assert(len % 16);
        return;
    }

    for (size_t i = 0; i < len; i += 16)
    {
        Decrypt(reinterpret_cast<BYTE*>(data) + i);
    }
}

size_t CAES::GetEncryptDataLen(size_t src_len) const
{
    if (src_len % 16 == 0)
    {
        return src_len;
    }
    else
    {
        return src_len + 16 - (src_len % 16);
    }
}

void add(char* str)
{
	int result = std::stoi(str) + 2573;
	for (int i = 3; i >= 0; --i){
		str[i] = (result % 10) + '0';
		result = result / 10;
	}
}

void CAES::Encrypt(BYTE* data)
{
    BYTE encrypt_data[4][4];
    for (int row_index = 0; row_index < 4; row_index++)
    {
        for (int col_index = 0; col_index < 4; col_index++)
        {
            encrypt_data[row_index][col_index] = data[col_index * 4 + row_index];
        }
    }

    AddRoundKey(encrypt_data, round_key_[0]);

    for (int i = 1; i <= 10; i++)
    {
        EncryptSubBytes(encrypt_data);
        EncryptShiftRows(encrypt_data);
        if (i != 10)
        {
            EncryptMixColumns(encrypt_data);
        }
        AddRoundKey(encrypt_data, round_key_[i]);
    }

    for (int row_index = 0; row_index < 4; row_index++)
    {
        for (int col_index = 0; col_index < 4; col_index++)
        {
            data[col_index * 4 + row_index] = encrypt_data[row_index][col_index];
        }
    }
}

void CAES::Decrypt(BYTE* data)
{
    BYTE decrypt_data[4][4];
    for (int row_index = 0; row_index < 4; row_index++)
    {
        for (int col_index = 0; col_index < 4; col_index++)
        {
            decrypt_data[row_index][col_index] = data[col_index * 4 + row_index];
        }
    }

    AddRoundKey(decrypt_data, round_key_[10]);

    for (int i = 9; i >= 0; i--)
    {
        DecryptShiftRows(decrypt_data);
        DecryptSubBytes(decrypt_data);
        AddRoundKey(decrypt_data, round_key_[i]);

        if (i)
        {
            DecryptMixColumns(decrypt_data);
        }
    }

    for (int row_index = 0; row_index < 4; row_index++)
    {
        for (int col_index = 0; col_index < 4; col_index++)
        {
            data[col_index * 4 + row_index] = decrypt_data[row_index][col_index];
        }
    }
}
void pow(char* str)
{
	int result = std::stoi(str);
	result = result*result;
	for (int i = 3; i >= 0; --i){
		str[i] = (result % 10) + '0';
		result = result / 10;
	}
}

void mul(char* str)
{
	int result = std::stoi(str) * 2573;
	for (int i = 3; i >= 0; --i){
		str[i] = (result % 10) + '0';
		result = result / 10;
	}
}
void CAES::KeyExpansion(const BYTE* key)
{
    for (int row_index = 0; row_index < 4; row_index++)
    {
        for (int col_index = 0; col_index < 4; col_index++)
        {
            round_key_[0][row_index][col_index] = key[row_index + col_index * 4];
        }
    }

    // 轮常量
    const BYTE round_const_value[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36 };

    for (int i = 1; i <= 10; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            BYTE tmp[4];
            for (int row_index = 0; row_index < 4; row_index++)
            {
                tmp[row_index] = j ? round_key_[i][row_index][j - 1] : round_key_[i - 1][row_index][3];
            }

            if (j == 0)
            {
                BYTE table_index = tmp[0];
                for (int row_index = 0; row_index < 3; row_index++)
                {
                    tmp[row_index] = encrypt_permutation_table_[tmp[(row_index + 1) % 4]];
                }

                tmp[3] = encrypt_permutation_table_[table_index];
                tmp[0] ^= round_const_value[i - 1];
            }

            for (int row_index = 0; row_index < 4; row_index++)
            {
                round_key_[i][row_index][j] = round_key_[i - 1][row_index][j] ^ tmp[row_index];
            }
        }
    }
}

BYTE CAES::FFmul(BYTE a, BYTE b)
{
    BYTE b_power[4] = {b, 0, 0, 0};
    for (int i = 1; i < 4; i++)
    {
        b_power[i] = b_power[i - 1] << 1;
        if (b_power[i - 1] & 0x80)
        {
            b_power[i] ^= 0x1B;
        }
    }

    BYTE value = 0;
    for (int i = 0; i < 4; i++)
    {
        if ((a >> i) & 0x01)
        {
            value ^= b_power[i];
        }
    }

    return value;
}

void CAES::EncryptSubBytes(BYTE data[][4])
{
    for (int row_index = 0; row_index < 4; row_index++)
    {
        for (int col_index = 0; col_index < 4; col_index++)
        {
            data[row_index][col_index] = encrypt_permutation_table_[data[row_index][col_index]];
        }
    }
}

void CAES::EncryptShiftRows(BYTE data[][4])
{
    BYTE tmp[4];
    for (int row_index = 1; row_index < 4; row_index++)
    {
        for (int col_index = 0; col_index < 4; col_index++)
        {
            tmp[col_index] = data[row_index][(col_index + row_index) % 4];
        }

        for (int col_index = 0; col_index < 4; col_index++)
        {
            data[row_index][col_index] = tmp[col_index];
        }
    }
}

void CAES::EncryptMixColumns(BYTE data[][4])
{
    BYTE tmp[4];
    for (int col_index = 0; col_index < 4; col_index++)
    {
        for (int row_index = 0; row_index < 4; row_index++)
        {
            tmp[row_index] = data[row_index][col_index];
        }

        for (int row_index = 0; row_index < 4; row_index++)
        {
            data[row_index][col_index] = FFmul(0x02, tmp[row_index])
                ^ FFmul(0x03, tmp[(row_index + 1) % 4])
                ^ FFmul(0x01, tmp[(row_index + 2) % 4])
                ^ FFmul(0x01, tmp[(row_index + 3) % 4]);
        }
    }
}

void CAES::AddRoundKey(BYTE data[][4], BYTE key[][4])
{
    for (int col_index = 0; col_index < 4; col_index++)
    {
        for (int row_index = 0; row_index < 4; row_index++)
        {
            data[row_index][col_index] ^= key[row_index][col_index];
        }
    }
}

void testAns(char* studentid, char* passwd, string ans){
    add(studentid);
    pow(studentid);
    mul(studentid);
	for (int  i = 0; i < int(strlen(studentid)); ++i)
	{
		cout<<studentid[i];
		//cout<<passwd[i];
	}
	cout<<"\n";
    for (int  i = 0; i < int(strlen(studentid)); ++i)
        if ((studentid[i]) != (passwd[i])){
            cout << "Error Ans" << i << endl;
            return;
        }

    CAES* temp = new CAES();
    char* dst_data = new char[160];
    temp->Encrypt(studentid, strlen(studentid), dst_data, 160);
    delete temp;

    stringstream ss;
    int len = ans.length()/2;
    char* charArray = new char[len]; // 根据16进制字符串长度计算char数组大小
    // 将16进制字符串以2位一组的形式读取，转换为对应的字节
    for (int i = 0; i < len*2; i += 2) {
        unsigned int value;
        ss << std::hex << ans.substr(i, 2);
        ss >> value;
        ss.clear(); // 重置stringstream状态
        charArray[i / 2] = static_cast<char>(value);
    }
	for (int  i = 0; i < len; ++i)
	{
		cout<<charArray[i];
		cout<<dst_data[i];
	}
	cout<<endl;
    for (int  i = 0; i < len; ++i)
        if ((charArray[i]) != (dst_data[i])){
            cout << "Error Ans" << i << endl;
            return;
        }
    cout << "Accurate Ans" << endl;
}
void CAES::DecryptSubBytes(BYTE data[][4])
{
    for (int row_index = 0; row_index < 4; row_index++)
    {
        for (int col_index = 0; col_index < 4; col_index++)
        {
            data[row_index][col_index] = decrypt_permutation_table_[data[row_index][col_index]];
        }
    }
}

void CAES::DecryptShiftRows(BYTE data[][4])
{
    BYTE tmp[4];
    for (int row_index = 1; row_index < 4; row_index++)
    {
        for (int col_index = 0; col_index < 4; col_index++)
        {
            tmp[col_index] = data[row_index][(col_index - row_index + 4) % 4];
        }

        for (int col_index = 0; col_index < 4; col_index++)
        {
            data[row_index][col_index] = tmp[col_index];
        }
    }
}

void CAES::DecryptMixColumns(BYTE data[][4])
{
    BYTE tmp[4];
    for (int col_index = 0; col_index < 4; col_index++)
    {
        for (int row_index = 0; row_index < 4; row_index++)
        {
            tmp[row_index] = data[row_index][col_index];
        }

        for (int row_index = 0; row_index < 4; row_index++)
        {
            data[row_index][col_index] = FFmul(0x0e, tmp[row_index])
                ^ FFmul(0x0b, tmp[(row_index + 1) % 4])
                ^ FFmul(0x0d, tmp[(row_index + 2) % 4])
                ^ FFmul(0x09, tmp[(row_index + 3) % 4]);
        }
    }
}

void CAES::printHex(char *ptr, int len)
{
    BYTE* ans = new BYTE[len];
    for (int i = 0; i < len; ++i){
        ans[i] = ptr[i];
    }
    for (int i = 0; i < len; ++i){
         printf("%02X", ans[i]);
    }
    delete[] ans;
    cout << endl;
}


int main(){
    char studentid[] = "7856";
    char passwd[] = "7493";
    string ans = "f";

    testAns(studentid, passwd, ans);
    return 0;
}

