#ifndef __DSA__DATA_H__
#define __DSA__DATA_H__
#include <stdint.h>


static const uint8_t keygen_dsa_test1_p[] = {
	0xd3, 0x83, 0x11, 0xe2, 0xcd, 0x38, 0x8c, 0x3e, 0xd6, 0x98, 0xe8, 0x2f,
	0xdf, 0x88, 0xeb, 0x92, 0xb5, 0xa9, 0xa4, 0x83, 0xdc, 0x88, 0x00, 0x5d,
	0x4b, 0x72, 0x5e, 0xf3, 0x41, 0xea, 0xbb, 0x47, 0xcf, 0x8a, 0x7a, 0x8a,
	0x41, 0xe7, 0x92, 0xa1, 0x56, 0xb7, 0xce, 0x97, 0x20, 0x6c, 0x4f, 0x9c,
	0x5c, 0xe6, 0xfc, 0x5a, 0xe7, 0x91, 0x21, 0x02, 0xb6, 0xb5, 0x02, 0xe5,
	0x90, 0x50, 0xb5, 0xb2, 0x1c, 0xe2, 0x63, 0xdd, 0xdb, 0x20, 0x44, 0xb6,
	0x52, 0x23, 0x6f, 0x4d, 0x42, 0xab, 0x4b, 0x5d, 0x6a, 0xa7, 0x31, 0x89,
	0xce, 0xf1, 0xac, 0xe7, 0x78, 0xd7, 0x84, 0x5a, 0x5c, 0x1c, 0x1c, 0x71,
	0x47, 0x12, 0x31, 0x88, 0xf8, 0xdc, 0x55, 0x10, 0x54, 0xee, 0x16, 0x2b,
	0x63, 0x4d, 0x60, 0xf0, 0x97, 0xf7, 0x19, 0x07, 0x66, 0x40, 0xe2, 0x09,
	0x80, 0xa0, 0x09, 0x31, 0x13, 0xa8, 0xbd, 0x73
};

static const uint8_t keygen_dsa_test1_q[] = {
	0x96, 0xc5, 0x39, 0x0a, 0x8b, 0x61, 0x2c, 0x0e, 0x42, 0x2b, 0xb2, 0xb0,
	0xea, 0x19, 0x4a, 0x3e, 0xc9, 0x35, 0xa2, 0x81
};

static const uint8_t keygen_dsa_test1_g[] = {
	0x06, 0xb7, 0x86, 0x1a, 0xbb, 0xd3, 0x5c, 0xc8, 0x9e, 0x79, 0xc5, 0x2f,
	0x68, 0xd2, 0x08, 0x75, 0x38, 0x9b, 0x12, 0x73, 0x61, 0xca, 0x66, 0x82,
	0x21, 0x38, 0xce, 0x49, 0x91, 0xd2, 0xb8, 0x62, 0x25, 0x9d, 0x6b, 0x45,
	0x48, 0xa6, 0x49, 0x5b, 0x19, 0x5a, 0xa0, 0xe0, 0xb6, 0x13, 0x7c, 0xa3,
	0x7e, 0xb2, 0x3b, 0x94, 0x07, 0x4d, 0x3c, 0x3d, 0x30, 0x00, 0x42, 0xbd,
	0xf1, 0x57, 0x62, 0x81, 0x2b, 0x63, 0x33, 0xef, 0x7b, 0x07, 0xce, 0xba,
	0x78, 0x60, 0x76, 0x10, 0xfc, 0xc9, 0xee, 0x68, 0x49, 0x1d, 0xbc, 0x1e,
	0x34, 0xcd, 0x12, 0x61, 0x54, 0x74, 0xe5, 0x2b, 0x18, 0xbc, 0x93, 0x4f,
	0xb0, 0x0c, 0x61, 0xd3, 0x9e, 0x7d, 0xa8, 0x90, 0x22, 0x91, 0xc4, 0x43,
	0x4a, 0x4e, 0x22, 0x24, 0xc3, 0xf4, 0xfd, 0x9f, 0x93, 0xcd, 0x6f, 0x4f,
	0x17, 0xfc, 0x07, 0x63, 0x41, 0xa7, 0xe7, 0xd9
};

