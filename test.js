var addr = Module.findExportByName(null, "check_pin");
console.log("findExportByName: " + addr);
var sym = DebugSymbol.fromName("check_pin");
console.log("DebugSymbol: " + (sym ? sym.address : "null"));
