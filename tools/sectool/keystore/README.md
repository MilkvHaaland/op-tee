# keystore生成

## RSA公私钥生成

1. PEM私钥格式文件
```bash
openssl genrsa -out privatekey.pem 2048
```
-out 指定生成文件，此文件包含公钥和私钥两部分，所以即可以加密，也可以解密
1024 生成密钥的长度

2. PEM公钥格式文件
```bash
openssl rsa -in privatekey.pem -pubout -out pubkey.pem
```
-in 指定输入的密钥文件
-out 指定提取生成公钥的文件(PEM公钥格式)