static const uint8_t keygen_dsa_2048_p[] = {
    0xee, 0xd0, 0x42, 0x68, 0xcf, 0x89, 0x49, 0x8e, 0xbd, 0x2a, 0x3e, 0x4a, 0xb8, 0xa0, 0x62, 
    0x23, 0xf7, 0x9b, 0x11, 0xf3, 0x06, 0xe7, 0xc4, 0x1b, 0xee, 0x03, 0xf2, 0x41, 0xac, 0x6a, 0xf2, 
    0xc3, 0x30, 0x61, 0x2f, 0x1a, 0x31, 0x35, 0x81, 0x90, 0xc7, 0x5b, 0xfa, 0xc2, 0x20, 0x81, 0x2b, 
    0x74, 0x8b, 0x16, 0xf5, 0x14, 0x2e, 0x6a, 0x7b, 0xd7, 0xbd, 0x8f, 0x49, 0x67, 0x16, 0x2c, 0x2c, 
    0xd2, 0xa2, 0x30, 0xde, 0x34, 0x1a, 0x93, 0xd9, 0x81, 0x05, 0xad, 0x28, 0xdb, 0x21, 0xb4, 0xbc, 
    0x73, 0xbf, 0x68, 0xab, 0x8f, 0x25, 0x91, 0xf1, 0xc7, 0x47, 0xe5, 0x32, 0x43, 0x43, 0x1e, 0x63, 
    0xe2, 0x6f, 0x85, 0x91, 0x2d, 0x6e, 0xfd, 0x29, 0x15, 0xba, 0x78, 0xa3, 0xc7, 0xaf, 0xf7, 0x32, 
    0xef, 0x37, 0xa5, 0xd5, 0x8d, 0xef, 0x50, 0xc6, 0xd2, 0x1c, 0x1c, 0x14, 0xfb, 0x72, 0x61, 0x74, 
    0x3f, 0xb5, 0xc0, 0xf7, 0xb0, 0x73, 0xe7, 0x32, 0x44, 0x59, 0x15, 0x48, 0xc4, 0xa7, 0x71, 0x66, 
    0x35, 0xc5, 0xa7, 0x19, 0x35, 0x54, 0x7d, 0x9d, 0xca, 0xe4, 0xbb, 0x1c, 0xb3, 0xda, 0xf8, 0xf0, 
    0x08, 0x83, 0x3c, 0x00, 0xef, 0xe6, 0xf4, 0x91, 0x66, 0x5d, 0x7e, 0x0a, 0x7f, 0x2b, 0x2b, 0x64, 
    0xc8, 0x49, 0x55, 0x5c, 0x10, 0xab, 0x69, 0x10, 0x27, 0x5a, 0x61, 0xe0, 0xbc, 0x45, 0x5a, 0x77, 
    0x38, 0x28, 0x3a, 0x73, 0x93, 0xdc, 0x81, 0x94, 0xcb, 0x8b, 0x2c, 0x4a, 0xe7, 0x19, 0x31, 0xa2, 
    0xf7, 0xa2, 0xfd, 0xd5, 0xbd, 0x16, 0xf5, 0xd7, 0x91, 0xf6, 0x96, 0xc6, 0x5b, 0x23, 0xad, 0x08, 
    0x3d, 0xc4, 0x4c, 0x07, 0x82, 0xe3, 0x01, 0x0e, 0x27, 0x15, 0xc1, 0xb9, 0x30, 0x47, 0xb9, 0x28, 
    0x1b, 0xce, 0x7a, 0xbf, 0x96, 0x1e, 0x05, 0xfd, 0x19, 0xa2, 0x95, 0xf8, 0x9b, 0x62, 0x8d, 0x03, 
    0x21
};

static const uint8_t keygen_dsa_2048_q[] = {
    0xa1, 0x72, 0x99, 0x7e, 0x8d, 0x50, 0xf3, 0x0a, 0x7a, 0xbe, 0x33, 0x62, 0xe1, 
    0x92, 0xe1, 0xfb, 0x12, 0x74, 0x1e, 0xc9, 0x23, 0x34, 0x2e, 0x21, 0xcf, 0xdc, 
    0x21, 0x6a, 0xa4, 0x97, 0x1e, 0x4b
};

