#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "decode.h"
#include "types.h"
#include "common.h"

Status open__files(DecodeInfo *decInfo) //file open Function definition
{

	decInfo->fptr_inp_image = fopen(decInfo->inp_image_fname, "r"); //open the input image file in read node
	if(decInfo->fptr_inp_image == NULL) //check if the file opening correctly or not
	{
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open files %s\n", decInfo->inp_image_fname);
	return e_failure;

	}

return e_success;

}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo) //read and validate function definition
{
	unsigned char arr[2];
	if(strcmp(strchr(argv[2],'.'),".bmp") != 0) //check the input file is bap or not 
	{
	printf("Error: The source file is not a bmp file\n");
	return e_failure;
	}

	decInfo->fptr_inp_image = fopen(argv[2],"r"); //store that input file in one structure member
	fread(arr, 1, 2, (decInfo->fptr_inp_image)); //if that input file is bmp file read 2 bytes from that file

	if((arr[0] == 'B') && (arr[1] =='M')) //check its first 2 bytes and compare it with sighnature bytes of bap to ensure it is bmp file.

	{
	printf("Source file is a bmp file\n");
	decInfo->inp_image_fname = argv[2]; //store the input file name in one structure member
	}
	fclose(decInfo->fptr_inp_image);

	if(argv[3] != NULL) //check if the output filename is passed or not {
	{
		printf("The output filename is given by the user\n");
		strcpy(decInfo->out_fname, argv[3]); //if passed store it in one structure member
		printf("The Filename given by the user is %s\n", decInfo->out_fname);
	}
	else
	{
		strcpy(decInfo->out_fname, "decoded_file"); //if user dont give output file name take it default and store in one structure member
		printf("The default filename is: %s\n", decInfo->out_fname);
	}
	return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo) //decode magic string function definition
{

	int len = strlen(MAGIC_STRING);
	char str[len+1];
	fseek(decInfo->fptr_inp_image, 54, SEEK_SET); //skip first 54 bytes in input image
	decode_data_from_image(len, decInfo->fptr_inp_image, str); //Function call 
	if(strcmp(MAGIC_STRING, str) == 0) //compare the encoded string with original magic string
		return e_success;
	else
		return e_failure;
	return e_success;
}

Status decode_data_from_image(int size, FILE *fptr_inp_image, char *str) //decode data from image function definition

{
	char arr[8];
	for(int i=0;i<size;i++) //run the loop size times
	{
		fread(arr,8,1,fptr_inp_image); //read 8 bytes from the input image
		unsigned char ch = 0;
		for(int j=0;j<8;j++) //since we used 8 bytes to encode 1 byte, here we have to decode these 8 bytes and get that 1 byte from it
		{
			ch |= ((arr[j] & 1) << (7-j)); //Logic for decoding from Isb and shift to msb since encoding is done from asb
		}
	       	str[i] = ch; //store each character in one string
	}

	str[size] = '\0';
	printf("%s\n",str);
	return e_success;
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo) //decode secret file extn size function definition
{
	char arr[32];
	fread(arr, 32, 1, decInfo->fptr_inp_image); //read 32 bytes from input file
	decInfo->sec_file_extn_size = decode_size_from_lsb(arr); //function call
	return e_success;
}

long decode_size_from_lsb(char *buffer) //decode size from lab function definition
{
	long num = 0;
	for(int i=0;i<32;i++) //since size is a integer data and we used 32 bytes to encode it, so to decode those 32 bits from 32 bytes run loop 32 times
	{
		num |= ((buffer[i] & 1) << (31-i)); //Logic for decoding
	}
	return num;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)//decode secret file extn function definition
{
	char str[decInfo->sec_file_extn_size+1];
	int len = strlen(str);
	decode_data_from_image(decInfo->sec_file_extn_size, decInfo->fptr_inp_image, str); //function call for decoding
	strcpy(decInfo->sec_file_extn, str); // store that secret file extension in one structure member
	return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo) //decode secret file size function definition
{
	char arr[32];
	fread(arr, 1,32, decInfo->fptr_inp_image); //read 32 bytes data from input file
	decInfo->sec_file_size = decode_size_from_lsb(arr);
	return e_success;
}

Status decode_secret_data(DecodeInfo *decInfo) //decode secret data function definition
{
	strcpy(decInfo->output_fname, decInfo->out_fname); //copy the output file name which was give into another structure member
	strcat(decInfo->output_fname, decInfo->sec_file_extn);//concatenate the extn to output file 
	decInfo->fptr_output = fopen(decInfo->output_fname, "w"); //open the output file pointer in write mode

	if(decInfo->fptr_output == NULL) //check whether the file apened correctly or not
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->out_fname);
		return e_failure;
	}
char str[decInfo->sec_file_size+1];
int len = strlen(str);
decode_data_from_image(decInfo->sec_file_size, decInfo->fptr_inp_image, str); //Function call
fwrite(str, decInfo->sec_file_size, 1, decInfo->fptr_output); //write that decoded data into that output file
return e_success;

}

Status do_decoding (DecodeInfo *decInfo) //decoding function
{
	if(open__files(decInfo) == e_success) //function call

	printf("Files opened successfully\n");
	if(decode_magic_string(decInfo) == e_success) //function call

	{
		printf("Magic string decoded successfully\n");
	}
	else
	{
		printf("Magic string decoding failed, So decoding cannot be done\n");
	}
	if(decode_secret_file_extn_size(decInfo) == e_success); //function call
	{
		printf("Secret file extension size decoded successfully\n");
	}
	if(decode_secret_file_extn(decInfo) == e_success) //function call

	{
		printf("Secret file extension decoded successfully\n");
	}

	if(decode_secret_file_size(decInfo) == e_success) //function call
	
	{
		printf("Secret file size decoded successfully\n");
	}

	if(decode_secret_data(decInfo) == e_success) //function call

	{	
		printf("Secret file data decoded successfully\n");
	}

	return e_success;

}

