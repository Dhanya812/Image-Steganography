#ifndef DECODE_H
#define DECODE_H

#include "types.h"

/*Structure to store the information required For decoding */
typedef struct _DecodeInfo
{

	/*Input Image info */
	char *inp_image_fname;
	FILE *fptr_inp_image;

	/*Secret file Info */
	long sec_file_extn_size;
	long sec_file_size;
	char sec_file_extn[20];

	/*Output File Info */

	char out_fname[10];
	FILE *fptr_output;
	char output_fname[20];

} DecodeInfo;

/* Decoding function prototype */

/*Read and validate Decode args from argv*/

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*Perform the decoding*/

Status do_decoding(DecodeInfo *decInfo);

/* Get file pointers for i/p and o/p files */

Status open__files(DecodeInfo *decInfo);

/*Decode magic string*/

Status decode_magic_string(DecodeInfo *decInfo);

/*Decode data from image*/

Status decode_data_from_image(int size, FILE *fptr_inp_image, char *data);
 
/*Decode secret file extension size */
 
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode size from the lsb */

long decode_size_from_lsb(char *image_buffer);

/* Decode secret file extension */

Status decode_secret_file_extn(DecodeInfo *decInfo);

/*Decode secret file size */

Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret data*/

Status decode_secret_data(DecodeInfo *decInfo);
#endif