static const uint8_t keygen_dsa_2048_g[] = {
    0xb5, 0x86, 0xe0, 0x36, 0x32, 0x77, 0x0b, 0x43, 0x86, 0xac, 0xe6, 0xbd, 0x2d, 0x64, 0x73, 0x09, 
    0x64, 0xfe, 0xc3, 0xf0, 0x6e, 0xbf, 0x24, 0xb5, 0xd7, 0x79, 0xa2, 0x24, 0x5e, 0xe3, 0xbf, 0xd6, 
    0xaf, 0x0c, 0xac, 0xb2, 0x74, 0x21, 0x04, 0xe4, 0x01, 0x7e, 0x01, 0x05, 0x9f, 0x31, 0xd0, 0xb6, 
    0x09, 0x34, 0x5a, 0x33, 0x13, 0x34, 0x90, 0x31, 0x39, 0x19, 0x0d, 0xd4, 0x90, 0x68, 0x60, 0xd0, 
    0xbd, 0xe2, 0xbb, 0x4d, 0x3f, 0xf4, 0x21, 0xc7, 0x02, 0xdf, 0x1c, 0x26, 0x34, 0xb1, 0xf1, 0x53, 
    0x22, 0xd0, 0xb7, 0x63, 0x9c, 0x67, 0x8b, 0x66, 0xeb, 0xd9, 0x25, 0x2a, 0x4e, 0x5f, 0xf7, 0xbb, 
    0x5c, 0x69, 0xb7, 0x3d, 0xd7, 0x50, 0x35, 0xc5, 0x48, 0x1b, 0x5a, 0xd8, 0x5d, 0xb4, 0x0d, 0x86, 
    0xfd, 0xff, 0x21, 0x76, 0x99, 0x66, 0x8a, 0x61, 0xf4, 0x27, 0x01, 0x59, 0x7a, 0x6e, 0x82, 0x88, 
    0xab, 0x87, 0x2a, 0xb5, 0xb0, 0xab, 0x11, 0x6b, 0x24, 0x54, 0x34, 0x80, 0xac, 0xcd, 0x78, 0xaa, 
    0xcc, 0xbd, 0xbc, 0x20, 0x6d, 0xd6, 0x22, 0xc6, 0xd1, 0x2c, 0xf9, 0x00, 0xd0, 0x8a, 0x91, 0xfb, 
    0x79, 0xc9, 0x96, 0x63, 0x15, 0x04, 0x9a, 0x17, 0x80, 0xbe, 0x1c, 0xea, 0xb2, 0x95, 0x9c, 0x09, 
    0x72, 0x84, 0xd2, 0x06, 0x2a, 0x41, 0x59, 0x69, 0xdd, 0x87, 0xc9, 0x0e, 0x60, 0x4e, 0xcd, 0x77, 
    0x47, 0xfa, 0x83, 0xc8, 0x5a, 0xf3, 0x74, 0x1f, 0xb6, 0x85, 0x15, 0xbc, 0xd4, 0xd6, 0xed, 0x74, 
    0x3f, 0x44, 0x49, 0xcd, 0xb3, 0xe7, 0xe8, 0x95, 0x8e, 0xa1, 0x9a, 0xa7, 0xf9, 0x2c, 0x4d, 0xbb, 
    0x09, 0x07, 0x8c, 0xdd, 0xe4, 0x5f, 0xd5, 0xe4, 0x01, 0x6a, 0x79, 0x90, 0x40, 0xcc, 0xf6, 0x70, 
    0xb7, 0xd3, 0x5a, 0x59, 0xc8, 0x4a, 0x7b, 0x7c, 0x59, 0xf8, 0x31, 0x91, 0x84, 0x90, 0x60, 0x3c
};

static const uint8_t keygen_dsa_2048_priv[] = {
    0x5c, 0x6f, 0xce, 0x92, 0x2c, 0x2e, 0xc5, 0xd9, 0x01, 0xe7, 0x05, 0x66, 0xa4, 0x34, 0xf0,
    0xc1, 0xc9, 0x9c, 0x52, 0x4d, 0x9d, 0x01, 0x5a, 0xd2, 0x11, 0x20, 0x4f, 0x12, 0x68, 0x07,
    0x79, 0xd6
};

static const uint8_t keygen_dsa512_p[] = {
	0xC8, 0x6B, 0xB7, 0x91, 0xD6, 0x63, 0xCE, 0xC0, 0xC6, 0xB8, 0xAC, 0x5B,
	0xEB, 0xA7, 0xEF, 0x17, 0xBE, 0x1A, 0x1A, 0x36, 0x6B, 0x38, 0x40, 0x0E,
	0x69, 0x13, 0x32, 0xD4, 0x4B, 0xBE, 0x00, 0xB5, 0x29, 0x7F, 0x6B, 0x87,
	0xAA, 0x1D, 0x98, 0x37, 0xD2, 0xAC, 0x62, 0x26, 0xD7, 0xFD, 0xE1, 0xC9,
	0x13, 0x4F, 0x2A, 0xF2, 0x82, 0xEC, 0xA8, 0x83, 0x6F, 0x29, 0xD3, 0xF5,
	0x16, 0xB9, 0x13, 0xCD,
};

