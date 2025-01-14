#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char* argv[])
{
    EncodeInfo encInfo;
    int ret = check_operation_type(argv);
    if(ret == e_encode)
    {
	    printf("Selected Encoding\n");
    
    if(argc>=4)
    {
	    int res = read_and_validate_encode_args(argv,&encInfo);
	    {
		    if(res == e_failure)
		    {
			    printf("Read and Validate failed\n");
			    return 0;
		    }
		    else
			    printf("success");
	   
	    }
	    if(do_encoding(&encInfo) == e_failure)
	    {
		    printf("encoding failed\n");
		    return 0;
	    }
	    else
	    {
		    printf("Encoding successful\n");
	    }
    }
    else
	    printf("insuffient arguments\n");
    }
    else if(ret == e_decode)
    {
	    DecodeInfo decInfo;
            printf("Selected Decoding\n");
	    if(argc >=3)
	    {
		    int res = read_and_validate_decode_args(argv,&decInfo);
		    if(res == e_failure)
		    {
			    printf("Read and validate failed and decoding cannot be done.\n");
			    return 0;
		    }
		    else if(res == e_success)
		    {
			    int rett = do_decoding(&decInfo);
			    if(rett == e_failure)
			    {
				    printf("Decoding failed\n");
				    return 0;
			    }
			    else
				    printf("Decoding successful\n");
		    }
    		}
    
	    else
	    {
	    printf("Insuffient arguments to do decoding\n");
	    return 0;
	    }
    }
    else
    {
            printf("Unsupported format\n");
    }
    return 0;

}
    OperationType check_operation_type(char *argv[])
    {
	    if(strcmp(argv[1],"-e")==0)
		    return e_encode;
	    else if(strcmp(argv[1],"-d")==0)
	    	    return e_decode;
	    else
		    return e_unsupported;
    } 

    //return 0;
//}

