#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);          //sizeof(int) = 4,store 4 bytes after 18th(18,19,20,21 stores width info) byte store in width
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);         //sizeof(int) = 4,store 4 bytes from 22th(22,23,24,25 stores height info) byte store in height
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;       //3 is multipled because of RGB
}
uint get_file_size(FILE *fptr)
{
	fseek(fptr,-1,SEEK_END);
	int pos = ftell(fptr);
	rewind(fptr);
	return pos;
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	if(strcmp(strchr(argv[2],'.'),".bmp") == 0)    //check whether the input file is of bmp
	{

	unsigned char arr1[2],arr2[2];
	encInfo->fptr_src_image = fopen(argv[2],"r");  //open the file and fptr_src_image ppints to that file
	fread(arr1,1,2,(encInfo->fptr_src_image));      //read 2 bytes
	if((arr1[0]==0x42) && (arr1[1] == 0x4d))          //check oth and 1st bytes are 0x42 and 0x4d
	{
		printf("the file is .bmp\n");
		encInfo->src_image_fname = argv[2];
	}
	else
	{
		printf("the file is not .bmp\n");
		fclose(encInfo->fptr_src_image);
		return e_failure;
	}
	fclose(encInfo->fptr_src_image);

	encInfo->secret_fname = argv[3];         //passinf secret file name
	strcpy(encInfo->extn_secret_file,strchr(argv[3],'.'));
	encInfo->sec_file_extn_size = strlen(encInfo->extn_secret_file);
	printf("secret file extension is %s\n",encInfo->extn_secret_file);  //passing secret file extension
	}
	else
	{
		printf("the file is not .bmp\n");
		return e_failure;
	}
	if(argv[4]!=NULL)  //checking output file is passed or not
	{
		if(strcmp(strchr(argv[4],'.'),".bmp") == 0) //if passed ,whether it is bmp or not
		{
		
			printf("Output file is .bmp file\n");
			encInfo->stego_image_fname = argv[4];
		}
		else
		{
			printf("output file is not .bmp\n");
			return e_failure;
		}
		}
		else
		{
			encInfo->stego_image_fname = "stego.bmp";
			printf("%s\n",encInfo->stego_image_fname);
		}
	
		return e_success;  //if all validation passed return success

}


Status check_capacity(EncodeInfo *encInfo) //checking capacity
{
        printf("checking capacity of %s and %s\n",encInfo->src_image_fname,encInfo -> secret_fname);
	encInfo-> image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
	//printf("the capacity of image is %d\n",encInfo->image_capacity);
	 
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
	printf("The size of the secret file is %ld byte\n",(strlen(MAGIC_STRING) * 8 )+32+(strlen(encInfo->extn_secret_file) * 8)+32+((encInfo->size_secret_file) * 8)+54) ; //checking capacity formula
 
	if(((strlen(MAGIC_STRING) * 8 )+32+(strlen(encInfo->extn_secret_file) * 8)+32+((encInfo->size_secret_file) * 8)+54)>=encInfo->image_capacity) //comapring size of image and size of secret text
	{
		printf("ERROR : Secret message size is more than image size\n");
		return e_failure;
	}
	else
	{
		printf("Secret message size is less than image size\n");
	}

	
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image) //to copy header data from source image to destination image
{
	rewind(fptr_src_image); //starting position
	rewind(fptr_dest_image); //offset to starting position
	char arr[54]; 
	fread(arr,sizeof(char),54,fptr_src_image); //reading header data of size 54 bytes from source 
	fwrite(arr,sizeof(char),54,fptr_dest_image); //copying it to destiantion
	return e_success;
}