static const uint8_t keygen_dsa512_q[] = {
	0x8D, 0xF9, 0x8B, 0x8A, 0xDA, 0x3B, 0x0B, 0x1C, 0xFA, 0x1C, 0xA7, 0xE8,
	0x9A, 0xA2, 0xD7, 0xC3, 0x2D, 0xD5, 0x9D, 0x1B,
};

static const uint8_t keygen_dsa512_g[] = {
	0xB3, 0xE2, 0xFD, 0x38, 0xE0, 0x9A, 0x21, 0x64, 0x8F, 0x6D, 0x7E, 0x4F,
	0xC2, 0x24, 0x18, 0x88, 0xEC, 0xA4, 0xCB, 0xB0, 0x5F, 0x43, 0xD8, 0x2B,
	0x5B, 0xDE, 0x01, 0xB4, 0xD2, 0x24, 0x1F, 0x80, 0xE7, 0xFC, 0xF3, 0x15,
	0xFA, 0x0C, 0x5B, 0x6F, 0x81, 0x55, 0x8C, 0x80, 0x36, 0xFB, 0x4D, 0xB5,
	0x8C, 0x5A, 0x26, 0xBE, 0xFB, 0x78, 0xEA, 0x62, 0x6A, 0x9D, 0x5E, 0xD0,
	0x21, 0x0C, 0xD9, 0x4E,
};

