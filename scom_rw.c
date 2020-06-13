/*
Copyright (c) 2014 Studer Innotec SA

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/**
 * \file usage_examples.c examples of usage of the scom library
 */

#include <stdio.h>
#include <string.h>
#include <scom_property.h>

#include <inttypes.h>

#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>

#define BAUDRATE B38400
#define TTYDEVICE "/dev/ttyUSB0"

static int fd;

/**
 * \brief configure the serial port
 * \return zero in case of an error
 */
int initialize_serial_port(char * device) {

    struct termios serial_port_settings;
    int error = 0;
        
    fd = open(TTYDEVICE, O_RDWR);
	if (fd < 0) {
		perror("Failed to open SERIAL_DEVICE");
		return 0;
	}
	
	error = tcgetattr(fd, &serial_port_settings);
	if (error < 0) {
		perror("Failed to get termios structure");
		return 0;
	}

	//setting baud rate to B38400
	error = cfsetospeed(&serial_port_settings, BAUDRATE);
	if (error < 0) {
		perror("Failed to set 38400 output baud rate");
		return 0;
	}
	error = cfsetispeed(&serial_port_settings, BAUDRATE);
	if (error < 0) {
		perror("Failed to set 38400 input baud rate");
		return 0;
	}
	//Disable ECHO
    serial_port_settings.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD | PARENB ;
    serial_port_settings.c_iflag &= ~(IXON | IXANY | IXOFF);
	serial_port_settings.c_lflag &= ~(ICANON);
	serial_port_settings.c_lflag &= ~(ECHO | ECHOE);
    serial_port_settings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    serial_port_settings.c_oflag &= ~OPOST;
    serial_port_settings.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    serial_port_settings.c_cflag &= ~(CSIZE);
    serial_port_settings.c_cflag |= CS8;
	serial_port_settings.c_cc[VMIN] = 14;
	serial_port_settings.c_cc[VTIME] = 50; 
	error = tcsetattr(fd, TCSANOW, &serial_port_settings);
	if (error < 0) {
		perror("Failed to set serial attributes");
		return 0;
	}

	return 1;
}

/**
 * \brief empty the rx and tx buffer before a new exchange
 */
int clear_serial_port() {

		 usleep(1000);
         return tcflush(fd,TCIOFLUSH);
}

/**
 * \brief read in a blocking way on the serial port
 *
 * This function must implement the proper timeout mechanism.
 * * \return number of byte read
 */
size_t read_serial_port(char* buffer, size_t byte_count) {

   size_t numRead = 0, tmpRead;

   while (1) {
				
		 tmpRead = read(fd,buffer,byte_count-numRead);

		 if (tmpRead != (size_t)-1)  {
		    numRead += tmpRead;
		 } else {
		    return 0;
		 }

		 if (numRead >= byte_count) { 
			 return numRead; }
                 buffer += numRead;
		 usleep (50000);
   }
}

/**
 * \brief write in a blocking way on the serial port
 *
 * This function must implement the proper timeout mechanism.
 * \return number of byte written
 */
size_t write_serial_port(const char* buffer, size_t byte_count) {

  return write(fd,buffer,byte_count);
}
/**
 * \brief close the serial port even if initialize_serial_port() failed
 */
void close_serial_port() {
	  close(fd);
}


/*  ----------------------examples function definitions ---------------------- */
scom_error_t exchange_frame(scom_frame_t* frame);


int main(int argc, char *argv[])
{

	/*char * device = (argc>2)?argv[2]:TTYDEVICE;
    int val = (argc>1)?atoi(argv[1]):3000;

    if(initialize_serial_port(device) != 0) {
        ///read_xt1(val);
    }
    else {
        printf("error in the serial port initialisation\n");
    }

    close_serial_port();
    */

    return 0;
}

int32_t _readValue(uint32_t p_oid, uint8_t * inBuffer, uint32_t inBufferSize, uint32_t destination);

int32_t readValue (uint32_t p_oid, uint8_t ** buffer, uint32_t bufferSize,uint32_t destination) {

   return _readValue(p_oid,*buffer, bufferSize,destination);

}

/**
 * \brief example code to read the battery voltage on Xtender 1 and print it to the standard output
 */
