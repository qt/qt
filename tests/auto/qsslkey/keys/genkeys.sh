#!/bin/sh

# This script generates cryptographic keys of different types.

#--- RSA ---------------------------------------------------------------------------
# Note: RSA doesn't require the key size to be divisible by any particular number
for size in 40 511 512 999 1023 1024 2048
do
  echo -e "\ngenerating RSA private key to PEM file ..."
  openssl genrsa -out rsa-pri-$size.pem $size

  echo -e "\ngenerating RSA private key to DER file ..."
  openssl rsa -in rsa-pri-$size.pem -out rsa-pri-$size.der -outform DER

  echo -e "\ngenerating RSA public key to PEM file ..."
  openssl rsa -in rsa-pri-$size.pem -pubout -out rsa-pub-$size.pem

  echo -e "\ngenerating RSA public key to DER file ..."
  openssl rsa -in rsa-pri-$size.pem -pubout -out rsa-pub-$size.der -outform DER
done

#--- DSA ----------------------------------------------------------------------------
# Note: DSA requires the key size to be in interval [512, 1024] and be divisible by 64
for size in 512 576 960 1024
do
  echo -e "\ngenerating DSA parameters to PEM file ..."
  openssl dsaparam -out dsapar-$size.pem $size

  echo -e "\ngenerating DSA private key to PEM file ..."
  openssl gendsa dsapar-$size.pem -out dsa-pri-$size.pem

  /bin/rm dsapar-$size.pem

  echo -e "\ngenerating DSA private key to DER file ..."
  openssl dsa -in dsa-pri-$size.pem -out dsa-pri-$size.der -outform DER

  echo -e "\ngenerating DSA public key to PEM file ..."
  openssl dsa -in dsa-pri-$size.pem -pubout -out dsa-pub-$size.pem

  echo -e "\ngenerating DSA public key to DER file ..."
  openssl dsa -in dsa-pri-$size.pem -pubout -out dsa-pub-$size.der -outform DER
done