static const uint8_t dsa_data_ptx_data[] = {
    0xe8, 0x31, 0x27, 0x42, 0xae, 0x23, 0xc4, 0x56, 0xef, 0x28, 0xa2, 0x31, 0x42, 0xc4, 0x49, 0x08, 
    0x95, 0x83, 0x27, 0x65, 0xda, 0xdc, 0xe0, 0x2a, 0xfe, 0x5b, 0xe5, 0xd3, 0x1b, 0x00, 0x48, 0xfb, 
    0xee, 0xe2, 0xcf, 0x21, 0x8b, 0x17, 0x47, 0xad, 0x4f, 0xd8, 0x1a, 0x2e, 0x17, 0xe1, 0x24, 0xe6, 
    0xaf, 0x17, 0xc3, 0x88, 0x8e, 0x6d, 0x2d, 0x40, 0xc0, 0x08, 0x07, 0xf4, 0x23, 0xa2, 0x33, 0xca, 
    0xd6, 0x2c, 0xe9, 0xea, 0xef, 0xb7, 0x09, 0x85, 0x6c, 0x94, 0xaf, 0x16, 0x6d, 0xba, 0x08, 0xe7, 
    0xa0, 0x69, 0x65, 0xd7, 0xfc, 0x0d, 0x8e, 0x5c, 0xb2, 0x65, 0x59, 0xc4, 0x60, 0xe4, 0x7b, 0xc0, 
    0x88, 0x58, 0x9d, 0x22, 0x42, 0xc9, 0xb3, 0xe6, 0x2d, 0xa4, 0x89, 0x6f, 0xab, 0x19, 0x9e, 0x14, 
    0x4e, 0xc1, 0x36, 0xdb, 0x8d, 0x84, 0xab, 0x84, 0xbc, 0xba, 0x04, 0xca, 0x3b, 0x90, 0xc8, 0xe5,
    0xe8, 0x31, 0x27, 0x42, 0xae, 0x23, 0xc4, 0x56, 0xef, 0x28, 0xa2, 0x31, 0x42, 0xc4, 0x49, 0x08, 
    0x95, 0x83, 0x27, 0x65, 0xda, 0xdc, 0xe0, 0x2a, 0xfe, 0x5b, 0xe5, 0xd3, 0x1b, 0x00, 0x48, 0xfb, 
    0xee, 0xe2, 0xcf, 0x21, 0x8b, 0x17, 0x47, 0xad, 0x4f, 0xd8, 0x1a, 0x2e, 0x17, 0xe1, 0x24, 0xe6, 
    0xaf, 0x17, 0xc3, 0x88, 0x8e, 0x6d, 0x2d, 0x40, 0xc0, 0x08, 0x07, 0xf4, 0x23, 0xa2, 0x33, 0xca, 
    0xd6, 0x2c, 0xe9, 0xea, 0xef, 0xb7, 0x09, 0x85, 0x6c, 0x94, 0xaf, 0x16, 0x6d, 0xba, 0x08, 0xe7, 
    0xa0, 0x69, 0x65, 0xd7, 0xfc, 0x0d, 0x8e, 0x5c, 0xb2, 0x65, 0x59, 0xc4, 0x60, 0xe4, 0x7b, 0xc0, 
    0x88, 0x58, 0x9d, 0x22, 0x42, 0xc9, 0xb3, 0xe6, 0x2d, 0xa4, 0x89, 0x6f, 0xab, 0x19, 0x9e, 0x14, 
    0x4e, 0xc1, 0x36, 0xdb, 0x8d, 0x84, 0xab, 0x84, 0xbc, 0xba, 0x04, 0xca, 0x3b, 0x90, 0xc8, 0xe5,
    0xe8, 0x31, 0x27, 0x42, 0xae, 0x23, 0xc4, 0x56, 0xef, 0x28, 0xa2, 0x31, 0x42, 0xc4, 0x49, 0x08, 
    0x95, 0x83, 0x27, 0x65, 0xda, 0xdc, 0xe0, 0x2a, 0xfe, 0x5b, 0xe5, 0xd3, 0x1b, 0x00, 0x48, 0xfb, 
    0xee, 0xe2, 0xcf, 0x21, 0x8b, 0x17, 0x47, 0xad, 0x4f, 0xd8, 0x1a, 0x2e, 0x17, 0xe1, 0x24, 0xe6, 
    0xaf, 0x17, 0xc3, 0x88, 0x8e, 0x6d, 0x2d, 0x40, 0xc0, 0x08, 0x07, 0xf4, 0x23, 0xa2, 0x33, 0xca, 
    0xd6, 0x2c, 0xe9, 0xea, 0xef, 0xb7, 0x09, 0x85, 0x6c, 0x94, 0xaf, 0x16, 0x6d, 0xba, 0x08, 0xe7, 
    0xa0, 0x69, 0x65, 0xd7, 0xfc, 0x0d, 0x8e, 0x5c, 0xb2, 0x65, 0x59, 0xc4, 0x60, 0xe4, 0x7b, 0xc0, 
    0x88, 0x58, 0x9d, 0x22, 0x42, 0xc9, 0xb3, 0xe6, 0x2d, 0xa4, 0x89, 0x6f, 0xab, 0x19, 0x9e, 0x14, 
    0x4e, 0xc1, 0x36, 0xdb, 0x8d, 0x84, 0xab, 0x84, 0xbc, 0xba, 0x04, 0xca, 0x3b, 0x90, 0xc8, 0xe5,
    0xe8, 0x31, 0x27, 0x42, 0xae, 0x23, 0xc4, 0x56, 0xef, 0x28, 0xa2, 0x31, 0x42, 0xc4, 0x49, 0x08, 
    0x95, 0x83, 0x27, 0x65, 0xda, 0xdc, 0xe0, 0x2a, 0xfe, 0x5b, 0xe5, 0xd3, 0x1b, 0x00, 0x48, 0xfb, 
    0xee, 0xe2, 0xcf, 0x21, 0x8b, 0x17, 0x47, 0xad, 0x4f, 0xd8, 0x1a, 0x2e, 0x17, 0xe1, 0x24, 0xe6, 
    0xaf, 0x17, 0xc3, 0x88, 0x8e, 0x6d, 0x2d, 0x40, 0xc0, 0x08, 0x07, 0xf4, 0x23, 0xa2, 0x33, 0xca, 
    0xd6, 0x2c, 0xe9, 0xea, 0xef, 0xb7, 0x09, 0x85, 0x6c, 0x94, 0xaf, 0x16, 0x6d, 0xba, 0x08, 0xe7, 
    0xa0, 0x69, 0x65, 0xd7, 0xfc, 0x0d, 0x8e, 0x5c, 0xb2, 0x65, 0x59, 0xc4, 0x60, 0xe4, 0x7b, 0xc0, 
    0x88, 0x58, 0x9d, 0x22, 0x42, 0xc9, 0xb3, 0xe6, 0x2d, 0xa4, 0x89, 0x6f, 0xab, 0x19, 0x9e, 0x14, 
    0x4e, 0xc1, 0x36, 0xdb, 0x8d, 0x84, 0xab, 0x84, 0xbc, 0xba, 0x04, 0xca, 0x3b, 0x90, 0xc8, 0xe5,
};

