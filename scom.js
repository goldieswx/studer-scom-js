const  ffi = require("ffi");
const  ref = require("ref");

const  uint32   = ref.types.uint32;
const  uint8    = ref.types.uint8;
const  _string  = ref.types.CString;
const  _void    = ref.types.void;

var platform = process.platform;
var scomlibLoc = null;

if (platform === "win32") {
  scomlibLoc = "./scom.dll";
} else if (platform === "linux") {
  scomlibLoc = "./build/Release/obj.target/scom.so";
} else if (platform === "darwin") {
  scomlibLoc = "./scom.dylib";
} else {
  throw new Error("unsupported plateform for scomlibLoc");
}


module.exports = ffi.Library(scomlibLoc, {

  // uint32_t writeUnsavedValue (uint32_t p_oid, uint32_t value, uint32_t intSize, uint32_t destination)
  writeUnsavedValue : [ uint32, [uint32,uint32,uint32,uint32]],

  initialize_serial_port : [_void,[_string]],
  close_serial_port : [_void,[]],

   // int32_t readValue (uint32_t p_oid, uint8_t ** buffer, uint32_t bufferSize,uint32_t destination)
  readValue : [ ref.types.int32, [uint32,ref.refType(uint8),uint32,uint32]]
});
