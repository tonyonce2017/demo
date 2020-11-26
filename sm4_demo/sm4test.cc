/*************************************************************************
       > File Name: SM4test.c
       > Author:NEWPLAN
       > E-mail:newplan001@163.com
       > Created Time: Thu Apr 13 23:55:50 2017
************************************************************************/

#include <string.h>
#include <stdio.h>
#include "sm4.h"
#include "sm4_ecb.h"

int main(int argc, char** argv)
{
	unsigned char key[16] = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'};

//    unsigned char input[16] = {0x01, 0x23, 0x45};
    unsigned char input[16] = {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c'};
//    unsigned char input[16] = {'c', 'c', 'c'};
//    unsigned char input[16] = {0xf0, 0xea, 0xba};

//    unsigned char input[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
//    unsigned char input[16] = {0xda, 0x3f, 0x9c, 0xcf, 0xcf, 0x83, 0xfd, 0x1d, 0x4f, 0xd9, 0xe9, 0xd1, 0x25, 0xa5, 0x0e, 0x10};
//    unsigned char input[16] = {0x1a, 0x3e, 0x80, 0x59, 0x5a, 0x8d, 0x92, 0x06, 0xcd, 0xc2, 0x1a, 0x2f, 0x36, 0xf7, 0xb7, 0xd9};

    unsigned char IV[16] = {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'};

	unsigned char output[16];
	sm4_context ctx;
	unsigned long i;

    //ECB
//    sm4_setkey_enc(&ctx, key);
//    sm4_crypt_ecb(&ctx, 1, 16, input, output);
//    for (i = 0; i < 16; i++)
//        printf("%02x ", output[i]);
//    printf("\n");
//
//    sm4_setkey_dec(&ctx, key);
//    sm4_crypt_ecb(&ctx, 0, 16, output, output);
//    for (i = 0; i < 16; i++)
//        printf("%02x ", output[i]);
//    printf("\n");


    //CBC
	sm4_setkey_enc(&ctx, key);
	sm4_crypt_cbc(&ctx, 1, 16, IV, input, output);
	for (i = 0; i < 16; i++)
		printf("%02x ", output[i]);
	printf("\n");

//	sm4_setkey_dec(&ctx, key);
//	sm4_crypt_cbc(&ctx, 0, 16, IV, input, output);
//	for (i = 0; i < 16; i++)
//		printf("%c", output[i]);
//	printf("\n");

	//decrypt 1M times testing vector based on standards.
//	i = 0;
//	sm4_setkey_enc(&ctx, key);
//	while (i < 1000000)
//	{
//		sm4_crypt_ecb(&ctx, 1, 16, input, input);
//		i++;
//	}
//	for (i = 0; i < 16; i++)
//		printf("%02x ", input[i]);
//	printf("\n");

	return 0;
}


//int main(int argc, char** argv) {
////    if (argc == 1) return 0;
////
////    std::ifstream fin(argv[1]);
////
////    fin.seekg(0, std::ios::end);
////    std::string buffer;
////    size_t len = fin.tellg();
////    buffer.reserve(len);
////    fin.seekg(0, std::ios::beg);
////
////    if (len % 16) {
////        printf("Length of plain text should be multiple of 16 bytes. \n");
////        return 0;
////    }
////
////    buffer.assign((std::istreambuf_iterator<char>(fin)),
////                   std::istreambuf_iterator<char>());
////
////    fin.close();
//
//    // 128 bit key size
//    unsigned char key[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
//    unsigned char buffer[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
////    unsigned char buffer[10] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc};
////    819884fd933f8d4977042ab6361dcc23
////    c3 e5 ed 78 50 ad aa 59 08 f2 a8 40 24 ee 50 a1
////    if (argc == 3) {
////        fin.open(argv[2]);
////        fin.seekg(0, std::ios::beg);
////        char buffer[3] = { 0 };
////        for (size_t i = 0; i < 16; ++i) {
////            fin.read(buffer, 2);
////            key[i] = std::stoi(buffer, 0, 16);
////        }
////        fin.close();
////    }
//
//    std::vector<char> cipher(16, 0);
//    sm4_ecb(buffer, 16, key, &cipher[0]);
//
//    for (size_t i = 0; i < 16; ++i)
//        printf("%02x", int(cipher[i]) & 0xff);
//    printf("\n");
////    681edf34d206965e86b3e94f536e4246
////68 1e df 34 d2 06 96 5e 86 b3 e9 4f 53 6e 42 46
//}