static const uint8_t ac_dsa_vect_priv_val[] = {
/* X */
	0x55, 0x0c, 0x87, 0x55, 0x23, 0x78, 0x57, 0xa0, 0xc8, 0xfc, 0x8a, 0x63,
	0x52, 0x5d, 0x40, 0x25, 0x71, 0x3b, 0x89, 0xbd, 0xb1, 0x27, 0xd1, 0xc3,
	0x30, 0xc3, 0x32, 0x4a
};
static const uint8_t ac_dsa_vect_pub_val[] = {
/* Y */
	0x64, 0xb5, 0x88, 0x49, 0x9c, 0x9d, 0xb3, 0xe5, 0x86, 0x41, 0x92, 0x46,
	0x4d, 0x32, 0xfa, 0x35, 0x47, 0xf6, 0x48, 0xfe, 0x67, 0x6c, 0x15, 0x0a,
	0x8f, 0x9e, 0x15, 0x3c, 0x5a, 0xf5, 0x79, 0x25, 0xc7, 0x6d, 0xda, 0x4b,
	0x41, 0x9d, 0x60, 0xb2, 0x2f, 0xa5, 0xcd, 0xea, 0x0f, 0xb6, 0xf0, 0xb8,
	0x47, 0x9c, 0x98, 0x8a, 0x32, 0x4d, 0x27, 0x5b, 0xd4, 0x2e, 0xf1, 0x0d,
	0x89, 0x98, 0xc3, 0x60, 0x39, 0xeb, 0x40, 0x21, 0xfc, 0x0d, 0x27, 0x88,
	0xb5, 0x9a, 0x75, 0xcf, 0x25, 0xed, 0x6e, 0xe4, 0xd4, 0x48, 0x82, 0xb0,
	0xc5, 0xc5, 0xcb, 0x8d, 0xcc, 0x10, 0x02, 0xc0, 0xba, 0xa4, 0x79, 0x81,
	0x07, 0xe0, 0xb5, 0x7c, 0xd2, 0x6d, 0xeb, 0xbc, 0xd0, 0xba, 0x41, 0xd1,
	0xb1, 0xb8, 0x60, 0xb8, 0xeb, 0x90, 0xf6, 0xf3, 0x05, 0x00, 0xb2, 0xe4,
	0xbe, 0x7a, 0x00, 0xb6, 0x7d, 0x93, 0xc8, 0x7d, 0x3f, 0xf7, 0xa6, 0xce,
	0x53, 0xb9, 0x77, 0xa9, 0x30, 0x99, 0x98, 0x07, 0xfc, 0xbe, 0xf5, 0x7d,
	0x8d, 0xc6, 0x7a, 0x8f, 0x36, 0x61, 0x24, 0x99, 0x13, 0x89, 0x32, 0x8c,
	0xe7, 0xe7, 0x0f, 0x9e, 0x5c, 0x22, 0xff, 0xde, 0xdb, 0x28, 0x49, 0x82,
	0x82, 0xb4, 0xa9, 0xa9, 0xc6, 0x85, 0x34, 0xa2, 0x38, 0x32, 0x2e, 0x0d,
	0xb6, 0x08, 0x8e, 0xd0, 0xaf, 0xa8, 0xbc, 0x77, 0xce, 0x99, 0x8c, 0x81,
	0x44, 0x71, 0xab, 0x56, 0x76, 0x7b, 0x35, 0xd0, 0x7b, 0x94, 0x29, 0x0e,
	0xa1, 0x06, 0xff, 0x0c, 0x99, 0x8b, 0x51, 0xf0, 0x22, 0x22, 0x73, 0x8e,
	0xf9, 0x30, 0x1f, 0x29, 0x0c, 0x6b, 0x48, 0x5d, 0xbc, 0x4f, 0x12, 0xb4,
	0x72, 0xa1, 0x19, 0x2f, 0xd9, 0x3f, 0x2d, 0x23, 0x52, 0x7a, 0x02, 0xd9,
	0x5a, 0xf0, 0xb4, 0x22, 0xbe, 0x76, 0x40, 0xa9, 0x70, 0x2e, 0xca, 0xac,
	0x26, 0xc9, 0xe0, 0x04
};


#endif /* __DSA__DATA_H__ */