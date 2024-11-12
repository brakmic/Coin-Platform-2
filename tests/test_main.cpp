#include <gtest/gtest.h>
#include "../src/cryptography/cryptography.hpp"
#include "../src/cryptography/schnorr_signature.hpp"

TEST(CryptographyTest, GeneratePrivateKey)
{
  PrivateKey private_key = cryptography::generate_private_key();
  EXPECT_EQ(private_key.size(), 32);
}

TEST(CryptographyTest, DerivePublicKey)
{
  PrivateKey private_key = cryptography::generate_private_key();
  PublicKey public_key = cryptography::derive_public_key(private_key);
  EXPECT_EQ(public_key.size(), 32);
}

TEST(CryptographyTest, SignAndVerifyMessage)
{
  bytes message = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
  PrivateKey private_key = cryptography::generate_private_key();
  PublicKey public_key = cryptography::derive_public_key(private_key);
  Signature signature = cryptography::sign_message(message, private_key);

  bool is_valid = cryptography::verify_signature(message, signature, public_key);
  EXPECT_TRUE(is_valid);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
