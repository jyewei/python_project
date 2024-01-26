/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include "iodefine.h"


#define IOSTREAM 20
#define STDIN  0                    /* Standard input (console)        */
#define STDOUT 1                    /* Standard output (console)       */
#define STDERR 2                    /* Standard error output (console) */

#define FLMIN  0                    /* Minimum file number     */
#define _MOPENR 0x1
#define _MOPENW 0x2
#define _MOPENA 0x4
#define _MNBF   0x800
#define O_RDONLY 0x0001 /* Read only                                       */
#define O_WRONLY 0x0002 /* Write only                                      */
#define O_RDWR   0x0004 /* Both read and Write                             */
#define CR 0x0d                     /* Carriage return */
#define LF 0x0a                     /* Line feed       */

const long _nfiles = IOSTREAM; /* The number of files for input/output files */
char flmod[IOSTREAM];          /* The location for the mode of opened file.  */
unsigned char sml_buf[IOSTREAM];

#define FPATH_STDIN     "C:\\stdin"
#define FPATH_STDOUT    "C:\\stdout"
#define FPATH_STDERR    "C:\\stderr"
#define     INTERNAL_NOT_USED(p)    ((void)(p))

FILE *_Files[IOSTREAM]; // structure for FILE



/***********************************************************************************************************************
* Function Name: charput
* Description  : puts a character on a serial port
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void charput (unsigned char output_char)
{
    while(!SCI1.SSR.BIT.TDRE);
    SCI1.TDR = output_char;	
}


/***********************************************************************************************************************
* Function Name: charget
* Description  : Gets a character on a serial port
* Arguments    : none
* Return Value : received character
***********************************************************************************************************************/
unsigned char charget (void)
{
    unsigned char temp;
	
	temp = SCI1.RDR;
    IR(SCI1, RXI1) = 0;
	while(IR(SCI1, RXI1) == 0);
	temp = SCI1.RDR;
	if(temp == 0x0d)
	{
		charput('\r');
		charput('\n');
	}
	else
	{
		charput(temp);
	}
	return temp;	
}

/****************************************************************************/
/* _INIT_IOLIB                                                              */
/*  Initialize C library Functions, if necessary.                           */
/*  Define USES_SIMIO on Assembler Option.                                  */
/****************************************************************************/
void _INIT_IOLIB( void )
{
    /* Standard Input File                                                  */
    if( freopen( FPATH_STDIN, "r", stdin ) == NULL )
    {
        stdin->_Mode = 0xffff;  /* Not allow the access if it fails to open */
    }
    stdin->_Mode  = _MOPENR;            /* Read only attribute              */
    stdin->_Mode |= _MNBF;              /* Non-buffering for data           */
    stdin->_Bend = stdin->_Buf + 1;  /* Re-set pointer to the end of buffer */

    /* Standard Output File                                                 */
    if( freopen( FPATH_STDOUT, "w", stdout ) == NULL ) 
    {
        stdout->_Mode = 0xffff; /* Not allow the access if it fails to open */
    }
    stdout->_Mode |= _MNBF;             /* Non-buffering for data           */
    stdout->_Bend = stdout->_Buf + 1;/* Re-set pointer to the end of buffer */
    
    /* Standard Error File                                                  */
    if( freopen( FPATH_STDERR, "w", stderr ) == NULL )
    {
        stderr->_Mode = 0xffff; /* Not allow the access if it fails to open */
    }
    stderr->_Mode |= _MNBF;             /* Non-buffering for data           */
    stderr->_Bend = stderr->_Buf + 1;/* Re-set pointer to the end of buffer */
}

/****************************************************************************/
/* _CLOSEALL                                                                */
/****************************************************************************/
void _CLOSEALL( void )
{
    long i;

    for( i=0; i < _nfiles; i++ )
    {
        /* Checks if the file is opened or not                               */
        if( _Files[i]->_Mode & (_MOPENR | _MOPENW | _MOPENA ) )
        {
            fclose( _Files[i] );    /* Closes the file                       */
        }
    }
}

/**************************************************************************/
/*       open:file open                                                   */
/*          Return value:File number (Pass)                               */
/*                       -1          (Failure)                            */
/**************************************************************************/
long open(const char *name, long  mode, long  flg)
{
    /* This code is only used to remove compiler info messages about these parameters not being used. */
    INTERNAL_NOT_USED(flg);

    if( strcmp( name, FPATH_STDIN ) == 0 )      /* Standard Input file?   */
    {
        if( ( mode & O_RDONLY ) == 0 )
        { 
            return -1;
        }
        flmod[STDIN] = (char)mode;
        return STDIN;
    }
    else if( strcmp( name, FPATH_STDOUT ) == 0 )/* Standard Output file?  */
    {
        if( ( mode & O_WRONLY ) == 0 )
        { 
            return -1;
        }
        flmod[STDOUT] = (char)mode;
        return STDOUT;
    }
    else if(strcmp(name, FPATH_STDERR ) == 0 )  /* Standard Error file?   */
    {
        if( ( mode & O_WRONLY ) == 0 ) 
        {
            return -1;
        }
        flmod[STDERR] = (char)mode;
        return STDERR;
    }
    else 
    {
        return -1;                             /*Others                  */
    }
}

long close( long fileno )
{
    /* This code is only used to remove compiler info messages about these parameters not being used. */
    INTERNAL_NOT_USED(fileno);

    return 1;
}

/**************************************************************************/
/* write:Data write                                                       */
/*  Return value:Number of write characters (Pass)                        */
/*               -1                         (Failure)                     */
/**************************************************************************/
long write(long  fileno,const unsigned char *buf,long  count)
{
    long    i;                          /* A variable for counter         */
    unsigned char    c;                 /* An output character            */

    if((flmod[fileno]&O_WRONLY) || (flmod[fileno]&O_RDWR))
    {
        if( fileno == STDIN ) 
        {
            return -1;            /* Standard Input     */
        }
        else if( (fileno == STDOUT) || (fileno == STDERR) ) /* Standard Error/output   */
        {
//put time first
            // for ( i = 0; i < sizeof(TimeType)/sizeof(char); i++)
            // {
            //     c = *((unsigned char *)(time.ms+i));
            //     charput(c);
            // }
            
            for( i = count; i > 0; --i )
            {
                c = *buf++;
                charput(c);
            }
            return count;        /*Return the number of written characters */
        }
        else 
        {
            return -1;                  /* Incorrect file number          */
        }
    }
    else 
    {
        return -1;                      /* An error                       */
    }
}

long read( long fileno, unsigned char *buf, long count )
{
       long i;

       if((flmod[fileno]&_MOPENR) || (flmod[fileno]&O_RDWR))
       {
             for(i = count; i > 0; i--)
             {
                   *buf = charget();
                   if(*buf==CR)/* Replace the new line character */
                   {
                         *buf = LF;
                   }
                   buf++;
             }
             return count;
       }
       else 
       {
             return -1;
       }
}

long lseek( long fileno, long offset, long base )
{
    INTERNAL_NOT_USED(fileno);
    INTERNAL_NOT_USED(offset);
    INTERNAL_NOT_USED(base);

    return -1L;
}

