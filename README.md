# studer-scom-js
Javascript wrapper for studer scom using serial port (232i)

Implementation of a studer scom Javascript wrapper. This library uses node-ffi and node-ref. 
Node.Js required (version <= 10). Alternatetively a native port should be possible, 
however using Studer default 'C' implementation is faster. 
This port is currently limited to writing/reading parameters/properties.

Usage

*Initialize the serial port* (hard-fixed at 38400, default setting).

`scom.initialize_serial_port("/dev/ttyUSB0");`

*Read value into buffer*

```
const buffer = new Buffer(64);

/// Read value at first extender (101)
const valueId = 3000;
const xtenderId = 101;

const response = scom.readValue(valueId,buffer.ref(),buffer.length,xtenderId);

console.log("Vbat : ", buffer.readFloatLE());
```

*Write value into buffer*

```
/// Write value to first extender (101). Set AUX2 ON
const response = scom.writeUnsavedValue(1311,0x00000004,4,xtenderId);
```


*Deinitialize serial port.*

`scom.close_serial_port();`
