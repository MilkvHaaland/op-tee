"""
filename:    sbmeta.py
description: python script used to build sbmeta.bin
"""
import sys              #used to get argument input
import os               #provide nomal funtion of os
import yaml             #used to analyze sbmeta.yaml
import hashlib          #used to calculate hash of image file
from struct import pack #used to pack sbmeta.yaml
import subprocess      #used to run scripts
from subprocess import PIPE


#class provide function to build sbmeta.bin
class sbmeta_image_gen:
    image_type_enums = {
        "dtb" : 0,
        "kernel_image" : 1,
        "sbi" : 2,
        "aon_fw" : 3,
        "rootfs" : 4,
        "tf" : 2,
        "tee" : 5,
        "uboot" : 6,
        "customer" : 7,
    }

    digest_schema_enums = {
        "none" : 0,
        "sha1" : 1,
        "md5" : 2,
        "sha224" : 3,
        "sha256" : 4,
        "sha384" : 5,
        "sha512" : 6,
        "sm3" : 7,
    }

    digest_scheme_list = [
        "none",
        "sha1",
        "md5",
        "sha224",
        "sha256",
        "sha384",
        "sha512",
        "sm3",
    ]

    sign_schema_enums = {
        "none" : 0,
        "rsa_1024" : 1,
        "rsa_2048" : 2,
        "ecc_256" : 3,
        "ecc_160" : 4,
        "sm2" : 5,
    }

    sign_schema_list = [
        "none",
        "rsa_1024",
        "rsa_2048",
        "ecc_256",
        "ecc_160",
        "sm2",
    ]

    media_type_enums = {
        "emmc" : 0,
        "sd" : 1,
        "tfcard" : 2,
        "nandflash" : 3,
    }

    #image fields info,used to analyze field
    image_fields = {
        "device":{"type":"integer"},
        "partition":{"type":"integer"},
        "type":{"type":"enum","enums":image_type_enums},
        "digest_scheme":{"type":"enum","enums":digest_schema_enums},
        "sign_scheme":{"type":"enum", "enums":sign_schema_enums},
        "is_image_encrypted":{"type":"integer"},
        "medium_type":{"type":"enum","enums":media_type_enums},
        "checksum_scheme":{"type":"enum","enums":digest_schema_enums},
        "name":{"type":"string","max_size":32},
        "digest":{"type":"hex", "max_size": 64},
        "relocated_addr":{"type":"int32"},
    }

    #padding each image info to size
    image_padding_to_size = 4 * 1024

    #padding sbmeta.bin to size
    meta_padding_to_size = 128

    #default header version
    default_version = "1.2"

    #sign scripts
    sign_tool = "./imagesign.sh"

    def __init__(self,yaml_path):
        self.yaml_path = yaml_path
        self.sbmeta_out_dir= "sbmeta"
        self.sbmeta_out_path = os.path.join(self.sbmeta_out_dir,"sbmeta.bin")
        self.current_image_index = 0

    #read image info from yaml
    def __get_all_image_info(self):
        yaml_data_list = []
        with open(self.yaml_path) as yaml_file_handle:
            yaml_data_list = yaml.load(yaml_file_handle, Loader=yaml.FullLoader)

        return yaml_data_list["image_configs"]

    #trans hex str to binary
    def __hexstring_to_bytes(self,hex_str):
        byte_arr = b""
        if(hex_str):
            for i in range(int(len(hex_str) / 2)):
                byte_arr += pack("B",int(hex_str[2*i:2*i+2], 16))
        return byte_arr

    #check if field value exists
    def __check_field_exists(self, image_info, field_name):
        if(not image_info or not field_name in image_info):
            return self.__make_config_value_check_error(field_name,"field not exists")

        return True,""

    def __make_config_value_check_error(self,field_name,error_msg):
        return False,"config field %s check error,%s"%(field_name,error_msg)

    #check string type field value
    def __check_string_field(self, image_info, field_name, max_size):
        rtn,error_msg = self.__check_field_exists(image_info,field_name)
        if(not rtn):
            return rtn,error_msg

        #check if size exceeds
        if(len(image_info[field_name]) > max_size - 1):
            return self.__make_config_value_check_error(field_name,"str value size %d exceeded max size %d"%(len(image_info[field_name]),max_size))

        return True,""
    
    #check hex type field value
    def __check_hex_field(self, image_info, field_name, max_size):
        rtn,error_msg = self.__check_field_exists(image_info,field_name)
        if(not rtn):
            return rtn,error_msg
        
        #check if size exceeds
        if(image_info[field_name] and len(str(image_info[field_name])) > max_size * 2):
            return self.__make_config_value_check_error(field_name,"hex value size %d exceeded max size %d"%(len(image_info[field_name]),max_size * 2))

        return True,""

    #check int type field value
    def __check_int_field(self, image_info, field_name):
        rtn,error_msg = self.__check_field_exists(image_info,field_name)
        if(not rtn):
            return rtn,error_msg

        #check if value type is int
        if(not isinstance(image_info[field_name], int)):
            return self.__make_config_value_check_error(field_name,"value is not an integer")

        return True,""

    #check enum type field value
    def __check_enum_field(self, image_info, field_name, enums):
        rtn,error_msg = self.__check_field_exists(image_info,field_name)
        if(not rtn):
            return rtn,error_msg

        #check if value is in enums
        if(not image_info[field_name] in enums):
            return self.__make_config_value_check_error(field_name,"value %s is not in any of the values %s"%(str(image_info[field_name]),str(enums.keys())))

        return True,""

    def __process_string_field(self,image_info,field_name):
        max_size = sbmeta_image_gen.image_fields[field_name]["max_size"]
        rtn,error_msg = self.__check_string_field(image_info,field_name,max_size)
        if not rtn:
            return rtn,error_msg,None
        field_data = image_info[field_name].encode("utf-8")
        field_data += b'\0' * (max_size - len(image_info[field_name]))
        return True,"",field_data

    def __process_integer_field(self,image_info,field_name):
        rtn,error_msg = self.__check_int_field(image_info,field_name)
        if not rtn:
            return rtn,error_msg,None

        return True,"",pack("b",image_info[field_name])

    def __process_enum_field(self,image_info,field_name):
        enums = sbmeta_image_gen.image_fields[field_name]["enums"]
        rtn,error_msg = self.__check_enum_field(image_info,field_name,enums)
        if not rtn:
            return rtn,error_msg,None

        return True,"",pack("b",enums[image_info[field_name]])

    def __process_hex_field(self,image_info,field_name):
        max_size = sbmeta_image_gen.image_fields[field_name]["max_size"]
        rtn,error_msg = self.__check_hex_field(image_info,field_name,max_size)
        if not rtn:
            return rtn,error_msg,None

        field_value = image_info[field_name]
        if(not field_value):
            field_value = ""
        else:
            field_value = str(image_info[field_name])

        field_data = self.__hexstring_to_bytes(field_value)#trans hex str to binary
        field_data += b'\0' * (max_size - len(field_data))#padding data

        return True,"",field_data

    def __process_int32_field(self,image_info,field_name):
        rtn,error_msg = self.__check_field_exists(image_info,field_name)
        if not rtn:
            return True,"",pack("i",-1)
        else:
            rtn,error_msg = self.__check_int_field(image_info,field_name)
            if not rtn:
                return rtn,error_msg,None

            return True,"",pack("I",int(image_info[field_name]))

    #write image info into  sbmeta.bin
    def __add_image(self,image_info):
        image_fields = sbmeta_image_gen.image_fields

        print("start write image info of %s"%image_info["name"])
        self.sbmeta_file.write(pack("i",0x544D4253))#magic number

        #read all fields info and write to sbmeta
        for field_name in image_fields.keys():
            if(image_fields[field_name]["type"] == "string"):
                rtn,error_msg,field_data = self.__process_string_field(image_info,field_name)
            elif(image_fields[field_name]["type"] == "integer"):
                rtn,error_msg,field_data = self.__process_integer_field(image_info,field_name)
            elif(image_fields[field_name]["type"] == "enum"):
                rtn,error_msg,field_data = self.__process_enum_field(image_info,field_name)
            elif(image_fields[field_name]["type"] == "hex"):
                rtn,error_msg,field_data = self.__process_hex_field(image_info,field_name)
            elif(image_fields[field_name]["type"] == "int32"):
                rtn,error_msg,field_data = self.__process_int32_field(image_info,field_name)
            else:
                rtn,error_msg = False,"unkown field(%s) type error"%image_info[field_name]["type"]

            if not rtn:
                return rtn,error_msg

            self.sbmeta_file.write(field_data)#write field data to sbmeta.bin

        padding_size = sbmeta_image_gen.meta_padding_to_size *(self.current_image_index + 1) - self.sbmeta_file.tell()
        if(padding_size < 0):
            return False,"meta size exceeded"

        self.current_image_index += 1

        self.sbmeta_file.write(b'\xff' * padding_size)
        print("end write image info of %s\n"%image_info["name"])

        return True,""

    #padding sbmeta.bin
    def __padding_image(self):
        padding_size = sbmeta_image_gen.image_padding_to_size - self.sbmeta_file.tell()
        if(padding_size <= 0):
            return False,"image count exceed max count"

        self.sbmeta_file.write(b'\xff' * padding_size)
        return True,""

    # check yaml field
    # 1. check image_path version: None/invalid, non-secure, secure
    # 2. algoritm:sm/ia/nor/None
    # 3. check version field
    def __check_config_yaml(self,image_info_list):
        if(len(image_info_list) == 0):
            return False,"no image info found in %s"%self.yaml_path

        for yaml_info in image_info_list:
            temp = {}
            if "image_path" in yaml_info and yaml_info["image_path"] is not None and os.path.exists(yaml_info["image_path"]):
                # if image is secure, parse field to fill in yaml_info
                temp["is_image_exist"] = True
                image_head = None
                with open(yaml_info["image_path"],"rb") as image_file:
                    # read magic to check whether image is secure
                    image_head = image_file.read(64)
                    if image_head[4:8] == b'THED':
                        temp["is_image_secure"] = True
                        temp["sign_scheme"] = self.sign_schema_list[image_head[31]]
                        temp["digest_scheme"] = self.digest_scheme_list[image_head[30]]
                        temp["is_image_encrypted"] = (image_head[32] & 0x2) >> 1
                        yaml_info.update(temp)
                        continue
            else:
                print("warning: %s image_path is invalid. No image will be signed and no digest will be calculated!"%yaml_info["type"])
            if "algorithm" not in yaml_info or yaml_info["algorithm"] == None:
                return False, "algoritm filed does not exist"
            elif yaml_info["algorithm"] == "nor":
                temp["sign_scheme"] = "none"
                temp["digest_scheme"] = "none"
            elif yaml_info["algorithm"] == "ia":
                temp["sign_scheme"] = "rsa_2048"
                temp["digest_scheme"] = "sha256"
            elif yaml_info["algorithm"] == "sm":
                temp["sign_scheme"] = "sm2"
                temp["digest_scheme"] = "sm3"
            else:
                return False, "algorithm type has not been specified"
            yaml_info.update(temp)
        return True,""

    def __get_image_digest(self,image_info):
        if not image_info:
            return False, ""

        #check if needs to achive digest
        if "is_image_exist" not in image_info or "checksum_scheme" not in image_info or image_info["checksum_scheme"] == "none":
            image_info["digest"] = ""
        else:
            #read image data
            image_data = None
            if "signed_file" in image_info and os.path.isfile(image_info["signed_file"]):
                with open(image_info["signed_file"],"rb") as image_file:
                    image_data = image_file.read()
            else:
                with open(image_info["image_path"],"rb") as image_file:
                    image_data = image_file.read()

            hash_obj = None
            if(image_info["checksum_scheme"] == "sha1"):
                hash_obj = hashlib.sha1()
            elif(image_info["checksum_scheme"] == "md5"):
                hash_obj = hashlib.md5()
            elif(image_info["checksum_scheme"] == "sha224"):
                hash_obj = hashlib.sha224()
            elif(image_info["checksum_scheme"] == "sha256"):
                hash_obj = hashlib.sha256()
            elif(image_info["checksum_scheme"] == "sha384"):
                hash_obj = hashlib.sha384()
            elif(image_info["checksum_scheme"] == "sha512"):
                hash_obj = hashlib.sha512()
            elif(image_info["checksum_scheme"] == "sm3"):
                hash_obj = hashlib.new("sm3")
            else:
                return False,"invald checksum_scheme %s"%image_info["checksum_scheme"]

            hash_obj.update(image_data)
            image_info["digest"] = hash_obj.hexdigest()

        return True,""

    # sign images with no secure header in "image_path"
    def sign_image(self, image_info):
        if image_info is None:
            return False, "null pointer error"

        if "is_image_exist" not in image_info or image_info["is_image_exist"] != True:
            return True, "no image to sign"

        # image with secure header need not be signed again
        if "is_image_secure" in image_info and image_info["is_image_secure"] == True:
            return True, "%s has been signed"%image_info["type"]

        cmd = ["bash", self.sign_tool]

        if "algorithm"  not in image_info or image_info["algorithm"] is None:
            return False, "invalid algorithm field"

        if image_info["algorithm"] == "nor":
            return True, ""
        cmd.append(image_info["algorithm"])

        if "is_image_encrypted" not in image_info or image_info["is_image_encrypted"] is None:
            return False, "invalid is_image_encrypted field"
        if image_info["is_image_encrypted"] != 0 and image_info["is_image_encrypted"] != 1:
            return False, "invalid is_image_encrypted field"
        cmd.append("enc" if image_info["is_image_encrypted"] else "nor")

        if "type" not in image_info or image_info["type"] not in self.image_type_enums:
            return False, "invalid type field"
        cmd.append(image_info["type"])

        version_valid = False
        if "version" in image_info and image_info["version"] is not None and isinstance(image_info["version"], str):
            # check version format "x.y" 0 <= x,y <= 255
            parts = image_info["version"].split(".")
            if len(parts) == 2 and parts[0].isdigit() and parts[1].isdigit():
                if int(parts[0]) <= 255 and int(parts[1]) <= 255:
                    version_valid = True
        if not version_valid:
             print("warning: invalid version field. Use default version 1.0")
             image_info["version"] = self.default_version
        cmd.append(image_info["version"])

        # execute ./imagesign ia nor tf 1.0 (e.g.)
        result = subprocess.run(cmd, stdout=PIPE, stderr=PIPE)
        if result.returncode:
            return False, result.stderr
        else:
            output = result.stdout.decode('utf-8')
            print("=========== Note: sbmeta.py is signing %s ============"%image_info["type"])
            print(output)
            print("=================== End of signing ======================")
            # get signed file name
            output_lines = output.split("\n")
            signed_files = [line.replace("Signed file: ", "") for line in output_lines if "Signed file:" in line]
            if len(signed_files) == 0:
                print("warning: %s has not been signed\n"%image_info["type"])
                return True, ""
            image_info["signed_file"] = signed_files[0]
            return True, ""

    #build snmeta.bin
    def pack_sbmeta(self):
        if(not os.path.isfile(self.yaml_path)):
            return False,"image config yaml %s do not exists"%self.yaml_path

        if(not os.path.exists(self.sbmeta_out_dir)):
            os.mkdir(self.sbmeta_out_dir)

        with open(self.sbmeta_out_path,"wb") as sbmeta_file:
            self.sbmeta_file = sbmeta_file
            image_info_list = self.__get_all_image_info()#read image info from yaml

            rtn,msg = self.__check_config_yaml(image_info_list)#check if yaml config is valid 
            if(rtn == False):
                return rtn,msg

            #read all yam config and write info to sbmeta.bin
            for image_info in image_info_list:
                #sign images
                rtn, msg = self.sign_image(image_info)
                if(rtn == False):
                    return rtn, msg

                #get digest of each image file
                rtn, msg = self.__get_image_digest(image_info)
                if(rtn == False):
                    return False,msg

                rtn,msg = self.__add_image(image_info)
                if(rtn == False):
                    return rtn,msg

            #write padding to image
            rtn,msg = self.__padding_image()
            if(rtn == False):
                return rtn,msg

            print("sbmeta out path is %s"%self.sbmeta_out_path)

        return True,""

def print_help():
    print("sbmeta package tool version 1.0\n" + \
          "Usage:\n" + \
          "python sbmeta.py [yaml_path]\n" + \
          "yaml_path\n" + \
          "    yaml path of which used to generate sbmeta image")


if __name__ == '__main__':
    yaml_path = "./sbmeta.yaml"

    #check argument inpute
    if(len(sys.argv) == 1):
        print("no yaml path input,default use %s"%yaml_path)
    elif(len(sys.argv) == 2):
         yaml_path = sys.argv[1]
    else:
         print_help()
         exit(1)

    #build sbmeta.bin
    rtn,msg = sbmeta_image_gen(yaml_path).pack_sbmeta()

    if(rtn == True):
        print("Generate sbmeta image successfully.")
    else:
        print("Generate sbmeta image failed:%s."%msg)
        exit(1)
