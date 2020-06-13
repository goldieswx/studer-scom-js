# studer-scom-js
Javascript wrapper for studer scom using serial port (232i)

Implementation of a studer scom Javascript wrapper. This library uses node-ffi and node-ref. 
Node.Js required (version <= 10). Alternatetively a native port should be possible, 
however using Studer default 'C' implementation is faster.

Usage

*Initialize the serial port* (hard-fixed at 38400, default setting).

`scom.initialize_serial_port("/dev/ttyUSB0");`

*Read value into buffer*

const buffer = new Buffer(64);

/// Read value at first extender (101)
const valueId = 3000;
const xtenderId = 101;

const response = scom.readValue(valueId,buffer.ref(),buffer.length,xtenderId);

console.log("Vbat : ", buffer.readFloatLE());


*Deinitialize serial port.*

`scom.close_serial_port();`
