"""
filename:    sbmeta.py
description: python script used to build sbmeta.bin
"""
import sys              #used to get argument input
import os               #provide nomal funtion of os
import yaml             #used to analyze sbmeta.yaml
import hashlib          #used to calculate hash of image file
from struct import pack #used to pack sbmeta.yaml

#class provide function to build sbmeta.bin
class sbmeta_image_gen:
    imag_type_enums = {
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
        "sha512" : 7,
        "sm3" : 7,
    }

    sign_schema_enums = {
        "none" : 0,
        "rsa_1024" : 1,
        "rsa_2048" : 2,
        "ecc_256" : 3,
        "ecc_160" : 4,
        "sm2" : 5,
    }

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
        "type":{"type":"enum","enums":imag_type_enums},
        "digest_scheme":{"type":"enum","enums":digest_schema_enums},
        "sign_scheme":{"type":"enum", "enums":sign_schema_enums},
        "is_image_encrypted":{"type":"integer"},
        "medium_type":{"type":"enum","enums":media_type_enums},
        "name":{"type":"string","max_size":32},
        "digest":{"type":"hex", "max_size": 64},
        "relocated_addr":{"type":"int32"},
    }

    #padding each image info to size
    image_padding_to_size = 4 * 1024

    #padding sbmeta.bin to size
    meta_padding_to_size = 128

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
            if(field_name == "medium_type"):#padding reserv0 after medium_type 
                field_data += b"\xff"
                
            if not rtn:
                return rtn,error_msg
            
            self.sbmeta_file.write(field_data)#write field data to sbmeta.bin

        padding_size = sbmeta_image_gen.meta_padding_to_size *(self.current_image_index + 1) - self.sbmeta_file.tell()
        if(padding_size < 0):
            return False,"meta size exceeded"
    
        self.current_image_index += 1

        self.sbmeta_file.write(b'\xff' * padding_size)
        print("end write image info of %s"%image_info["name"])
        
        return True,""             

    #padding sbmeta.bin
    def __padding_image(self):
        padding_size = sbmeta_image_gen.image_padding_to_size - self.sbmeta_file.tell()
        if(padding_size <= 0):
            return False,"image count exceed max count"

        self.sbmeta_file.write(b'\xff' * padding_size)
        return True,""
    
    def __check_config_yaml(self,image_info_list):
        if(len(image_info_list) == 0):
            return False,"no image info found in %s"%self.yaml_path
            
        for yaml_info in image_info_list:
            if(yaml_info["sign_scheme"] == "rsa_2048" and yaml_info["digest_scheme"] != "sha256"):
                return False,"when sign_scheme is rsa_2048,digest_scheme must be sha256,actual %s"%yaml_info["digest_scheme"]
            elif(yaml_info["sign_scheme"] == "sm2" and yaml_info["digest_scheme"] != "sm3"):
                return False,"when sign_scheme is sm2,digest_scheme must be sm3,actual %s"%yaml_info["digest_scheme"]
        
        return True,""

    def __get_image_digest(self,image_info_list):
        if not image_info_list:
            return image_info_list
        
        #loop each image_info,achive digest and assign to image_info
        image_info_list_rtn = []
        for image_info in image_info_list:
            #check if needs to achive digest
            if not "image_path" in image_info or not "digest_scheme" in image_info or image_info["digest_scheme"] == "none" or image_info["image_path"] == None:
                image_info["digest"] = ""
            else:
                if not os.path.exists(image_info["image_path"]):
                    return [],"image_path %s not exists"%image_info["image_path"]
                
                #read image data
                image_data = None
                with open(image_info["image_path"],"rb") as image_file:
                    image_data = image_file.read()
    
                hash_obj = None
                if(image_info["digest_scheme"] == "sha1"):
                    hash_obj = hashlib.sha1()
                elif(image_info["digest_scheme"] == "md5"):
                    hash_obj = hashlib.md5()
                elif(image_info["digest_scheme"] == "sha224"):
                    hash_obj = hashlib.sha224()
                elif(image_info["digest_scheme"] == "sha256"):
                    hash_obj = hashlib.sha256()
                elif(image_info["digest_scheme"] == "sha384"):
                    hash_obj = hashlib.sha384()
                elif(image_info["digest_scheme"] == "sha512"):
                    hash_obj = hashlib.sha512()
                elif(image_info["digest_scheme"] == "sm3"):
                    hash_obj = hashlib.sm3()
                else:
                    return [],"invald digest_scheme %s"%image_info["digest_scheme"]
                
                hash_obj.update(image_data)
                image_info["digest"] = hash_obj.hexdigest()
            image_info_list_rtn.append(image_info)

        return image_info_list_rtn,""

    #buld snmeta.bin
    def pack_sbmeta(self):
        if(not os.path.isfile(self.yaml_path)):
            return False,"image config yaml %s do not exists"%self.yaml_pat

        if(not os.path.exists(self.sbmeta_out_dir)):
            os.mkdir(self.sbmeta_out_dir)
    
        with open(self.sbmeta_out_path,"wb") as sbmeta_file:
            self.sbmeta_file = sbmeta_file
            image_info_list = self.__get_all_image_info()#read image info from yaml
            
            rtn,msg = self.__check_config_yaml(image_info_list)#check if yaml config is valid 
            if(rtn == False):
                return rtn,msg
            
            #get digest of each image file
            image_info_list,msg = self.__get_image_digest(image_info_list)
            if(len(image_info_list) == 0):
                return False,msg

            #read all yam config and write info to sbmeta.bin
            for image_info in image_info_list:
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
         os._exit(0)

    #build sbmeta.bin
    rtn,msg = sbmeta_image_gen(yaml_path).pack_sbmeta()

    if(rtn == True):
        print("Generate sbmeta image successfully.")
    else:
        print("Generate sbmeta image failed:%s."%msg)
