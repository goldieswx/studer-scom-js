const scom = require("studer-scom-js")
const ref = require("ref");

scom.initialize_serial_port("/dev/ttyUSB0");

const buffer = new Buffer(64);


/// Read value at first extender (101)
const valueId = 3000;
const xtenderId = 101;

const response = scom.readValue(valueId,buffer.ref(),buffer.length,xtenderId);

/// Write value to first extender (101). Set AUX2 ON
// const response = scom.writeUnsavedValue(1311,0x00000004,4,xtenderId);

console.log("Vbat : ", buffer.readFloatLE());

scom.close_serial_port();