Status encode_magic_string(char *magic_string, EncodeInfo *encInfo) // magic string encoding function defn
{
	int size = strlen(magic_string); // find the length of magic string
	encode_data_to_image(magic_string,size,encInfo->fptr_src_image,encInfo->fptr_stego_image);
	return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo) //encode secret file extn size
{	
	 char arr2[32];
        fread(arr2,32,1,encInfo->fptr_src_image); //read 32 bits from source image
        encode_size_to_lsb(size,arr2); //pass it to encode size to lsb
        fwrite(arr2,32,1,encInfo->fptr_stego_image); //write to stego file
	return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image) //encode data to image function defination
{
	char arr1[8];
	for(int i = 0;i<size; i++)
	{
		fread(arr1,8,sizeof(char),fptr_src_image); //read 8 bytes from source image
		encode_byte_to_lsb(data[i],arr1); // pass it to byte to lsb function where encoding of data in image bytes takes place
		fwrite(arr1,8,sizeof(char),fptr_stego_image); //write it to stego file
	}
	return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer) //encoding the bits in lsb of image
{
	for(int j=0;j<8;j++)
	{
		image_buffer[j] = (image_buffer[j] & (0xFE)) | ((data >> (7-j)) & 1); // encoding byte logic
	}
	return e_success;
}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo) //encode secretfile extn function defination
{
	encode_data_to_image(encInfo->extn_secret_file,strlen(encInfo->extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image); //calling encode date to image functn
	return e_success;
}
Status encode_size_to_lsb(char data, char *image_buffer) //encode file size data to lsb
{
	for(int i=0;i<32;i++)
	{
		image_buffer[i] = ((image_buffer[i]) & (0xFE)) | ((data >> (31-i)) & 1); //storing each bits in lsb position of image bytes
	}
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo) //encoding secret file size function defination
{
	char arr2[32];
	fread(arr2,32,1,encInfo->fptr_src_image); //as size are of int type reading 32 bits of data
	encode_size_to_lsb(file_size,arr2); // passingthe 32 bits to lsb to encodeit in 32 bytes of image bytes
	fwrite(arr2,32,1,encInfo->fptr_stego_image); //writing it to stego file
	printf("Loading suucessful\n");
}


Status encode_secret_file_data(EncodeInfo *encInfo) // encoding secret file data function defination
{
	long size = encInfo->size_secret_file; //size of secret file
       	char secret_data[encInfo->size_secret_file];
	char ch;
	int i=0;
	while((ch = fgetc(encInfo->fptr_secret))!=EOF) //getting each data character by character
	{	
		secret_data[i++] = ch; //store it in secret data
	}
	secret_data[i]='\0';
       	encode_data_to_image(secret_data,strlen(secret_data),encInfo->fptr_src_image,encInfo->fptr_stego_image); //pass each character data to encode data to image
        return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest) //copying remaining bytes
{
	char ch;
	while(fread(&ch,1,1,fptr_src)>0)
	{
		fwrite(&ch,1,1,fptr_dest);
	}
	return e_success;
}

Status do_encoding(EncodeInfo *encInfo) //do encoding
{
{

	printf("----Encoding process started----\n");
}
if(open_files(encInfo) == e_failure)
{
	printf("ERROR : Opening file issue\n");
	return e_failure;
}
printf("---Checking %s file size---\n",encInfo->secret_fname);
{
	if(check_capacity(encInfo) == 0)
	{
		printf("ERROR : Insuffient capacity\n");
		return e_failure;
	}
	else
	{
		printf("Suffient image capacity\n");
	}
}
if(copy_bmp_header(encInfo-> fptr_src_image, encInfo ->fptr_stego_image) == e_failure)
{
	printf("ERROR : Data from source file to stego file not copied\n");
	return e_failure;
}
printf("Data from the source file to stego file copied successfully\n");

if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
{
	printf("ERROR :Data from source file to stego file not copied\n");
	return e_failure;
}
printf("Data from source file to stego file copied successfully\n");

if(encode_secret_file_extn_size(encInfo->sec_file_extn_size,encInfo) == e_failure)
{
         printf("ERROE : Failed in encode secret file size\n");
        return e_failure;
}

if(encode_secret_file_extn((encInfo->extn_secret_file),encInfo) == e_failure)
{
	return e_failure;
}

if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_failure)
{
         printf("ERROR : Failed in encode secret file size\n");
        return e_failure;
}


if(encode_secret_file_data(encInfo) == e_failure)
{
	return e_failure;
}


if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
{
	return e_failure;
}
printf("Remaining data encoded successfully\n");
return e_success;
}
