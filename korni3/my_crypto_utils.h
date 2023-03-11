#include <string>

#include <openssl/err.h> // ERR_print_errors
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
//#include <openssl/sha.h>

using namespace std;

//TODO  check
RSA* createPrivateRSA(std::string key);
// OK
bool RSASign(RSA* rsa, const unsigned char* Msg, size_t MsgLen, unsigned char** EncMsg,
             size_t* MsgLenEnc);
// OK
void Base64Encode(const unsigned char* buffer, size_t length, char** base64Text);

// OK
// !!!!!!!!!!!
char* signMessage(const std::string& privateKey, const std::string& plainText);

// TODO check
// base64 decode
size_t calcDecodeLength(const char* b64input);
void Base64Decode(const char* b64message, unsigned char** buffer, size_t* length);

// OK
// !!!!!!!!!!!
RSA* createPublicRSA(std::string key);

// TODO check
bool RSAVerifySignature(RSA* rsa, unsigned char* MsgHash, size_t MsgHashLen, const char* Msg,
                        size_t MsgLen, bool* Authentic);

// OK
// !!!!!!!!!!!
bool verifySignature(const std::string& publicKey, const std::string& plainText,
                     const char* signatureBase64);

string getOpenSSLError();
