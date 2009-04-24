#!/bin/sh

# This script generates digital certificates of different types.

#--- Certificates ---------------------------------------------------------------------------
echo -e "\ngenerating 1024-bit RSA private key to PEM file ..."
openssl genrsa -out rsa-pri-1024.pem 1024

echo -e "\ngenerating the corresponding public key to PEM and DER file ..."
openssl rsa -in rsa-pri-1024.pem -pubout -out rsa-pub-1024.pem
openssl rsa -in rsa-pri-1024.pem -pubout -out rsa-pub-1024.der -outform der

echo -e "\ngenerating certificate signing request (CSR) ..."
openssl req -out req.pem -new -key rsa-pri-1024.pem -subj "/CN=name\/with\/slashes/C=NO"

echo -e "\n generating a self-signed certifificate to PEM file ..."
openssl x509 -req -in req.pem -out cert-ss.pem -signkey rsa-pri-1024.pem

echo -e "\n generating a self-signed certifificate to DER file ..."
openssl x509 -req -in req.pem -out cert-ss.der -signkey rsa-pri-1024.pem -outform der

echo -e "\n generating a certifificate signed by a dummy CA to PEM file ..."
openssl x509 -req -in req.pem -out cert.pem -CA ca-cert.pem -set_serial 17

echo -e "\n generating a certifificate signed by a dummy CA to DER file ..."
openssl x509 -req -in req.pem -out cert.der -CA ca-cert.pem -set_serial 17 -outform der

#--- Public keys --------------------------------------------------------------------------------
echo -e "\n associate public keys with all certificates ..."
# Note: For now, there is only one public key (encoded in both PEM and DER), but that could change.
/bin/cp rsa-pub-1024.pem cert-ss.pem.pubkey
/bin/cp rsa-pub-1024.der cert-ss.der.pubkey
/bin/cp rsa-pub-1024.pem cert.pem.pubkey
/bin/cp rsa-pub-1024.der cert.der.pubkey

#--- Digests --------------------------------------------------------------------------------
echo -e "\n generating md5 and sha1 digests of all certificates ..."
for digest in md5 sha1
do
  openssl x509 -in ca-cert.pem -noout -fingerprint -$digest > ca-cert.pem.digest-$digest
  openssl x509 -in cert-ss.pem -noout -fingerprint -$digest > cert-ss.pem.digest-$digest
  openssl x509 -in cert.pem -noout -fingerprint -$digest > cert.pem.digest-$digest
done

#--- Subjet Alternative Name extension ----------------------------------------------------
echo -e "\n generating self signed root cert. with Subject Alternative Name extension (X509v3) ..."
outname=cert-ss-san.pem
openssl req -out req-san.pem -new -key rsa-pri-1024.pem -subj "/CN=Johnny GuitarC=NO"
openssl req -x509 -in req-san.pem -out $outname -key rsa-pri-1024.pem \
    -config san.cnf -extensions subj_alt_name
/bin/cp san.cnf $outname.san

echo -e "\n cleaning up ..."
/bin/rm rsa-pri-1024.pem rsa-pub-1024.* req*.pem
