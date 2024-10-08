/******************************************************************************
*
* Copyright © 2018-2019 Anthony Mai Mai_Anthony@hotmail.com. All Rights Reserved.
*
* This file is a part of the software package TinyTls, originally known as TinySsl.
* This software is written by Anthony Mai and is provided under the terms and
* conditions of the GNU General Public License Version 3.0 (GPL V3.0). For the
* specific GPL V3.0 license terms please refer to:
*         https://www.gnu.org/licenses/gpl.html.
*
* This Copyright Notices contained in this code. are NOT to be removed or modified.
* If this package is used in a product, Anthony Mai should be given attribution as
* the author of the parts of the library used. This can be in the form of a textual
* message at program startup or in documentation provided with the package.
*
* This library is free for commercial and non-commercial use as long as the
* following conditions are aheared to. The following conditions apply to
* all code found in this distribution:
*
* 1. Redistributions of source code must retain the copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. All advertising materials mentioning features or use of this software
*    must display the following acknowledgement:
*
*    "This product contains software written by Anthony Mai (Mai_Anthony@hotmail.com)
*     The original source code can obtained from such and such internet sites or by
*     contacting the author directly."
*
* 4. This software may or may not contain patented technology owned by a third party.
*    Obtaining a copy of this software, with or without explicit authorization from
*    the author, does NOT imply that applicable patents have been licensed. It is up
*    to you to make sure that utilization of this software package does not infringe
*    on any third party's patents or other intellectual proerty rights.
*
* THIS SOFTWARE IS PROVIDED BY ANTHONY MAI "AS IS". ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The license and distribution terms for any publically available version or derivative
* of this code cannot be changed.  i.e. this code cannot simply be copied and put under
* another distribution license [including the GNU Public License.]
*
******************************************************************************/

/******************************************************************************
*
*  File Name:       testMain.cpp
*
*  Description:     The main test program for the TinyTls library.
*
*
*  Programmers:     Anthony Mai (am) mai_anthony@hotmail.com
*
*  History:         10/08/2018 Initial creation
*
*  Notes:           This file uses 4 spaces indents
*
******************************************************************************/

#include <stdio.h>
#include <malloc.h>
#include <stdint.h>

#ifdef __linux__
#include <arpa/inet.h>
#elif defined(WIN32)
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "Crypt32.lib")
#endif


#include "ecc.h"
#include "aes_test.h"
#include "ecc_test.h"
#include "rsa_test.h"
#include "net_test.h"
#include "certgen_test.h"
#include "rfc8448_test.h"
#include "cipher.h"
#include "cert.h"

#define MAX_NUM_TESTS   50

#define TEST_BEGIN()                        \
    int total_tests = 0;                    \
	int num_failed_tests = 0;               \
    char *failed_tests_name[MAX_NUM_TESTS] = {0};     

#define TEST_RUN(x)                         \
	do {                                    \
		total_tests++;	                    \
		ret = (x);                          \
    	if(ret != 0)                        \
		{                                   \
			printf("%s failed:%d\n", #x, ret);   \
			failed_tests_name[num_failed_tests++] = #x;  \
		}                                   \
    }while(0)

#define TEST_RESULT()                       \
	printf("total run %d tests, failed:%d tests"  \
    , total_tests, num_failed_tests);       \
    if(num_failed_tests > 0)                \
    {                                       \
		printf(":\n");               \
		for(int i = 0; i < num_failed_tests; ++i)   \
		{                                   \
			printf("\t%s\n", failed_tests_name[i]); \
		}                                   \
    } else { printf("\n"); }

	

extern uint32_t getIp(const char* hostname);

const CIPHERSET* gpCipherSet = nullptr;

int dnsTest()
{
    uint32_t ip = 0;
    ip = getIp("www.cnn.com");
    in_addr in;
#if defined(WIN32) || defined(WIN64)
    in.S_un.S_addr = htonl(ip);
#else //defined(WIN32) || defined(WIN64)
    in.s_addr = htonl(ip);
#endif //defined(WIN32) || defined(WIN64)
    printf("IP obtained is %s\n", inet_ntoa(in));
    return ip;
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
    WSADATA wsa = {0};
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif

    TEST_BEGIN();

    int ret = 0;
    const CIPHERSET* pCipherSet = InitCiphers(&gCipherSet);

    gpCipherSet = pCipherSet;

    dnsTest();

    StartCerts(malloc, free, pCipherSet);

    TEST_RUN(do_CertGenTest(*pCipherSet));
    //TEST_RUN(ecc_Test());
    //TEST_RUN(DoRSATest());
    //TEST_RUN(aes_test());
    //TEST_RUN(rfc8448_test(*pCipherSet));
    TEST_RUN(do_clientTest());
    //TEST_RUN(do_serverTest());

    TEST_RESULT();

    CleanupCerts(NULL);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}