int32_t _readValue(uint32_t p_oid, uint8_t * inBuffer, uint32_t inBufferSize, uint32_t destination)  {

    scom_frame_t frame;
    scom_property_t property;
    char buffer[1024];

    int32_t error = 0;

    /* initialize the structures */
    scom_initialize_frame(&frame, buffer, sizeof(buffer));
    scom_initialize_property(&property, &frame);

    frame.src_addr = 1;     /* my address, could be anything */
    frame.dst_addr = destination; // 101;   /* the first inverter */

    property.object_type = SCOM_USER_INFO_OBJECT_TYPE;  /* read a user info */
    property.object_id = p_oid;  /* the battery voltage */

    scom_encode_read_property(&property);

    if(frame.last_error != SCOM_ERROR_NO_ERROR) {
        printf("read property frame encoding failed with error %d\n", (int) frame.last_error);
        error = -1;
    }

    /* do the exchange of frames */
    if(exchange_frame(&frame)!= SCOM_ERROR_NO_ERROR) {
        error = -2;
    }


    /* reuse the structure to save space */
    scom_initialize_property(&property, &frame);



    /* decode the read property service part */
    scom_decode_read_property(&property);
    if(frame.last_error != SCOM_ERROR_NO_ERROR) {
        printf("read property decoding failed with error %d\n", (int) frame.last_error);
        error = -3;
    }



    if (!error) {
        if (property.value_length <= inBufferSize) {
            memcpy(inBuffer,property.value_buffer,property.value_length);
            return property.value_length;
        } else {
            error = -4;
        }
    }
    
   return error;

}

uint32_t writeUnsavedValue (uint32_t p_oid, uint32_t value, uint32_t intSize, uint32_t destination)
{

    scom_frame_t frame;
    scom_property_t property;
    char buffer[1024];

    /* initialize the structures */
    scom_initialize_frame(&frame, buffer, sizeof(buffer));
    scom_initialize_property(&property, &frame);

    frame.src_addr = 1;     /* my address, could be anything */
    frame.dst_addr = destination; // 101;   /* the first inverter */

    property.object_type = SCOM_PARAMETER_OBJECT_TYPE;  /* read a user info */
    property.object_id = p_oid;  /* the battery voltage */
    property.property_id = 0x000D; // unsaved_value_qsp
    property.value_buffer_size = 1024;
    property.value_length = intSize;

    memcpy (property.value_buffer,&value, intSize);

    scom_encode_write_property(&property);

    if(frame.last_error != SCOM_ERROR_NO_ERROR) {
        printf("write property frame encoding failed with error %d\n", (int) frame.last_error);
        return 2;
    }

    /* do the exchange of frames */
    if(exchange_frame(&frame)!= SCOM_ERROR_NO_ERROR) {
        return 3;
    }

    /* reuse the structure to save space */
    scom_initialize_property(&property, &frame);

    /* decode the write property service part */
    scom_decode_write_property(&property);
    if(frame.last_error != SCOM_ERROR_NO_ERROR) {
        printf("write property decoding failed with error %d\n", (int) frame.last_error);
        return 4;
    }

   return 1;

}



/**
 * \brief example code to exchange a frame and print possible error on standard output
 *
 * \param frame an initialized frame configured for a service
 * \return a possible error that occurred or SCOM_NO_ERROR
 */
scom_error_t exchange_frame(scom_frame_t* frame)
{
    size_t byte_count;

    clear_serial_port();

    scom_encode_request_frame(frame);

    if(frame->last_error != SCOM_ERROR_NO_ERROR) {
        printf("data link frame encoding failed with error %d\n", (int) frame->last_error);
        return frame->last_error;
    }

    /* send the request on the com port */

    byte_count = write_serial_port(frame->buffer, scom_frame_length(frame));
    if(byte_count != scom_frame_length(frame)) {
        printf("error when writing to the com port\n");
        return SCOM_ERROR_STACK_PORT_WRITE_FAILED;
    }



    /* reuse the structure to save space */
    scom_initialize_frame(frame, frame->buffer, frame->buffer_size);

    /* clear the buffer for debug purpose */
    memset(frame->buffer,0, frame->buffer_size);

    /* Read the fixed size header.
       A platform specific handling of a timeout mechanism should be
       provided in case of the possible lack of response */



    byte_count = read_serial_port(frame->buffer, SCOM_FRAME_HEADER_SIZE);




    if(byte_count != SCOM_FRAME_HEADER_SIZE) {
        printf("error when reading the header from the com port\n");
        return SCOM_ERROR_STACK_PORT_READ_FAILED;
    }

    /* decode the header */
    scom_decode_frame_header(frame);
    if(frame->last_error != SCOM_ERROR_NO_ERROR) {
        printf("data link header decoding failed with error %d\n", (int) frame->last_error);
        return frame->last_error;
    }

    /* read the data part */

    byte_count = read_serial_port(&frame->buffer[SCOM_FRAME_HEADER_SIZE],  scom_frame_length(frame) - SCOM_FRAME_HEADER_SIZE);
    if(byte_count != (scom_frame_length(frame) - SCOM_FRAME_HEADER_SIZE)) {
        printf("error when reading the data from the com port\n");
        return SCOM_ERROR_STACK_PORT_READ_FAILED;
    }

    /* decode the data */
    scom_decode_frame_data(frame);
    if(frame->last_error != SCOM_ERROR_NO_ERROR) {
        printf("data link data decoding failed with error %d\n", (int) frame->last_error);
        return frame->last_error;
    }

    return SCOM_ERROR_NO_ERROR;

}
