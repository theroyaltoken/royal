// Copyright (c) 2014-2019, AEON, The Monero Project 
 Copyright (c) 2022, Royal 
//  Copyright (c) 2022, Royal
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#include "gtest/gtest.h"

#include <vector>

#include "crypto/crypto.h"
#include "ringct/rctTypes.h"

using namespace crypto;

TEST(borromean_signature, valid)
{
  const char message[] = "borromean_signature";
  hash prefix_hash;
  hash_to_scalar(message, sizeof(message), *(ec_scalar*)&prefix_hash);
  for (size_t n = 1; n < 30; ++n)
  {
    const size_t ring_size = 3 + (n % 5);
    std::vector<key_image> images(n);
    std::vector<std::vector<public_key>> pubs(n);
    std::vector<secret_key> secs(n);
    std::vector<std::size_t> sec_indices(n);
    for (size_t i = 0; i < n; ++i)
    {
      pubs[i].resize(ring_size);
      sec_indices[i] = i % ring_size;
      for (size_t j = 0; j < ring_size; ++j)
      {
        secret_key sec;
        generate_keys(pubs[i][j], sec);
        if (j == sec_indices[i])
        {
          secs[i] = sec;
          generate_key_image(pubs[i][j], sec, images[i]);
        }
      }
    }
    borromean_signature sig;
    generate_borromean_signature(prefix_hash, images, pubs, secs, sec_indices, {}, nullptr, sig);
    EXPECT_TRUE(check_borromean_signature(prefix_hash, images, pubs, sig));
  }
}

TEST(borromean_signature, invalid)
{
  const char message[] = "borromean_signature";
  hash prefix_hash;
  hash_to_scalar(message, sizeof(message), *(ec_scalar*)&prefix_hash);
  for (size_t n = 1; n < 30; ++n)
  {
    const size_t ring_size = 3 + (n % 5);
    std::vector<key_image> images(n);
    std::vector<std::vector<public_key>> pubs(n);
    std::vector<secret_key> secs(n);
    std::vector<std::size_t> sec_indices(n);
    for (size_t i = 0; i < n; ++i)
    {
      pubs[i].resize(ring_size);
      sec_indices[i] = i % ring_size;
      for (size_t j = 0; j < ring_size; ++j)
      {
        secret_key sec;
        generate_keys(pubs[i][j], sec);
        if (j == sec_indices[i])
        {
          secs[i] = sec;
          generate_key_image(pubs[i][j], sec, images[i]);
        }
      }
    }
    borromean_signature sig;
    generate_borromean_signature(prefix_hash, images, pubs, secs, sec_indices, {}, nullptr, sig);
    borromean_signature bad_sig = sig;
    public_key pub;
    secret_key sec;
    generate_keys(pub, sec);
    {
      bad_sig.c = *(ec_scalar*)&sec;
      EXPECT_FALSE(check_borromean_signature(prefix_hash, images, pubs, bad_sig));
    }
    bad_sig = sig;
    {
      bad_sig.r[n/2][0] = *(ec_scalar*)&sec;
      EXPECT_FALSE(check_borromean_signature(prefix_hash, images, pubs, bad_sig));
    }
    bad_sig = sig;
    {
      std::swap(bad_sig.r[n/2].front(), bad_sig.r[n/2].back());
      EXPECT_FALSE(check_borromean_signature(prefix_hash, images, pubs, bad_sig));
    }
  }
}
